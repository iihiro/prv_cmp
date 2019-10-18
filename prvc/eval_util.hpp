#ifndef EVAL_UTIL_HPP
#define EVAL_UTIL_HPP

int64_t DecryptAndGetConstantTerm (const FHEContext& cc, const LPKeyPair<PolyType>& key_pair, Ctxt& ctxt) {
  Plaintext poly_res;
  cc->Decrypt(key_pair.secretKey, ctxt, &poly_res);
  return poly_res->GetCoefPackedValue().at(0);
}

inline void EvalAddMany(const FHEContext& cc, const vector<Ctxt>& c_vec, Ctxt& c0) {
  size_t v_len = c_vec.size();
  size_t num_nodes = 2 * v_len - 1;
  vector<Ctxt> c_tree_vec(num_nodes);
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


inline void ExtractConstantTerm (const FHEContext& cc, const Ctxt& c_org, Ctxt& c_constant) {
  usint N = cc->GetCryptoParameters()->GetElementParams()->GetCyclotomicOrder() / 2;
  size_t logN = log2(N);
  auto c_automorph = cc->EvalAutomorphism(c_org, N + 1, cc->GetEvalAutomorphismKeyMap(c_org->GetKeyTag()) );
  c_constant = cc->EvalAdd(c_org, c_automorph);
  for (size_t j = 1; j != logN; ++j) {
    c_automorph = cc->EvalAutomorphism(c_constant,  N/kPof2s[j] + 1, cc->GetEvalAutomorphismKeyMap(c_constant->GetKeyTag()));
    c_constant = cc->EvalAdd(c_constant, c_automorph);
  }
}


inline Ctxt EvalMultMany(const FHEContext& cc, vector<Ctxt>& c_vec) {
  size_t v_len = c_vec.size();
  vector<Ctxt> c_tmp_vec(2 * v_len - 1);
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

#endif // EVAL_UTIL_HPP