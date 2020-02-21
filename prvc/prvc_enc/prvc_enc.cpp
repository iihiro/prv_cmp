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
#include <prvc_enc/prvc_enc_dec_client.hpp>
#include <prvc_enc/prvc_enc.hpp>

namespace prvc_enc
{

static void SplitValues(const size_t numbit,
                        const size_t bit_per_chunk,
                        const uint64_t a,
                        size_t& numint,
                        vector<uint64_t>& as)
{
    if (numbit <= bit_per_chunk) {
        numint = 1;
        as.push_back(a);
    } else {
        numint = ceil(static_cast<double>(numbit) / bit_per_chunk);
        uint64_t mask = (1 << (bit_per_chunk + 1)) - 1;
        uint64_t a_tmp;
        for (size_t i = 0; i < numint; ++i) {
            a_tmp = a >> (i * bit_per_chunk);
            as.push_back(a_tmp & mask);
        }
    }
    // Decryptor側へnumint(num_chunk)を渡して、そちらでresizeする
    //ca_times_cbs_.resize(numint);
    //v_c_equ_res_.resize(numint - 1);
    //v_c_nonequ_res_.resize(numint - 1);
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
          is_neg_mononical_coef_(is_neg_mononical_coef),
          retry_interval_usec_(retry_interval_usec),
          timeout_sec_(timeout_sec)
    {
        STDSC_IF_CHECK(dl_pubkey, "False of dl_pubkey is not supported yet.");

        dec_client_->connect(retry_interval_usec_, timeout_sec_);

        context_ = std::make_shared<prvc_share::Context>();
        dec_client_->get_context(*context_);
        
        pubkey_ = std::make_shared<prvc_share::PubKey>(context_->get());
        dec_client_->get_pubkey(*pubkey_);

        // test
        //context_->save_to_file("context-enc.txt");
        //pubkey_->save_to_file("pubkey-enc.txt");
    }

    ~Impl(void)
    {
        dec_client_->disconnect();
    }

    void compute(const uint64_t val, const size_t logN, const size_t num_bit)
    {
        size_t num_chunk = 1;
        
        vector<uint64_t> x_chunks;
        SplitValues(num_bit, logN, val, num_chunk, x_chunks);
        STDSC_LOG_DEBUG("split values: bit:%lu, chunks:%lu",
                        num_bit, num_chunk);

        auto& cc = context_->get();
        const usint N = cc->GetCryptoParameters()->GetElementParams()->GetCyclotomicOrder() / 2;

        std::vector<lbcrypto::Plaintext> vptxt;
        
        for (size_t i=0; i<num_chunk; ++i) {
            vector<int64_t> coeffs_a(N, 0);
            MakeMonomialCoeff(x_chunks[i], N, is_neg_mononical_coef_, coeffs_a);
            auto poly_a = cc->MakeCoefPackedPlaintext(coeffs_a);
            vptxt.push_back(poly_a);
        }

        prvc_share::EncData encdata(*context_.get());
        encdata.encrypt(vptxt, *pubkey_.get());

#if 1 //test
        encdata.save_to_file("encinput.txt");

        prvc_share::EncData encdata2(*context_.get());
        encdata2.load_from_file("encinput.txt");
        encdata2.save_to_file("encinput2.txt");
#endif
    }

private:
    std::shared_ptr<DecClient> dec_client_;
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

void Encryptor::compute(const uint64_t val, const size_t logN, const size_t num_bit)
{
    pimpl_->compute(val, logN, num_bit);
}

} /* namespace prvc_enc */