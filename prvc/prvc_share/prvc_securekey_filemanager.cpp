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

#include <sstream>
#include <memory>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <prvc_share/prvc_utility.hpp>
#include <prvc_share/prvc_cmp_param_list.hpp>
#include <prvc_share/prvc_enctype.hpp>
#include <prvc_share/prvc_securekey_filemanager.hpp>

#define CHECK_KIND(k) do {                                               \
        if (!((k) < kNumOfKind)) {                                       \
            std::ostringstream oss;                                      \
            oss << "Err: Invalid securekey kind. (kind: " << (k) << ")"; \
            STDSC_THROW_INVPARAM(oss.str().c_str());                     \
        }                                                                \
    } while(0)

namespace prvc_share
{

struct SecureKeyFileManager::Impl
{
    Impl(const std::string& pubkey_filename,
         const std::string& seckey_filename,
         const std::string& context_filename,
         const std::string& emk_filename,
         const std::string& eak_filename,
         const std::size_t mul_depth,
         const std::size_t logN,
         const std::size_t rel_window,
         const std::size_t dcrt_bits)
        : mul_depth_(mul_depth),
          logN_(logN),
          rel_window_(rel_window),
          dcrt_bits_(dcrt_bits)
    {
        filenames_.emplace(kKindPubKey,  pubkey_filename);
        filenames_.emplace(kKindSecKey,  seckey_filename);
        filenames_.emplace(kKindContext, context_filename);
        filenames_.emplace(kKindEMK,     emk_filename);
        filenames_.emplace(kKindEAK,     eak_filename);
    }
    
    ~Impl(void) = default;

    void initialize(void)
    {
        STDSC_LOG_INFO("Generating keys");
        
        double root_hermit = 0.0;
        GetRootHermit(mul_depth_, logN_, rel_window_, dcrt_bits_, root_hermit);

        // Context
        lbcrypto::PlaintextModulus ptm = (1 << logN_) - 1;
        double sigma = DecParam::DefaultSigma;
        
        FHEContext cc;
        cc = lbcrypto::CryptoContextFactory<PolyType>::genCryptoContextBFVrns(
            ptm, root_hermit, sigma, 0, mul_depth_, 0, OPTIMIZED, 2, rel_window_, dcrt_bits_);
        cc->Enable(ENCRYPTION);
        cc->Enable(SHE);

        //lbcrypto::LPKeyPair<PolyType> kp;
        kp_ = cc->KeyGen();
        cc->EvalMultKeysGen(kp_.secretKey);
        STDSC_THROW_FAILURE_IF_CHECK(kp_.good(), "failed to generate key");
        auto N = cc->GetCryptoParameters()->GetElementParams()->GetCyclotomicOrder() / 2;
        
        // Evalkey
        EvkAut eval_automorph_ks;
        vector<usint> index_list{N + 1}; // now just for x^{N+1}
        for (usint i=2; i != N; i = i << 1) {
            index_list.push_back(i + 1);
        }
        index_list.push_back(2 * N - 1);
        eval_automorph_ks = cc->EvalAutomorphismKeyGen(kp_.secretKey, index_list);
        cc->InsertEvalAutomorphismKey(eval_automorph_ks);
        ShowParam(cc);
        
        lbcrypto::Serialized ser_ctx, ser_emk, ser_eak, ser_pub, ser_pri;
        
        std::ofstream ofs_ctx(filename(kKindContext), std::ios::binary | std::ios::trunc);
        STDSC_THROW_FILE_IF_CHECK(cc->Serialize(&ser_ctx), "Error serializing context");
        STDSC_THROW_FILE_IF_CHECK(lbcrypto::SerializableHelper::SerializationToStream(ser_ctx, ofs_ctx),
                                  "Error writing serialized context to file");

        std::ofstream ofs_emk(filename(kKindEMK), std::ios::binary | std::ios::trunc);
        STDSC_THROW_FILE_IF_CHECK(cc->SerializeEvalMultKey(&ser_emk), "Error serializing eval multi key");
        STDSC_THROW_FILE_IF_CHECK(lbcrypto::SerializableHelper::SerializationToStream(ser_emk, ofs_emk),
                                  "Error writing serialized eval multi key to file");
        
        std::ofstream ofs_eak(filename(kKindEAK), std::ios::binary | std::ios::trunc);
        STDSC_THROW_FILE_IF_CHECK(cc->SerializeEvalAutomorphismKey(&ser_eak), "Error serializing eval automorphism key");
        STDSC_THROW_FILE_IF_CHECK(lbcrypto::SerializableHelper::SerializationToStream(ser_eak, ofs_eak),
                                  "Error writing serialized eval automorphism key to file");
        
        std::ofstream ofs_pub(filename(kKindPubKey), std::ios::binary | std::ios::trunc);
        STDSC_THROW_FILE_IF_CHECK(kp_.publicKey->Serialize(&ser_pub), "Error serializing public key");
        STDSC_THROW_FILE_IF_CHECK(lbcrypto::SerializableHelper::SerializationToStream(ser_pub, ofs_pub),
                                  "Error writing serialized public key to file");

        std::ofstream ofs_pri(filename(kKindSecKey), std::ios::binary | std::ios::trunc);
		STDSC_THROW_FILE_IF_CHECK(kp_.secretKey->Serialize(&ser_pri), "Error serializing private key");
        STDSC_THROW_FILE_IF_CHECK(lbcrypto::SerializableHelper::SerializationToStream(ser_pri, ofs_pri),
                                  "Error writing serialized private key to file");
    }
    
