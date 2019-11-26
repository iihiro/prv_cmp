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
#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_client.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_buffer.hpp>
#include <prvc_share/prvc_packet.hpp>
#include <prvc_dec/prvc_dec_eval_param.hpp>
#include <prvc_eval/prvc_eval_state.hpp>
#include <prvc_eval/prvc_eval_dec_client.hpp>

namespace prvc_eval
{

struct DecClient::Impl
{
    Impl(const char* host, const char* port, stdsc::StateContext& state)
      : state_(state)
    {
        STDSC_LOG_TRACE("Connecting %s@%s", host, port);
        client_.connect(host, port);
        STDSC_LOG_INFO("Connected %s@%s", host, port);
    }

    ~Impl(void) = default;

    void download_evk(const std::string& out_filename)
    {
        stdsc::Buffer context;
        client_.recv_data_blocking(prvc_share::kControlCodeDownloadEVK,
                                   context);

        auto data = reinterpret_cast<const char*>(context.data());
        auto* hdr = reinterpret_cast<const prvc_dec::eval::Param*>(data);
        auto hdr_size = sizeof(prvc_dec::eval::Param);
        
        std::ofstream ofs(out_filename.c_str(), std::ios::binary | std::ios::trunc);
        ofs.write(data + hdr_size, hdr->ctx_size);
        STDSC_LOG_INFO("Saved a context file. (%s)", out_filename.c_str());

        // このステートチェンジをここでやると、prvc_eval::CannbackParamのコンストラクタ内の
        // lbcrypto::CryptoContextFactory<PolyType>::DeserializeAndCreateContext(ser_context)でSEGVる
        //state_.set(kEventSendEVKRequest);
    }

    void upload_enc_result(const stdsc::Buffer& buffer)
    {
        STDSC_LOG_INFO("Sending encrypted result.");
        client_.send_data_blocking(prvc_share::kControlCodeDataEncResult,
                                   buffer);
    }

private:
    stdsc::StateContext& state_;
    stdsc::Client client_;
};

DecClient::DecClient(const char* host, const char* port,
                     stdsc::StateContext& state)
  : pimpl_(new Impl(host, port, state))
{
}

void DecClient::download_evk(const std::string& out_filename)
{
    pimpl_->download_evk(out_filename);
}

void DecClient::upload_enc_result(const stdsc::Buffer& buffer)
{
    pimpl_->upload_enc_result(buffer);
}

} /* prvc_eval */
