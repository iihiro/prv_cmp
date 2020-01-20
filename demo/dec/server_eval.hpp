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

#ifndef SERVER_EVAL_HPP
#define SERVER_EVAL_HPP

#include <stdsc/stdsc_callback_function_container.hpp>
#include <stdsc/stdsc_callback_function.hpp>
#include <stdsc/stdsc_state.hpp>
#include <prvc_share/prvc_packet.hpp>
#include <prvc_dec/prvc_dec_callback_function_eval.hpp>
#include <prvc_dec/prvc_dec_callback_param.hpp>
#include <prvc_dec/prvc_dec_server.hpp>
#include <share/define.hpp>

namespace prvc_demo
{

struct ServerEval
{
    ServerEval(prvc_dec::CallbackParam& param, stdsc::StateContext& state,
               const char* port = DEC_PORT_FOR_EVAL,
               bool is_generate_securekey = false)
    {
        stdsc::CallbackFunctionContainer callback;

        std::shared_ptr<stdsc::CallbackFunction> cb_evk(
            new prvc_dec::eval::CallbackFunctionEVKRequest(param));
        callback.set(prvc_share::kControlCodeDownloadEVK, cb_evk);

        dec_ = std::make_shared<prvc_dec::DecServer>(
          port, callback, state, param.get_skm(), is_generate_securekey);
    }

    ~ServerEval(void) = default;

    void start(void)
    {
        dec_->start();
    }

    void join(void)
    {
        dec_->join();
    }

private:
    std::shared_ptr<prvc_dec::DecServer> dec_;
};

} /* namespace prvc_demo */

#endif /* SERVER_EVAL_HPP */