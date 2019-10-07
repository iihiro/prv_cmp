#include "cryptocontexthelper.h"
#include "utils/debug.h"
#include "encoding/encodings.h"

using namespace std;
using namespace lbcrypto;
using PolyType = DCRTPoly;
using Ctxt = Ciphertext<PolyType>;
using EvkAut = shared_ptr<map<usint, LPEvalKey<PolyType>>>;
using FHEContext = CryptoContext<PolyType>;

constexpr usint kPof2s[15] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384}; // 2^j
Plaintext test_poly_;
Plaintext poly_constant_invtwo_;
Plaintext poly_constant_one_;

#include "cmp_param_list.hpp"
#include "poly_util.hpp"
#include "eval_util.hpp"
vector<Ctxt> ca_times_cbs_;
vector<Ctxt> v_c_equ_res_;
vector<Ctxt> v_c_nonequ_res_;

constexpr uint32_t kRelWindow = 0;
constexpr size_t kDcrtBits = 60;
constexpr size_t kLogN = 13;

void ShowParam (const FHEContext& cc) {
  cout << "Params:" << endl;
  cout << "\t t: " << cc->GetCryptoParameters()->GetPlaintextModulus() << endl;
  cout << "\t n: " << cc->GetCryptoParameters()->GetElementParams()->GetCyclotomicOrder() / 2 << endl;
  cout << "\t logQ: " << log2(cc->GetCryptoParameters()->GetElementParams()->GetModulus().ConvertToDouble()) << endl;
}

void SetContextAndKeys (
 const unsigned int mul_depth,
 const usint t,
 const double root_hermit,
 FHEContext& cc,
 LPKeyPair<PolyType>& kp,
 EvkAut& eval_automorph_ks
 )
{
  // Context gen.
  cc = CryptoContextFactory<PolyType>::genCryptoContextBFVrns(t, root_hermit, 3.2, 0, mul_depth, 0, OPTIMIZED, 2, kRelWindow, kDcrtBits);
  cc->Enable(ENCRYPTION);
  cc->Enable(SHE);

  // Key gen.
  kp = cc->KeyGen();
  cc->EvalMultKeysGen(kp.secretKey);
  if ( !kp.good() ) {
    cout << "Key generation failed!" << endl;
    exit(1);
  }
  auto N = cc->GetCryptoParameters()->GetElementParams()->GetCyclotomicOrder() / 2;
  // Evalkey gen.
  vector<usint> index_list{N + 1}; // now just for x^{N+1}
  for (usint i=2; i != N; i = i << 1) {
    index_list.push_back(i + 1);
  }
  index_list.push_back(2 * N - 1);
  eval_automorph_ks = cc->EvalAutomorphismKeyGen(kp.secretKey, index_list);
  cc->InsertEvalAutomorphismKey(eval_automorph_ks);
}

void PreComputeConstatPolys(const FHEContext& cc) {
  auto N = cc->GetCryptoParameters()->GetElementParams()->GetCyclotomicOrder() / 2;
  auto t = cc->GetCryptoParameters()->GetPlaintextModulus();
  // T(X) gen
  vector<int64_t> dense_coeffs(N, 0);
  for (size_t i = 0; i < N; ++i) {
    dense_coeffs[i] = static_cast<int64_t>(t+1) / 2 - static_cast<int64_t>(t);
    //dense_coeffs[i] = 1;
  }
  test_poly_ = cc->MakeCoefPackedPlaintext(dense_coeffs);

  // 2^{-1}
  vector<int64_t> twoinv_coeffs(N, 0);
  twoinv_coeffs[0] = static_cast<int64_t>(t+1) / 2 - static_cast<int64_t>(t);
  poly_constant_invtwo_ = cc->MakeCoefPackedPlaintext(twoinv_coeffs);

  // 1
  vector<int64_t> one_coeffs(N, 0);
  one_coeffs[0] = 1;
  poly_constant_one_ = cc->MakeCoefPackedPlaintext(one_coeffs);
}

