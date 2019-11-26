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
#include <prvc_share/prvc_securekey_filemanager.hpp>
#include <prvc_share/prvc_cmp_param_list.hpp>
#include <prvc_share/prvc_enctype.hpp>

#define CHECK_KIND(k) do {                                               \
        if (!((k) < kNumOfDataKind)) {                                   \
            std::ostringstream oss;                                      \
            oss << "Err: Invalid securekey kind. (kind: " << (k) << ")"; \
            STDSC_THROW_INVPARAM(oss.str().c_str());                     \
        }                                                                \
    } while(0)

namespace prvc_share
{

struct SecureKeyFileManager::Impl
{
    Impl(const std::string& context_filename,
         const std::string& pubkey_filename,
         const std::string& seckey_filename)
        : context_filename_(context_filename),
          pubkey_filename_(pubkey_filename),
          seckey_filename_(seckey_filename)
    {
    }
    ~Impl(void) = default;

    void initialize(const std::size_t mul_depth,
                    const std::size_t logN,
                    const std::size_t rel_window,
                    const std::size_t dcrt_bits)
    {
        double root_hermit = 0.0;
        GetRootHermit(mul_depth, logN, rel_window, dcrt_bits, root_hermit);

        // Context gen
        lbcrypto::PlaintextModulus ptm = (1 << logN) - 1;
        double sigma = DefaultSigma;
        
        FHEContext cc;
        cc = lbcrypto::CryptoContextFactory<PolyType>::genCryptoContextBFVrns(
            ptm, root_hermit, sigma, 0, mul_depth, 0, OPTIMIZED, 2, rel_window, dcrt_bits);
        cc->Enable(ENCRYPTION);
        cc->Enable(SHE);

        lbcrypto::LPKeyPair<PolyType> kp;
        kp = cc->KeyGen();
        cc->EvalMultKeysGen(kp.secretKey);
        if ( !kp.good() ) {
            std::cout << "Key generation failed!" << std::endl;
            exit(1);
        }
        auto N = cc->GetCryptoParameters()->GetElementParams()->GetCyclotomicOrder() / 2;
        
        // Evalkey gen.
        EvkAut eval_automorph_ks;
        vector<usint> index_list{N + 1}; // now just for x^{N+1}
        for (usint i=2; i != N; i = i << 1) {
            index_list.push_back(i + 1);
        }
        index_list.push_back(2 * N - 1);
        eval_automorph_ks = cc->EvalAutomorphismKeyGen(kp.secretKey, index_list);
        cc->InsertEvalAutomorphismKey(eval_automorph_ks);
    
        lbcrypto::Serialized ctxK, pubK, privK, emkK;
        
        if( cc->Serialize(&ctxK) ) {
            std::ofstream ofs(context_filename_, std::ios::binary | std::ios::trunc);
            if (!lbcrypto::SerializableHelper::SerializationToStream(ctxK, ofs)) {
				std::cerr << "Error writing serialization of public key to CTX.txt" << std::endl;
				return;
			}
		}
		else {
			std::cerr << "Error serializing context" << std::endl;
			return;
		}
        
        if( kp.publicKey->Serialize(&pubK) ) {
            std::ofstream ofs(pubkey_filename_, std::ios::binary | std::ios::trunc);
            if (!lbcrypto::SerializableHelper::SerializationToStream(pubK, ofs)) {
				std::cerr << "Error writing serialization of public key to PUB.txt" << std::endl;
				return;
			}
		}
		else {
			std::cerr << "Error serializing public key" << std::endl;
			return;
		}

		if( kp.secretKey->Serialize(&privK) ) {
            std::ofstream ofs(seckey_filename_, std::ios::binary | std::ios::trunc);
            if (!lbcrypto::SerializableHelper::SerializationToStream(privK, ofs)) {
				std::cerr << "Error writing serialization of private key to PRI.txt" << std::endl;
				return;
			}
		}
		else {
			std::cerr << "Error serializing private key" << std::endl;
			return;
		}
    }
    
    size_t size(const DataKind_t kind) const
    {
        CHECK_KIND(kind);
        return prvc_share::utility::file_size(this->filename(kind));
    }
    
    void data(const DataKind_t kind, void* buffer)
    {
        CHECK_KIND(kind);
        auto filename = this->filename(kind);
        
        size_t size = this->size(kind);
        std::ifstream ifs(filename, std::ios::binary);
        if (!ifs.is_open())
        {
            std::ostringstream oss;
            oss << "failed to open. (" << filename << ")";
            STDSC_THROW_FILE(oss.str());
        }
        else
        {
            ifs.read(reinterpret_cast<char*>(buffer), size);
        }
    }
    
    bool is_exist(const DataKind_t kind) const
    {
        CHECK_KIND(kind);
        std::ifstream ifs(this->filename(kind));
        return ifs.is_open();
    }
    
    std::string filename(const DataKind_t kind) const
    {
        CHECK_KIND(kind);
        return (kind == kDataKindContext) ?     \
                        context_filename_ :     \
                        ((kind == kDataKindPubKey) ? pubkey_filename_ : seckey_filename_);
    }
    
private:
    std::string context_filename_;
    std::string pubkey_filename_;
    std::string seckey_filename_;
};

SecureKeyFileManager::SecureKeyFileManager(const std::string& context_filename,
                                           const std::string& pubkey_filename,
                                           const std::string& seckey_filename)
    : pimpl_(new Impl(context_filename, pubkey_filename, seckey_filename))
{
}

void SecureKeyFileManager::initialize(const std::size_t mul_depth,
                                      const std::size_t logN,
                                      const std::size_t rel_window,
                                      const std::size_t dcrt_bits)
{
    pimpl_->initialize(mul_depth, logN, rel_window, dcrt_bits);
}

size_t SecureKeyFileManager::size(const DataKind_t kind) const
{
    return pimpl_->size(kind);
}

void SecureKeyFileManager::data(const DataKind_t kind, void* buffer)
{
    pimpl_->data(kind, buffer);
}

bool SecureKeyFileManager::is_exist(const DataKind_t kind) const
{
    return pimpl_->is_exist(kind);
}

std::string SecureKeyFileManager::filename(const DataKind_t kind) const
{
    return pimpl_->filename(kind);
}

} /* namespace prvc_share */
