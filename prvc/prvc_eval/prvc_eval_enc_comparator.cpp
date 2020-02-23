/*
 * Copyright 2018 Yamana Laboratory, Waseda University
 * Supported by JST CREST Grant Number JPMJCR1503, Japan.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE‐2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdsc/stdsc_exception.hpp>
#include <prvc_share/prvc_enctype.hpp>
#include <prvc_share/prvc_encdata.hpp>
#include <prvc_eval/prvc_eval_enc_comparator.hpp>

namespace prvc_eval
{

static void MultByTestPoly(const prvc_share::FHEContext& cc,
                           const size_t chunk_id,
                           const prvc_share::Ctxt& c_a,
                           const prvc_share::Ctxt& c_b,
                           const lbcrypto::Plaintext& test_poly,
                           const lbcrypto::Plaintext& poly_constant_invtwo,
                           vector<prvc_share::Ctxt>& ca_times_cbs,
                           prvc_share::Ctxt& c_mult_mono)
{
	ca_times_cbs[chunk_id] = cc->EvalMult(c_a, c_b);
	c_mult_mono = cc->EvalMult(ca_times_cbs[chunk_id], test_poly);
    c_mult_mono = cc->EvalAdd(c_mult_mono, poly_constant_invtwo);
}

static void ExtractConstantTerm(const prvc_share::FHEContext& cc,
                                const prvc_share::Ctxt& c_org,
                                prvc_share::Ctxt& c_constant)
{
    constexpr usint kPof2s[15] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384}; // 2^j

    usint N = cc->GetCryptoParameters()->GetElementParams()->GetCyclotomicOrder() / 2;
    size_t logN = log2(N);
    auto c_automorph = cc->EvalAutomorphism(c_org, N + 1, cc->GetEvalAutomorphismKeyMap(c_org->GetKeyTag()) );
    auto a = cc->EvalAdd(c_org, c_automorph);
    c_constant = cc->EvalAdd(c_org, c_automorph);
    for (size_t j = 1; j != logN; ++j) {
        c_automorph = cc->EvalAutomorphism(c_constant,  N/kPof2s[j] + 1, cc->GetEvalAutomorphismKeyMap(c_constant->GetKeyTag()));
        c_constant = cc->EvalAdd(c_constant, c_automorph);
    }
}

static void NegateBit(const prvc_share::FHEContext& cc,
                      const prvc_share::Ctxt& c_src,
                      const lbcrypto::Plaintext& poly_constant_one,
                      prvc_share::Ctxt& c_res)
{
    c_res = cc->EvalNegate(c_src);
    c_res = cc->EvalAdd(c_res, poly_constant_one);
}

static void Comparison(const prvc_share::FHEContext& cc,
                       const size_t num_chunk,
                       const vector<prvc_share::Ctxt>& v_ca,
                       const vector<prvc_share::Ctxt>& v_cb,
                       vector<prvc_share::Ctxt>& v_c_equ_res,
                       vector<prvc_share::Ctxt>& v_c_nonequ_res,
                       vector<prvc_share::Ctxt>& v_cres)
{
    vector<prvc_share::Ctxt> ca_times_cbs(num_chunk);

    v_c_equ_res.resize(num_chunk - 1);
    v_c_nonequ_res.resize(num_chunk - 1);

    auto N = cc->GetCryptoParameters()->GetElementParams()->GetCyclotomicOrder() / 2;
    auto t = cc->GetCryptoParameters()->GetPlaintextModulus();
    // T(X) gen
    vector<int64_t> dense_coeffs(N, 0);
    for (size_t i = 0; i < N; ++i) {
        dense_coeffs[i] = static_cast<int64_t>(t+1) / 2 - static_cast<int64_t>(t);
        //dense_coeffs[i] = 1;
    }
    lbcrypto::Plaintext test_poly = cc->MakeCoefPackedPlaintext(dense_coeffs);

    // 2^{-1}
    std::vector<int64_t> twoinv_coeffs(N, 0);
    twoinv_coeffs[0] = static_cast<int64_t>(t+1) / 2 - static_cast<int64_t>(t);
    lbcrypto::Plaintext poly_constant_invtwo = cc->MakeCoefPackedPlaintext(twoinv_coeffs);

    // 1
    std::vector<int64_t> one_coeffs(N, 0);
    one_coeffs[0] = 1;
    lbcrypto::Plaintext poly_constant_one = cc->MakeCoefPackedPlaintext(one_coeffs);

    if (num_chunk > 1) {
        // from Top chunk to bottom
        for (size_t i = 0; i < num_chunk - 1; ++i)  {

            // cout << "Chunk [" << i << "]" << endl;
            prvc_share::Ctxt c_mult_monomial;
            MultByTestPoly(cc, i, v_ca[i], v_cb[i],
                           test_poly, poly_constant_invtwo, ca_times_cbs,
                           c_mult_monomial);

            ExtractConstantTerm(cc, c_mult_monomial, v_cres[i]); // GT
            ExtractConstantTerm(cc, ca_times_cbs[i], v_c_equ_res[i]); // EQU

            NegateBit(cc, v_c_equ_res[i], poly_constant_one, v_c_nonequ_res[i]);
        }
        // cout << "Chunk [" << num_chunk - 1 << "]" << endl;
        prvc_share::Ctxt c_mult_monomial;
        MultByTestPoly(cc, num_chunk - 1, v_ca[num_chunk - 1], v_cb[num_chunk - 1],
                       test_poly, poly_constant_invtwo, ca_times_cbs,
                       c_mult_monomial);
        ExtractConstantTerm(cc, c_mult_monomial, v_cres[num_chunk - 1]); // GT
    } else { // # chunk == 1
        prvc_share::Ctxt c_mult_monomial;
        MultByTestPoly(cc, 0, v_ca[0], v_cb[0],
                       test_poly, poly_constant_invtwo, ca_times_cbs,
                       c_mult_monomial);
        ExtractConstantTerm(cc, c_mult_monomial, v_cres[0]);
    }
}

static prvc_share::Ctxt EvalMultMany(const prvc_share::FHEContext& cc,
                                     std::vector<prvc_share::Ctxt>& c_vec)
{
    std::size_t v_len = c_vec.size();
    std::vector<prvc_share::Ctxt> c_tmp_vec(2 * v_len - 1);
    size_t itr = 0;
    for(size_t i = 0; i < v_len; ++i) {
        c_tmp_vec[i] = c_vec[itr++];
    }

    size_t result_index = v_len;
    for(size_t i = 0, e = c_tmp_vec.size(); i < e - 1; i = i + 2) {
        c_tmp_vec[result_index++] = cc->EvalMult(c_tmp_vec[i], c_tmp_vec[i + 1]);
    }
    return c_tmp_vec.back();
}

static void EvalAddMany(const prvc_share::FHEContext& cc,
                        const std::vector<prvc_share::Ctxt>& c_vec,
                        prvc_share::Ctxt& c0)
{
    size_t v_len = c_vec.size();
    size_t num_nodes = 2 * v_len - 1;
    std::vector<prvc_share::Ctxt> c_tree_vec(num_nodes);
    size_t itr = 0;
    for(size_t i = 0; i < v_len; ++i) {
        c_tree_vec[i] = c_vec[itr++];
    }
    size_t result_index = v_len;
    for(size_t i = 0, e = c_tree_vec.size(); i < e - 1; i = i + 2) {
        c_tree_vec[result_index++] = cc->EvalAdd(c_tree_vec[i], c_tree_vec[i + 1]);
    }
    c0 = c_tree_vec.back();
}

static void GreaterThanUnroll(const prvc_share::FHEContext& cc,
                              const size_t num_chunk,
                              const std::vector<prvc_share::Ctxt>& v_c_equ_res,
                              const vector<prvc_share::Ctxt>& v_c_nonequ_res,
                              const std::vector<prvc_share::Ctxt>& c_gt_res,
                              prvc_share::Ctxt& c_res)
{
    std::vector<prvc_share::Ctxt> v_add(num_chunk);
    size_t itr_cnt = num_chunk - 1;
    v_add[0] = cc->EvalMult(c_gt_res[0], v_c_nonequ_res[0]);
    for (size_t i = 1; i != itr_cnt; ++i) {
        std::vector<prvc_share::Ctxt> v_single_column(i + 2);
        v_single_column[0] = c_gt_res[i];
        v_single_column[1] = v_c_nonequ_res[i];
        for (size_t j = 0; j < i; ++j) {
            v_single_column[j + 2] = v_c_equ_res[j];
        }
        v_add[i] = EvalMultMany(cc, v_single_column);
    }
    std::vector<prvc_share::Ctxt> v_last_column(num_chunk);
    v_last_column[0] = c_gt_res[num_chunk - 1];
    for (size_t j = 1; j < num_chunk; ++j) {
        v_last_column[j] = v_c_equ_res[j - 1];
    }
    v_add[num_chunk - 1] = EvalMultMany(cc, v_last_column);
    EvalAddMany(cc, v_add, c_res);
}
    
struct EncComparator::Impl
{
    Impl(void) = default;
    ~Impl(void) = default;

    void initialize(void)
    {
        vencdata_.clear();
    }
    
    void push(const prvc_share::EncData& encdata)
    {
        if (is_comparable()) {
            vencdata_.erase(vencdata_.begin());
        }
        vencdata_.push_back(encdata);
    }

    bool is_comparable(void) const
    {
        return vencdata_.size() >= 2;
    }

    void compare(const prvc_share::FHEContext& context,
                 const size_t num_chunk,
                 std::vector<prvc_share::Ctxt>& v_c_cmp_res,
                 prvc_share::Ctxt& c_cmp_res) const
    {
        STDSC_IF_CHECK(is_comparable(), "data is too few");
        
        const auto& c_x_chunks = vencdata_.at(0).vdata();
        const auto& c_y_chunks = vencdata_.at(1).vdata();

        vector<prvc_share::Ctxt> v_c_equ_res;
        vector<prvc_share::Ctxt> v_c_nonequ_res;
        Comparison(context, num_chunk, c_x_chunks, c_y_chunks,
                   v_c_equ_res, v_c_nonequ_res, v_c_cmp_res);

        if (num_chunk > 1) {
            GreaterThanUnroll(context, num_chunk,
                              v_c_equ_res,v_c_nonequ_res,
                              v_c_cmp_res, c_cmp_res);
        } else {
            c_cmp_res = v_c_cmp_res[0];
        }
    }

private:
    std::vector<prvc_share::EncData> vencdata_;
};

EncComparator::EncComparator(void)
    : pimpl_(new Impl())
{}

void EncComparator::initialize(void)
{
    pimpl_->initialize();
}

void EncComparator::push(const prvc_share::EncData& encdata)
{
    pimpl_->push(encdata);
}

bool EncComparator::is_comparable(void) const
{
    return pimpl_->is_comparable();
}

void EncComparator::compare(const prvc_share::FHEContext& context,
                            const size_t num_chunk,
                            std::vector<prvc_share::Ctxt>& v_c_cmp_res,
                            prvc_share::Ctxt& c_cmp_res) const
{
    pimpl_->compare(context, num_chunk, v_c_cmp_res, c_cmp_res);
}

} /* namespace prvc_eval */
