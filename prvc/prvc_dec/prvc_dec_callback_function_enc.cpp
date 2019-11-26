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
#include <prvc_dec/prvc_dec_callback_function_enc.hpp>
#include <prvc_dec/prvc_dec_callback_param.hpp>
#include <prvc_dec/prvc_dec_state_enc.hpp>
#include <prvc_dec/prvc_dec_enc_param.hpp>


namespace prvc_dec
{
namespace enc
{

// CallbackFunctionPubkeyRequest

void CallbackFunctionPubkeyRequest::download_function(
  uint64_t code, const stdsc::Socket& sock, stdsc::StateContext& state)
{
    STDSC_LOG_INFO("Received public key request. (current state : %lu)",
                   state.current_state());
    STDSC_THROW_CALLBACK_IF_CHECK(
      kStateReady <= state.current_state(),
      "Warn: must be ready state to receive pubkey request.");

    auto& skm = param_.get_skm();
    Param hdr = {skm.size(SKM_DATAKIND_CONTEXT), skm.size(SKM_DATAKIND_PUBKEY)};

    auto hdr_size   = sizeof(Param);
    auto total_size = hdr_size + hdr.ctx_size + hdr.key_size;
    stdsc::Buffer pubkey(total_size);

    auto* p = static_cast<uint8_t*>(pubkey.data());
    std::memcpy(p, &hdr, hdr_size);
    skm.data(SKM_DATAKIND_CONTEXT, p + hdr_size);
    skm.data(SKM_DATAKIND_PUBKEY, p + hdr_size + hdr.ctx_size);
    STDSC_LOG_INFO("Sending public key.");
    sock.send_packet(stdsc::make_data_packet(prvc_share::kControlCodeDataPubkey,
                                             total_size));
    sock.send_buffer(pubkey);
    
    state.set(kEventReceivedPubkeyReq);
}
DEFINE_REQUEST_FUNC(CallbackFunctionPubkeyRequest);
DEFINE_DATA_FUNC(CallbackFunctionPubkeyRequest);

} /* namespace enc */
} /* namespace prvc_dec */
