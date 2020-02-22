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
#include <prvc_share/prvc_dec_client_base.hpp>

namespace prvc_share
{
    
struct DecClientBase::Impl
{
    Impl(const char* host, const char* port)
        : host_(host),
          port_(port)
    {}

    ~Impl(void)
    {
        disconnect();
    }

    void connect(const uint32_t retry_interval_usec,
                 const uint32_t timeout_sec)
    {
        STDSC_LOG_INFO("Connecting to Decryptor.");
        client_.connect(host_, port_, retry_interval_usec, timeout_sec);
        STDSC_LOG_INFO("Connected to Decryptor.");
    }

    void disconnect(void)
    {
        client_.close();
    }
    
    void get_pubkey(prvc_share::PubKey& pubkey)
    {
        stdsc::Buffer buffer;
        client_.recv_data_blocking(prvc_share::kControlCodeDownloadPubkey, buffer);

        stdsc::BufferStream buffstream(buffer);
        std::iostream stream(&buffstream);
        pubkey.load_from_stream(stream);
    }

    void get_context(prvc_share::Context& context,
                     const bool is_receive_emk,
                     const bool is_receive_eak)
    {
        stdsc::Buffer buffer;
        client_.recv_data_blocking(prvc_share::kControlCodeDownloadContext, buffer);

        stdsc::BufferStream buffstream(buffer);
        std::iostream stream(&buffstream);
        context.load_from_stream(stream);

        if (is_receive_emk) {
            stdsc::Buffer buffer;
            client_.recv_data_blocking(prvc_share::kControlCodeDownloadEMK, buffer);

            stdsc::BufferStream buffstream(buffer);
            std::iostream stream(&buffstream);
            context.load_from_stream(stream, Context::kKindEMK);
        }

        if (is_receive_eak) {
            stdsc::Buffer buffer;
            client_.recv_data_blocking(prvc_share::kControlCodeDownloadEAK, buffer);

            stdsc::BufferStream buffstream(buffer);
            std::iostream stream(&buffstream);
            context.load_from_stream(stream, Context::kKindEAK);
        }
    }

    stdsc::Client& client(void)
    {
        return client_;
    }

private:
    const char* host_;
    const char* port_;    
    stdsc::Client client_;
};

DecClientBase::DecClientBase(const char* host, const char* port)
  : pimpl_(new Impl(host, port))
{
}

void DecClientBase::connect(const uint32_t retry_interval_usec,
                       const uint32_t timeout_sec)
{
    pimpl_->connect(retry_interval_usec, timeout_sec);
}

void DecClientBase::disconnect(void)
{
    pimpl_->disconnect();
}
    
void DecClientBase::get_pubkey(prvc_share::PubKey& pubkey)
{
    pimpl_->get_pubkey(pubkey);
}

void DecClientBase::get_context(prvc_share::Context& context,
                                const bool is_receive_emk,
                                const bool is_receive_eak)
{
    pimpl_->get_context(context, is_receive_emk, is_receive_eak);
}

stdsc::Client& DecClientBase::client(void)
{
    return pimpl_->client();
}
    
} /* namespace prvc_share */
