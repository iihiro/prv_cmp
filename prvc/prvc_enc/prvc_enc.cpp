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

#include <memory>
#include <fstream>
#include <vector>
#include <cstring>
#include <cmath>
#include <stdsc/stdsc_client.hpp>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <prvc_share/prvc_packet.hpp>
#include <prvc_share/prvc_define.hpp>
#include <prvc_share/prvc_pubkey.hpp>
#include <prvc_share/prvc_context.hpp>
#include <prvc_share/prvc_encdata.hpp>
#include <prvc_share/prvc_encparam.hpp>
#include <prvc_share/prvc_plaindata.hpp>
#include <prvc_share/prvc_decparam.hpp>
#include <prvc_enc/prvc_enc_dec_client.hpp>
#include <prvc_enc/prvc_enc_eval_client.hpp>
#include <prvc_enc/prvc_enc.hpp>

namespace prvc_enc
{

static uint64_t NormalizeValue(const int64_t val,
                               const size_t bitlen)
{
    if (bitlen > 64 || bitlen == 0) {
        std::ostringstream oss;
        oss << "bitlen is invalid value. (bitlen: " << bitlen
            << ")" << std::endl;
        STDSC_THROW_INVPARAM(oss.str());
    }

    double signedmax = std::pow(2, bitlen-1);
    auto offset = static_cast<uint64_t>(signedmax);
    
    return val + offset;
}
   
static void SplitValues(const size_t bitlen,
                        const size_t bit_per_chunk,
                        const uint64_t x,
                        size_t& numint,
                        vector<uint64_t>& xs)
{
    if (bitlen <= bit_per_chunk) {
        numint = 1;
        xs.push_back(x);
    } else {
        numint = ceil(static_cast<double>(bitlen) / bit_per_chunk);
        uint64_t mask = (1 << (bit_per_chunk)) - 1;
        uint64_t x_tmp;
        for (size_t i = 0; i < numint; ++i) {
            x_tmp = x >> (i * bit_per_chunk);
            xs.push_back(x_tmp & mask);
        }
    }
}

static void MakeMonomialCoeff(const usint d,
                              const usint N,
                              const bool is_neg,
                              vector<int64_t>& coeff)
{
    if (d > N) {
        std::ostringstream oss;
        oss << "invalid param size (d:" << d;
        oss << ", N:" << N << ")";
        STDSC_THROW_INVPARAM(oss.str().c_str());
    }

    STDSC_THROW_INVPARAM_IF_CHECK(coeff.size() > 0, "invalid coeff size");

    if (d == N) {
        coeff[0] = -1;
        return;
    }

    if (d == 0) {
        coeff[0] = 1;
        return;
    }

    if (is_neg) {
        coeff[N - d] = -1;
    } else {
        coeff[d] = 1;
    }
}

    
struct Encryptor::Impl
{
    Impl(const char* dec_host, const char* dec_port,
         const char* eval_host, const char* eval_port,
         const bool is_neg_mononical_coef,
         const bool dl_pubkey,
         const uint32_t retry_interval_usec,
         const uint32_t timeout_sec)
        : dec_client_(new DecClient(dec_host, dec_port)),
          eval_client_(new EvalClient(eval_host, eval_port)),
          is_neg_mononical_coef_(is_neg_mononical_coef),
          retry_interval_usec_(retry_interval_usec),
          timeout_sec_(timeout_sec)
    {
        STDSC_IF_CHECK(dl_pubkey, "False of dl_pubkey is not supported yet.");

        dec_client_->connect(retry_interval_usec_, timeout_sec_);
        eval_client_->connect(retry_interval_usec_, timeout_sec_);

        context_ = std::make_shared<prvc_share::Context>();
        dec_client_->get_context(*context_);
        
        pubkey_ = std::make_shared<prvc_share::PubKey>(context_->get());
        dec_client_->get_pubkey(*pubkey_);

        dec_client_->get_param(dec_param_);
        
        STDSC_LOG_INFO("param: logN:%lu, bit_len:%lu",
                       dec_param_.logN, dec_param_.bit_len);
    }

    ~Impl(void)
    {
        dec_client_->disconnect();
    }

    void compute(const int64_t val)
    {
        auto norm_val = NormalizeValue(val, dec_param_.bit_len);

        size_t num_chunk = 1;
        vector<uint64_t> x_chunks;
        SplitValues(dec_param_.bit_len, dec_param_.logN, norm_val, num_chunk, x_chunks);

        std::reverse(x_chunks.begin(), x_chunks.end());
        
        {
            std::ostringstream oss;
            oss << "splitted values (n=" << num_chunk << "): ";
            for (size_t i=0; i<x_chunks.size(); ++i) {
                oss << i << ":" << x_chunks[i] << " ";
            }
            STDSC_LOG_INFO(oss.str().c_str());
        }

        auto& cc = context_->get();
        const usint N = cc->GetCryptoParameters()->GetElementParams()->GetCyclotomicOrder() / 2;

        std::vector<lbcrypto::Plaintext> vptxt;
        
        for (size_t i=0; i<num_chunk; ++i) {
            STDSC_LOG_DEBUG("num_chunk:%lu, x_chunks[%lu]=%lu, N=%lu", num_chunk, i, x_chunks[i], N);
            vector<int64_t> coeffs_a(N, 0);
            MakeMonomialCoeff(x_chunks[i], N, is_neg_mononical_coef_, coeffs_a);
            auto poly_a = cc->MakeCoefPackedPlaintext(coeffs_a);
            vptxt.push_back(poly_a);
        }

        prvc_share::EncData encdata(*context_.get());
        encdata.encrypt(vptxt, *pubkey_.get());

#if 0 //test
        encdata.save_to_file("encinput.txt");

        prvc_share::EncData encdata2(*context_.get());
        encdata2.load_from_file("encinput.txt");
        encdata2.save_to_file("encinput2.txt");
#endif

        prvc_share::EncParam param;
        param.num_chunk = num_chunk;

        prvc_share::PlainData<prvc_share::EncParam> plaindata;
        plaindata.push(param);

        eval_client_->send_input(encdata, plaindata);
    }

private:
    std::shared_ptr<DecClient> dec_client_;
    std::shared_ptr<EvalClient> eval_client_;
    prvc_share::DecParam dec_param_;
    const bool is_neg_mononical_coef_;
    const uint32_t retry_interval_usec_;
    const uint32_t timeout_sec_;
    std::shared_ptr<prvc_share::Context> context_;
    std::shared_ptr<prvc_share::PubKey> pubkey_;
};

Encryptor::Encryptor(const char* dec_host, const char* dec_port,
                     const char* eval_host, const char* eval_port,
                     const bool is_neg_mononical_coef,
                     const bool dl_pubkey,
                     const uint32_t retry_interval_usec,
                     const uint32_t timeout_sec)
    : pimpl_(new Impl(dec_host, dec_port, eval_host, eval_port,
                      is_neg_mononical_coef,
                      dl_pubkey, retry_interval_usec, timeout_sec))
{
}

void Encryptor::compute(const int64_t val)
{
    pimpl_->compute(val);
}

} /* namespace prvc_enc */