    size_t size(const Kind_t kind) const
    {
        CHECK_KIND(kind);
        return prvc_share::utility::file_size(filename(kind));
    }
    
    void data(const Kind_t kind, void* buffer)
    {
        CHECK_KIND(kind);
        auto filename = this->filename(kind);
        
        size_t size = this->size(kind);
        std::ifstream ifs(filename, std::ios::binary);
        if (!ifs.is_open()) {
            std::ostringstream oss;
            oss << "failed to open. (" << filename << ")";
            STDSC_THROW_FILE(oss.str());
        } else {
            ifs.read(reinterpret_cast<char*>(buffer), size);
        }
    }
    
    bool is_exist(const Kind_t kind) const
    {
        CHECK_KIND(kind);
        std::ifstream ifs(filename(kind));
        return ifs.is_open();
    }
    
    std::string filename(const Kind_t kind) const
    {
        CHECK_KIND(kind);
        return filenames_.at(kind);
    }

    const lbcrypto::LPKeyPair<PolyType>& keypair(void) const
    {
        return kp_;
    }

private:
    void ShowParam(const prvc_share::FHEContext& cc)
    {
        std::ostringstream oss;
        oss << "Params: t: " << cc->GetCryptoParameters()->GetPlaintextModulus();
        oss << ", n: "       << cc->GetCryptoParameters()->GetElementParams()->GetCyclotomicOrder() / 2;
        oss << ", logQ: "    << log2(cc->GetCryptoParameters()->GetElementParams()->GetModulus().ConvertToDouble());
        STDSC_LOG_INFO(oss.str().c_str());
    }
    
private:
    std::unordered_map<Kind_t, std::string> filenames_;
    std::size_t mul_depth_;
    std::size_t logN_;
    std::size_t rel_window_;
    std::size_t dcrt_bits_;
    lbcrypto::LPKeyPair<PolyType> kp_;
};

SecureKeyFileManager::SecureKeyFileManager(const std::string& pubkey_filename,
                                           const std::string& seckey_filename,
                                           const std::string& context_filename,
                                           const std::string& emk_filename,
                                           const std::string& eak_filename,
                                           const std::size_t mul_depth,
                                           const std::size_t logN,
                                           const std::size_t rel_window,
                                           const std::size_t dcrt_bits)
    : pimpl_(new Impl(pubkey_filename, seckey_filename, context_filename,
                      emk_filename, eak_filename,
                      mul_depth, logN, rel_window, dcrt_bits))
{
}

void SecureKeyFileManager::initialize(void)
{
    pimpl_->initialize();
}

size_t SecureKeyFileManager::size(const Kind_t kind) const
{
    return pimpl_->size(kind);
}

void SecureKeyFileManager::data(const Kind_t kind, void* buffer)
{
    pimpl_->data(kind, buffer);
}

bool SecureKeyFileManager::is_exist(const Kind_t kind) const
{
    return pimpl_->is_exist(kind);
}

std::string SecureKeyFileManager::filename(const Kind_t kind) const
{
    return pimpl_->filename(kind);
}

const lbcrypto::LPKeyPair<PolyType>& SecureKeyFileManager::keypair(void) const
{
    return pimpl_->keypair();
}

} /* namespace prvc_share */
