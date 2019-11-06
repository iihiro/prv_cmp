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
//#include <iostream>
//#include <fstream>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <prvc_share/prvc_utility.hpp>
#include <prvc_share/prvc_securekey_filemanager.hpp>
#include <prvc_share/prvc_cmp_param_list.hpp>

//#include "FHE.h"
//#include "EncryptedArray.h"

#include "cryptocontexthelper.h"
#include "utils/debug.h"
#include "encoding/encodings.h"
#include "utils/serializablehelper.h"

using PolyType = lbcrypto::DCRTPoly;
using EvkAut   = shared_ptr<map<usint, lbcrypto::LPEvalKey<PolyType>>>;

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
#if 1
        double root_hermit = 0.0;
        GetRootHermit(mul_depth, logN, rel_window, dcrt_bits, root_hermit);

        // Context gen
        lbcrypto::PlaintextModulus ptm = (1 << logN) - 1;
        double sigma = DefaultSigma;
        
        lbcrypto::CryptoContext<PolyType> cc;
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
#else
        FHEcontext context(m_, p_, r_);
        buildModChain(context, L_, c_);

        STDSC_LOG_INFO("Generating secure keys.");

        NTL::ZZX G = context.alMod.getFactorsOverZZ()[0];

        FHESecKey secretKey(context);
        const FHEPubKey& publicKey = secretKey;

        secretKey.GenSecKey(w_);
        addSome1DMatrices(secretKey);

        EncryptedArray ea(context, G);

        if (context.zMStar.numOfGens() != 1) {
            STDSC_THROW_FAILURE("noise increase when it rotates\n");
        }

        for (int i = 0; i < (int)context.zMStar.numOfGens(); i++)
        {
            if (!context.zMStar.SameOrd(i)) {
                STDSC_THROW_FAILURE("noise increase when it rotates\n");
            }
        }

        std::fstream publicFile(pubkey_filename_,
                                std::fstream::out | std::fstream::trunc);
        writeContextBase(publicFile, context);
        publicFile << context << std::std::endl;
        publicFile << publicKey << std::std::endl;
        publicFile.close();

        std::fstream secretFile(seckey_filename_,
                                std::fstream::out | std::fstream::trunc);
        writeContextBase(secretFile, context);
        secretFile << context << std::std::endl;
        secretFile << secretKey << std::std::endl;
        secretFile.close();
#endif
    }

    size_t context_size(void) const
    {
        return prvc_share::utility::file_size(context_filename_);
    }
    
    size_t pubkey_size(void) const
    {
        return prvc_share::utility::file_size(pubkey_filename_);
    }
    
    size_t seckey_size(void) const
    {
        return prvc_share::utility::file_size(seckey_filename_);
    }

    void context_data(void* buffer)
    {
        size_t size = context_size();
        std::ifstream ifs(context_filename_, std::ios::binary);
        if (!ifs.is_open())
        {
            std::ostringstream oss;
            oss << "failed to open. (" << context_filename_ << ")";
            STDSC_THROW_FILE(oss.str());
        }
        else
        {
            ifs.read(reinterpret_cast<char*>(buffer), size);
        }
    }
    
    void pubkey_data(void* buffer)
    {
        size_t size = pubkey_size();
        std::ifstream ifs(pubkey_filename_, std::ios::binary);
        if (!ifs.is_open())
        {
            std::ostringstream oss;
            oss << "failed to open. (" << pubkey_filename_ << ")";
            STDSC_THROW_FILE(oss.str());
        }
        else
        {
            ifs.read(reinterpret_cast<char*>(buffer), size);
        }
    }

    void seckey_data(void* buffer)
    {
        size_t size = seckey_size();
        std::ifstream ifs(seckey_filename_, std::ios::binary);
        if (!ifs.is_open())
        {
            std::ostringstream oss;
            oss << "failed to open. (" << seckey_filename_ << ")";
            STDSC_THROW_FILE(oss.str());
        }
        else
        {
            ifs.read(reinterpret_cast<char*>(buffer), size);
        }
    }

    bool is_exist_context(void) const
    {
        std::ifstream ifs(context_filename_);
        return ifs.is_open();
    }
    
    bool is_exist_pubkey(void) const
    {
        std::ifstream ifs(pubkey_filename_);
        return ifs.is_open();
    }

    bool is_exist_seckey(void) const
    {
        std::ifstream ifs(seckey_filename_);
        return ifs.is_open();
    }

    std::string context_filename(void) const
    {
        return context_filename_;
    }
    
    std::string pubkey_filename(void) const
    {
        return pubkey_filename_;
    }

    std::string seckey_filename(void) const
    {
        return seckey_filename_;
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

size_t SecureKeyFileManager::context_size(void) const
{
    return pimpl_->context_size();
}

size_t SecureKeyFileManager::pubkey_size(void) const
{
    return pimpl_->pubkey_size();
}

size_t SecureKeyFileManager::seckey_size(void) const
{
    return pimpl_->seckey_size();
}

void SecureKeyFileManager::context_data(void* buffer)
{
    pimpl_->context_data(buffer);
}

void SecureKeyFileManager::pubkey_data(void* buffer)
{
    pimpl_->pubkey_data(buffer);
}

void SecureKeyFileManager::seckey_data(void* buffer)
{
    pimpl_->seckey_data(buffer);
}

bool SecureKeyFileManager::is_exist_context(void) const
{
    return pimpl_->is_exist_context();
}

bool SecureKeyFileManager::is_exist_pubkey(void) const
{
    return pimpl_->is_exist_pubkey();
}

bool SecureKeyFileManager::is_exist_seckey(void) const
{
    return pimpl_->is_exist_seckey();
}

std::string SecureKeyFileManager::context_filename(void) const
{
    return pimpl_->context_filename();
}

std::string SecureKeyFileManager::pubkey_filename(void) const
{
    return pimpl_->pubkey_filename();
}

std::string SecureKeyFileManager::seckey_filename(void) const
{
    return pimpl_->seckey_filename();
}

} /* namespace prvc_share */