void SplitValues (
 const size_t numbit,
 const size_t bit_per_chunk,
 const uint64_t a,
 const uint64_t b, 
 size_t& numint,
 vector<uint64_t>& as,
 vector<uint64_t>& bs
) {
  if (numbit <= bit_per_chunk) {
    numint = 1;
	  as.push_back(a);
	  bs.push_back(b);
  } else {
    numint = ceil(static_cast<double>(numbit)/bit_per_chunk);
    uint64_t mask = (1 << (bit_per_chunk + 1)) - 1;
    uint64_t a_tmp;
    uint64_t b_tmp;
    for (size_t i = 0; i < numint; ++i) {
      a_tmp = a >> (i * bit_per_chunk);
      b_tmp = b >> (i * bit_per_chunk);
      as.push_back(a_tmp & mask);
      bs.push_back(b_tmp & mask);
    }
  }
  ca_times_cbs_.resize(numint);
  v_c_equ_res_.resize(numint - 1);
  v_c_nonequ_res_.resize(numint - 1);
}

void EncodeAndEncryptSplittedValues (
 const FHEContext& cc,
 const LPKeyPair<PolyType>& key_pair,
 const size_t numint,
 vector<uint64_t>& as,
 vector<uint64_t>& bs,
 vector<Ctxt>& c_as,
 vector<Ctxt>& c_bs) {
  const usint N = cc->GetCryptoParameters()->GetElementParams()->GetCyclotomicOrder() / 2;
  for (size_t i = 0; i < numint; ++i) {
    vector<int64_t> coeffs_a(N, 0);
	  vector<int64_t> coeffs_b(N, 0);
	  MakeMonomialCoeff(as[i], N, false, coeffs_a);
	  MakeMonomialCoeff(bs[i], N, true, coeffs_b);
    auto poly_a = cc->MakeCoefPackedPlaintext(coeffs_a);
    auto poly_b = cc->MakeCoefPackedPlaintext(coeffs_b);
	  // cout << "polya" << endl;
	  // ShowPolyInfo(N, poly_a);
	  // cout << "polyb" << endl;
	  // ShowPolyInfo(N, poly_b);
    auto c_monomial_a = cc->Encrypt(key_pair.publicKey, poly_a);
    auto c_monomial_b = cc->Encrypt(key_pair.publicKey, poly_b);
	  c_as.push_back(c_monomial_a);
	  c_bs.push_back(c_monomial_b);
  }
}

template<class T>
void ShowVec (const vector<T>& v) {
  size_t e = v.size();
  cout << "[";
  for (size_t i = 0, e = v.size(); i < e-1; ++i) {
    cout << v[i] << ",";
  }
  cout << v[e-1] << "]" << endl;
}

inline void MultByTestPoly (
 const FHEContext& cc,
 const size_t chunk_id,
 const Ctxt& c_a,
 const Ctxt& c_b,
 Ctxt& c_mult_mono) {
	ca_times_cbs_[chunk_id] = cc->EvalMult(c_a, c_b);
	c_mult_mono = cc->EvalMult(ca_times_cbs_[chunk_id], test_poly_);
  c_mult_mono = cc->EvalAdd(c_mult_mono, poly_constant_invtwo_);
}

inline void NegateBit (const FHEContext& cc, const Ctxt& c_src, Ctxt& c_res) {
  c_res = cc->EvalNegate(c_src);
  c_res = cc->EvalAdd(c_res, poly_constant_one_);
}

inline void Comparison (
 const FHEContext& cc,
 const size_t num_chunk,
 const vector<Ctxt>& v_ca,
 const vector<Ctxt>& v_cb,
 vector<Ctxt>& v_cres) {
  if (num_chunk > 1) {
    // from Top chunk to bottom
    for (size_t i = 0; i < num_chunk - 1; ++i)  {
      // cout << "Chunk [" << i << "]" << endl;
      Ctxt c_mult_monomial;
      MultByTestPoly(cc, i, v_ca[i], v_cb[i], c_mult_monomial);
      ExtractConstantTerm(cc, c_mult_monomial, v_cres[i]); // GT
      ExtractConstantTerm(cc, ca_times_cbs_[i], v_c_equ_res_[i]); // EQU
      NegateBit(cc, v_c_equ_res_[i], v_c_nonequ_res_[i]);
  	}
    // cout << "Chunk [" << num_chunk - 1 << "]" << endl;
    Ctxt c_mult_monomial;
    MultByTestPoly(cc, num_chunk - 1, v_ca[num_chunk - 1], v_cb[num_chunk - 1], c_mult_monomial);
    ExtractConstantTerm(cc, c_mult_monomial, v_cres[num_chunk - 1]); // GT

  } else { // # chunk == 1
    Ctxt c_mult_monomial;
    MultByTestPoly(cc, 0, v_ca[0], v_cb[0], c_mult_monomial);
    ExtractConstantTerm(cc, c_mult_monomial, v_cres[0]);
  }
}

