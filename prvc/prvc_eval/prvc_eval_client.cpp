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
#include <stdsc/stdsc_state.hpp>
#include <prvc_share/prvc_packet.hpp>
#include <prvc_share/prvc_define.hpp>
#include <prvc_share/prvc_pubkey.hpp>
#include <prvc_share/prvc_context.hpp>
#include <prvc_share/prvc_encdata.hpp>
#include <prvc_eval/prvc_eval_dec_client.hpp>
#include <prvc_eval/prvc_eval_client.hpp>

namespace prvc_eval
{
    
struct Client::Impl
{
    Impl(const char* dec_host,
         const char* dec_port,
         const bool dl_pubkey,
         const uint32_t retry_interval_usec,
         const uint32_t timeout_sec)
        : dec_client_(new DecClient(dec_host, dec_port)),
          retry_interval_usec_(retry_interval_usec),
          timeout_sec_(timeout_sec)
    {
        STDSC_IF_CHECK(dl_pubkey, "False of dl_pubkey is not supported yet.");

        dec_client_->connect(retry_interval_usec_, timeout_sec_);

        bool is_receive_emk = true;
        bool is_receive_eak = true;
        
        context_ = std::make_shared<prvc_share::Context>();
        dec_client_->get_context(*context_, is_receive_emk, is_receive_eak);
    }

    ~Impl(void)
    {
        dec_client_->disconnect();
    }

    const prvc_share::Context& context(void) const
    {
        return *context_;
    }

    void send_result(const std::vector<prvc_share::Ctxt>& v_c_cmp_res,
                     const prvc_share::Ctxt& c_cmp_res)
    {
        prvc_share::EncData vcmpres(context());
        prvc_share::EncData cmpres(context());

        for (const auto& c : v_c_cmp_res) {
            vcmpres.push(c);
        }
        cmpres.push(c_cmp_res);
        
        dec_client_->send_result(vcmpres, cmpres);
    }

private:
    std::shared_ptr<DecClient> dec_client_;
    const uint32_t retry_interval_usec_;
    const uint32_t timeout_sec_;
    std::shared_ptr<prvc_share::Context> context_;
};

Client::Client(const char* dec_host,
               const char* dec_port,
               const bool dl_pubkey,
               const uint32_t retry_interval_usec,
               const uint32_t timeout_sec)
    : pimpl_(new Impl(dec_host, dec_port,
                      dl_pubkey, retry_interval_usec, timeout_sec))
{
}

const prvc_share::Context& Client::context(void) const
{
    return pimpl_->context();
}

void Client::send_result(const std::vector<prvc_share::Ctxt>& v_c_cmp_res,
                         const prvc_share::Ctxt& c_cmp_res)
{
    pimpl_->send_result(v_c_cmp_res, c_cmp_res);
}

} /* namespace prvc_eval */
