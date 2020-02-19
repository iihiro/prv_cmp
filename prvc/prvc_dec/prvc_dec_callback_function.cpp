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
#include <prvc_dec/prvc_dec_callback_function.hpp>
#include <prvc_dec/prvc_dec_callback_param.hpp>
#include <prvc_dec/prvc_dec_state.hpp>
//#include <prvc_dec/prvc_dec_enc_param.hpp>


namespace prvc_dec
{

// CallbackFunctionPubkeyRequest
DEFUN_DOWNLOAD(CallbackFunctionPubkeyRequest)
{
    STDSC_LOG_INFO("Received public key request. (current state : %s)",
                   state.current_state_str().c_str());

    DEF_CDATA_ON_EACH(prvc_dec::CallbackParam);

    //auto  kind = prvc_share::SecureKeyFileManager::Kind_t::kKindPubKey;
    //auto& skm  = *cdata_e->skm_ptr;
    //stdsc::Buffer pubkey(skm.size(kind));
    //skm.data(kind, pubkey.data());
    //STDSC_LOG_INFO("Sending public key.");
    //sock.send_packet(stdsc::make_data_packet(nbc_share::kControlCodeDataPubkey,
    //                                         skm.size(kind)));
    //sock.send_buffer(pubkey);
    //state.set(kEventPubkeyRequest);
}

} /* namespace prvc_dec */