inline void GreaterThanUnroll (
 const FHEContext& cc,
 const size_t num_chunk,
 const vector<Ctxt>& c_gt_res,
 Ctxt& c_res) {
  vector<Ctxt> v_add(num_chunk);
  size_t itr_cnt = num_chunk - 1;
  v_add[0] = cc->EvalMult(c_gt_res[0], v_c_nonequ_res_[0]);
  for (size_t i = 1; i != itr_cnt; ++i) {
    vector<Ctxt> v_single_column(i + 2);
    v_single_column[0] = c_gt_res[i];
    v_single_column[1] = v_c_nonequ_res_[i];
    for (size_t j = 0; j < i; ++j) {
      v_single_column[j + 2] = v_c_equ_res_[j];
    }
    v_add[i] = EvalMultMany(cc, v_single_column);
  }
  vector<Ctxt> v_last_column(num_chunk);
  v_last_column[0] = c_gt_res[num_chunk - 1];
  for (size_t j = 1; j < num_chunk; ++j) {
    v_last_column[j] = v_c_equ_res_[j - 1];
  }
  v_add[num_chunk - 1] = EvalMultMany(cc, v_last_column);
  EvalAddMany(cc, v_add, c_res);
}

int main (void) {
// offline
  size_t num_chunk = 1;
  size_t num_bit = 30;
  unsigned int mul_depth = 4;

  FHEContext context;
  LPKeyPair<PolyType> key_pair;
  EvkAut eval_automorph_keys;


  double root_hermit = 0.0;
  usint t = (1 << kLogN) - 1;
  GetRootHermit(mul_depth, kLogN, kRelWindow, kDcrtBits, root_hermit);
  SetContextAndKeys(mul_depth, t, root_hermit, context, key_pair, eval_automorph_keys);
  ShowParam(context);
  PreComputeConstatPolys(context);
  

// online
  uint64_t x = 10;
  uint64_t y = 6;
  vector<uint64_t> x_chunks, y_chunks;
  SplitValues(num_bit, kLogN, x, y,  num_chunk, x_chunks, y_chunks);
  cout << "\t # bit: " << num_bit << endl;
  cout << "\t # chunks: " << num_chunk << endl;
	
  vector<Ctxt> c_x_chunks, c_y_chunks, c_res_chunks;
  EncodeAndEncryptSplittedValues(context, key_pair, num_chunk, x_chunks, y_chunks, c_x_chunks, c_y_chunks);
  for (size_t i = 0; i < num_chunk; ++i) {
    cout << "x[" << i << "]=" << x_chunks[i] << ", y[" << i << "]=" << y_chunks[i] << endl;
  }
  vector<Ctxt> v_c_cmp_res(num_chunk);
  Ctxt c_cmp_res;
  double start = currentDateTime();
  Comparison(context, num_chunk, c_x_chunks, c_y_chunks, v_c_cmp_res);
  if (num_chunk > 1) {
    GreaterThanUnroll(context, num_chunk, v_c_cmp_res, c_cmp_res);
  } else {
	  c_cmp_res = v_c_cmp_res[0];
	}
	double end = currentDateTime();

	cout << "Timing for Comparison: "<< end - start  << " [ms]" << endl;
	for (size_t i = 0; i < num_chunk; ++i) {
    int64_t dec_constant = DecryptAndGetConstantTerm(context, key_pair, v_c_cmp_res[i]);
    cout << "Comparison Result on " << i << "-th chunck: " << dec_constant << endl;
	}
  int64_t dec_constant = DecryptAndGetConstantTerm(context, key_pair, c_cmp_res);
  cout << "Overall Comparison Result (x<=y)?: " << dec_constant << endl;
  return 0;
}
