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
#include <prvc_dec/prvc_dec_enc_param.hpp>
#include <prvc_enc/prvc_enc_dec_client.hpp>

namespace prvc_enc
{
    
template <class T>
struct DecClient<T>::Impl
{
    std::shared_ptr<stdsc::ThreadException> te_;

    Impl(const char* host, const char* port)
        : host_(host), port_(port)
    {
        te_ = stdsc::ThreadException::create();
    }

    void download_pubkey(const char* context_filename, const char* pubkey_filename)
    {
        stdsc::Buffer pubkey;
        client_.recv_data_blocking(prvc_share::kControlCodeDownloadPubkey, pubkey);

        auto data = reinterpret_cast<const char*>(pubkey.data());
        auto* hdr = reinterpret_cast<const prvc_dec::enc::Param*>(data);
        auto hdr_size = sizeof(prvc_dec::enc::Param);

        {
            std::ofstream ofs(context_filename, std::ios::binary);
            ofs.write(data + hdr_size, hdr->ctx_size);
            STDSC_LOG_INFO("Saved a context file. (%s)", context_filename);
        }
        {
            std::ofstream ofs(pubkey_filename, std::ios::binary);
            ofs.write(data + hdr_size + hdr->ctx_size, hdr->key_size);
            STDSC_LOG_INFO("Saved a public key file. (%s)", pubkey_filename);
        }
    }

    void exec(T& args, std::shared_ptr<stdsc::ThreadException> te)
    {
        try
        {
            constexpr uint32_t retry_interval_usec = PRVC_RETRY_INTERVAL_USEC;
            constexpr uint32_t timeout_sec = PRVC_TIMEOUT_SEC;

            STDSC_LOG_INFO("Connecting to Decryptor.");
            client_.connect(host_, port_, retry_interval_usec, timeout_sec);
            STDSC_LOG_INFO("Connected to Decryptor.");

            download_pubkey(args.context_filename.c_str(),
                            args.pubkey_filename.c_str());
        }
        catch (const stdsc::AbstractException& e)
        {
            STDSC_LOG_TRACE("Failed to client process (%s)", e.what());
            te->set_current_exception();
        }
    }

private:
    const char* host_;
    const char* port_;
    stdsc::Client client_;
};

template <class T>
DecClient<T>::DecClient(const char* host, const char* port)
    : pimpl_(new Impl(host, port))
{
}

template <class T>
DecClient<T>::~DecClient(void)
{
    super::join();
}

template <class T>
void DecClient<T>::start(T& param)
{
    super::start(param, pimpl_->te_);
}

template <class T>
void DecClient<T>::wait_for_finish(void)
{
    super::join();
    pimpl_->te_->rethrow_if_has_exception();
}

template <class T>
void DecClient<T>::exec(T& args,
                        std::shared_ptr<stdsc::ThreadException> te) const
{
    try
    {
        pimpl_->exec(args, te);
    }
    catch (...)
    {
        te->set_current_exception();
    }
}

template class DecClient<DecParam>;

} /* namespace prvc_enc */
