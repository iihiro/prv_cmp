/*
 * Copyright 2018 Yamana Laboratory, Waseda University
 * Supported by JST CREST Grant Number JPMJCR1503, Japan.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fstream>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_log.hpp>
#include <prvc_share/prvc_encdata.hpp>
#include <prvc_eval/prvc_eval_evaluator.hpp>

namespace prvc_eval
{

constexpr usint kPof2s[15] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384}; // 2^j
    
static inline
void EvalAddMany(const FHEContext& cc, const vector<Ctxt>& c_vec, Ctxt& c0) {
    size_t v_len = c_vec.size();
    size_t num_nodes = 2 * v_len - 1;
    std::vector<Ctxt> c_tree_vec(num_nodes);
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

static inline
void ExtractConstantTerm (const FHEContext& cc, const Ctxt& c_org, Ctxt& c_constant)
{
    usint N = cc->GetCryptoParameters()->GetElementParams()->GetCyclotomicOrder() / 2;
    size_t logN = log2(N);
    auto c_automorph = cc->EvalAutomorphism(c_org, N + 1,
                                            cc->GetEvalAutomorphismKeyMap(c_org->GetKeyTag()) );
    c_constant = cc->EvalAdd(c_org, c_automorph);
    for (size_t j = 1; j != logN; ++j) {
        c_automorph = cc->EvalAutomorphism(c_constant,  N/kPof2s[j] + 1,
                                           cc->GetEvalAutomorphismKeyMap(c_constant->GetKeyTag()));
        c_constant = cc->EvalAdd(c_constant, c_automorph);
    }
}    

static inline
Ctxt EvalMultMany(const FHEContext& cc, vector<Ctxt>& c_vec) {
    size_t v_len = c_vec.size();
    std::vector<Ctxt> c_tmp_vec(2 * v_len - 1);
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
    
struct Evaluator::Impl
{
    Impl(const FHEContext& context, const size_t nsplit)
        : context_(context),
          nsplit_(nsplit)
    {
        ca_times_cbs_.resize(nsplit);
        v_c_equ_res_.resize(nsplit - 1);
        v_c_nonequ_res_.resize(nsplit - 1);

        //
        // pre compute constant polys
        //
        auto N = context_->GetCryptoParameters()->GetElementParams()->GetCyclotomicOrder() / 2;
        auto t = context_->GetCryptoParameters()->GetPlaintextModulus();
        // T(X) gen
        vector<int64_t> dense_coeffs(N, 0);
        for (size_t i = 0; i < N; ++i) {
            dense_coeffs[i] = static_cast<int64_t>(t+1) / 2 - static_cast<int64_t>(t);
        }
        test_poly_ = context_->MakeCoefPackedPlaintext(dense_coeffs);

        // 2^{-1}
        vector<int64_t> twoinv_coeffs(N, 0);
        twoinv_coeffs[0] = static_cast<int64_t>(t+1) / 2 - static_cast<int64_t>(t);
        poly_constant_invtwo_ = context_->MakeCoefPackedPlaintext(twoinv_coeffs);

        // 1
        vector<int64_t> one_coeffs(N, 0);
        one_coeffs[0] = 1;
        poly_constant_one_ = context_->MakeCoefPackedPlaintext(one_coeffs);
    }

    ~Impl(void) = default;

    void comparision(const prvc_share::EncData& enc_input_x,
                     const prvc_share::EncData& enc_input_y,
                     std::vector<Ctxt>& vec_enc_result,
                     Ctxt& enc_result)
    {
        const size_t num_chunk = nsplit_;
        const auto& v_ca = enc_input_x.data();
        const auto& v_cb = enc_input_y.data();
        
        STDSC_LOG_DEBUG("a\n");
        if (!(v_ca.size() == nsplit_ && v_cb.size() == nsplit_)) {
            std::ostringstream oss;
            oss << "encryped input x/y size is invalid. (enc_input_x: "
                << v_ca.size()
                << "), (enc_input_y: "
                << v_cb.size()
                << ")";
            STDSC_THROW_INVPARAM(oss.str().c_str());
        }

        STDSC_LOG_DEBUG("start comparision. (num_chunk:%u)\n", num_chunk);
        if (num_chunk > 1) {
            // from Top chunk to bottom
            for (size_t i = 0; i < num_chunk - 1; ++i)  {
                STDSC_LOG_DEBUG("chunk[%u]\n", i);
                Ctxt c_mult_monomial;
                mult_by_test_poly(i, v_ca[i], v_cb[i], c_mult_monomial);
                STDSC_LOG_DEBUG("1\n");
                ExtractConstantTerm(context_, c_mult_monomial, vec_enc_result[i]); // GT
                STDSC_LOG_DEBUG("2\n");
                ExtractConstantTerm(context_, ca_times_cbs_[i], v_c_equ_res_[i]); // EQU
                STDSC_LOG_DEBUG("3\n");
                
                //NegateBit(cc, v_c_equ_res_[i], v_c_nonequ_res_[i]);
                v_c_nonequ_res_[i] = context_->EvalNegate(v_c_equ_res_[i]);
                v_c_nonequ_res_[i] = context_->EvalAdd(v_c_nonequ_res_[i], poly_constant_one_);
            }
            STDSC_LOG_DEBUG("chunk[%u]\n", num_chunk-1);
            Ctxt c_mult_monomial;
            mult_by_test_poly(num_chunk-1, v_ca[num_chunk-1], v_cb[num_chunk-1], c_mult_monomial);
            ExtractConstantTerm(context_, c_mult_monomial, vec_enc_result[num_chunk - 1]); // GT
        } else { // # chunk == 1
            Ctxt c_mult_monomial;
            mult_by_test_poly(0, v_ca[0], v_cb[0], c_mult_monomial);
            ExtractConstantTerm(context_, c_mult_monomial, vec_enc_result[0]);
        }
        STDSC_LOG_DEBUG("b\n");

        if (num_chunk > 1) {
            greater_than_unroll(vec_enc_result, enc_result);
        } else {
            enc_result = vec_enc_result[0];
        }
        STDSC_LOG_DEBUG("c\n");
    }

private:

void mult_by_test_poly(const size_t chunk_id,
                       const Ctxt& c_a,
                       const Ctxt& c_b,
                       Ctxt& c_mult_mono)
{
    STDSC_LOG_DEBUG("11\n");
    // 2019.11.26:
    // 2つのencrptorからenc_input_x/yをevaluatorがもらって、
    // ここを実行する際に、以下のエラーが発生する
    //
    // terminate called after throwing an instance of 'std::logic_error'
    //     what():  You need to use EvalMultKeyGen so that you have an EvalMultKey available for this ID
    //
    // 場所: ~/palisade-release/src/pke/lib/cryptocontext.cpp : L154
    //
    // 原因は、contextの実体であるCryptoContextImplがstatic変数としてもつ
    // evalSumKeyMapがおそらく空のため。
    // このマップは、もとの実装(prvc_cmp.org/demo/comparision.cpp)では、DecryptorでSetContextAndKeys()内で
    // cc->EvalMultKeysGen()を読んだタイミングで作られているが、
    // Evaluatorへcontextをシリアライズして送った段階で、static変数のため保持されていなくて、
    // この状態になっていると推測される。
    
	ca_times_cbs_[chunk_id] = context_->EvalMult(c_a, c_b);
    STDSC_LOG_DEBUG("12\n");
	c_mult_mono = context_->EvalMult(ca_times_cbs_[chunk_id], test_poly_);
    STDSC_LOG_DEBUG("13\n");
    c_mult_mono = context_->EvalAdd(c_mult_mono, poly_constant_invtwo_);
}

void greater_than_unroll(const vector<Ctxt>& c_gt_res, Ctxt& c_res)
{
    const size_t num_chunk = nsplit_;
    
    std::vector<Ctxt> v_add(num_chunk);
    size_t itr_cnt = num_chunk - 1;
    v_add[0] = context_->EvalMult(c_gt_res[0], v_c_nonequ_res_[0]);
    for (size_t i = 1; i != itr_cnt; ++i) {
        std::vector<Ctxt> v_single_column(i + 2);
        v_single_column[0] = c_gt_res[i];
        v_single_column[1] = v_c_nonequ_res_[i];
        for (size_t j = 0; j < i; ++j) {
            v_single_column[j + 2] = v_c_equ_res_[j];
        }
        v_add[i] = EvalMultMany(context_, v_single_column);
    }
    vector<Ctxt> v_last_column(num_chunk);
    v_last_column[0] = c_gt_res[num_chunk - 1];
    for (size_t j = 1; j < num_chunk; ++j) {
        v_last_column[j] = v_c_equ_res_[j - 1];
    }
    v_add[num_chunk - 1] = EvalMultMany(context_, v_last_column);
    EvalAddMany(context_, v_add, c_res);
}
    
    
    
private:
    const FHEContext& context_;
    const size_t nsplit_;
    vector<Ctxt> ca_times_cbs_;
    vector<Ctxt> v_c_equ_res_;
    vector<Ctxt> v_c_nonequ_res_;
    lbcrypto::Plaintext test_poly_;
    lbcrypto::Plaintext poly_constant_invtwo_;
    lbcrypto::Plaintext poly_constant_one_;
};

Evaluator::Evaluator(const FHEContext& context, const size_t nsplit)
    : pimpl_(new Impl(context, nsplit))
{
}

void Evaluator::comparision(const prvc_share::EncData& enc_input_x,
                            const prvc_share::EncData& enc_input_y,
                            std::vector<Ctxt>& vec_enc_result,
                            Ctxt& enc_result)
{
    pimpl_->comparision(enc_input_x, enc_input_y, vec_enc_result, enc_result);
}


} /* prvc_eval */
