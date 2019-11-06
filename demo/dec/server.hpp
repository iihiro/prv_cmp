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

#ifndef SERVER_HPP
#define SERVER_HPP

#include <stdsc/stdsc_callback_function_container.hpp>
#include <stdsc/stdsc_callback_function.hpp>
#include <stdsc/stdsc_state.hpp>
#include <prvc_share/prvc_packet.hpp>
#include <prvc_dec/prvc_dec_callback_function.hpp>
#include <prvc_dec/prvc_dec_callback_param.hpp>
#include <prvc_dec/prvc_dec_thread.hpp>
#include <share/define.hpp>

namespace prvc_demo
{

struct Server
{
    Server(prvc_dec::CallbackParam& param, stdsc::StateContext& state,
           const char* port = DEC_PORT_FOR_ENC,
           bool is_generate_securekey = false)
    {
        stdsc::CallbackFunctionContainer callback;

        std::shared_ptr<stdsc::CallbackFunction> cb_conn(
          new prvc_dec::CallbackFunctionRequestConnect(param));
        callback.set(prvc_share::kControlCodeRequestConnect, cb_conn);

        std::shared_ptr<stdsc::CallbackFunction> cb_disconn(
          new prvc_dec::CallbackFunctionRequestDisconnect(param));
        callback.set(prvc_share::kControlCodeRequestDisconnect, cb_disconn);

        std::shared_ptr<stdsc::CallbackFunction> cb_pubkey(
          new prvc_dec::CallbackFunctionPubkeyRequest(param));
        callback.set(prvc_share::kControlCodeDownloadPubkey, cb_pubkey);

        std::shared_ptr<stdsc::CallbackFunction> cb_result(
          new prvc_dec::CallbackFunctionEncResult(param));
        callback.set(prvc_share::kControlCodeDataEncResult, cb_result);

        dec_ = std::make_shared<prvc_dec::DecThread>(
          port, callback, state, param.get_skm(), is_generate_securekey);
    }

    ~Server(void) = default;

    void start(void)
    {
        dec_->start();
    }

    void join(void)
    {
        dec_->join();
    }

private:
    std::shared_ptr<prvc_dec::DecThread> dec_;
};

} /* namespace prvc_demo */

#endif /* SERVER_HPP */
