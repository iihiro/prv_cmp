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

#include <fstream>
#include <vector>
#include <cstring>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_socket.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_log.hpp>
#include <prvc_share/prvc_packet.hpp>
#include <prvc_share/prvc_securekey_filemanager.hpp>
#include <prvc_dec/prvc_dec_callback_function_eval.hpp>
#include <prvc_dec/prvc_dec_callback_param.hpp>
#include <prvc_dec/prvc_dec_state_eval.hpp>
#include <prvc_dec/prvc_dec_eval_param.hpp>


namespace prvc_dec
{
namespace eval
{

// CallbackFunctionEVKRequest

void CallbackFunctionEVKRequest::download_function(
  uint64_t code, const stdsc::Socket& sock, stdsc::StateContext& state)
{
    STDSC_LOG_INFO("Received evk key request. (current state : %lu)",
                   state.current_state());
    STDSC_THROW_CALLBACK_IF_CHECK(
      kStateReady <= state.current_state(),
      "Warn: must be ready state to receive evk request.");

    auto& skm = param_.get_skm();
    Param hdr = {skm.size(SKM_DATAKIND_CONTEXT)};

    auto hdr_size   = sizeof(Param);
    auto total_size = hdr_size + hdr.ctx_size;
    stdsc::Buffer context(total_size);

    auto* p = static_cast<uint8_t*>(context.data());
    std::memcpy(p, &hdr, hdr_size);
    skm.data(SKM_DATAKIND_CONTEXT, p + hdr_size);
    STDSC_LOG_INFO("Sending evk.");
    sock.send_packet(stdsc::make_data_packet(prvc_share::kControlCodeDataEVK,
                                             total_size));
    sock.send_buffer(context);
    
    state.set(kEventReceivedEVKReq);
}
DEFINE_REQUEST_FUNC(CallbackFunctionEVKRequest);
DEFINE_DATA_FUNC(CallbackFunctionEVKRequest);

} /* namespace eval */
} /* namespace prvc_dec */
