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
#include <prvc_share/prvc_context.hpp>
#include <prvc_share/prvc_encdata.hpp>
#include <prvc_share/prvc_plaindata.hpp>
#include <prvc_dec/prvc_dec_callback_function.hpp>
#include <prvc_dec/prvc_dec_callback_param.hpp>
#include <prvc_dec/prvc_dec_state.hpp>
#include <prvc_dec/prvc_dec_result.hpp>

namespace prvc_dec
{

static int64_t DecryptAndGetConstantTerm(const prvc_share::FHEContext& cc,
                                         const prvc_share::FHESecKey& seckey,
                                         const prvc_share::Ctxt& ctxt)
{
    lbcrypto::Plaintext poly_res;
    cc->Decrypt(seckey, ctxt, &poly_res);
    return poly_res->GetCoefPackedValue().at(0);
}
    
// CallbackFunctionPubkeyRequest
DEFUN_DOWNLOAD(CallbackFunctionPubkeyRequest)
{
    STDSC_LOG_INFO("Received public key request. (current state : %s)",
                   state.current_state_str().c_str());

    DEF_CDATA_ON_EACH(prvc_dec::CallbackParam);

    auto  kind = prvc_share::SecureKeyFileManager::Kind_t::kKindPubKey;
    auto& skm  = *cdata_e->skm_ptr;
    stdsc::Buffer pubkey(skm.size(kind));
    skm.data(kind, pubkey.data());
    STDSC_LOG_INFO("Sending public key.");
    sock.send_packet(stdsc::make_data_packet(prvc_share::kControlCodeDataPubkey,
                                             skm.size(kind)));
    sock.send_buffer(pubkey);
    state.set(kEventPubkeyRequest);
}

// CallbackFunctionContextRequest
DEFUN_DOWNLOAD(CallbackFunctionContextRequest)
{
    STDSC_LOG_INFO("Received context request. (current state : %s)",
                   state.current_state_str().c_str());

    DEF_CDATA_ON_EACH(prvc_dec::CallbackParam);

    auto  kind = prvc_share::SecureKeyFileManager::Kind_t::kKindContext;
    auto& skm  = *cdata_e->skm_ptr;
    stdsc::Buffer context(skm.size(kind));
    skm.data(kind, context.data());
    STDSC_LOG_INFO("Sending context.");
    sock.send_packet(stdsc::make_data_packet(prvc_share::kControlCodeDataContext,
                                             skm.size(kind)));
    sock.send_buffer(context);
    state.set(kEventContextRequest);
}

// CallbackFunctionEMKRequest
DEFUN_DOWNLOAD(CallbackFunctionEMKRequest)
{
    STDSC_LOG_INFO("Received emk request. (current state : %s)",
                   state.current_state_str().c_str());

    DEF_CDATA_ON_EACH(prvc_dec::CallbackParam);

    auto  kind = prvc_share::SecureKeyFileManager::Kind_t::kKindEMK;
    auto& skm  = *cdata_e->skm_ptr;
    stdsc::Buffer emk(skm.size(kind));
    skm.data(kind, emk.data());
    STDSC_LOG_INFO("Sending emk.");
    sock.send_packet(stdsc::make_data_packet(prvc_share::kControlCodeDataEMK,
                                             skm.size(kind)));
    sock.send_buffer(emk);
    state.set(kEventEMKRequest);
}

// CallbackFunctionEAKRequest
DEFUN_DOWNLOAD(CallbackFunctionEAKRequest)
{
    STDSC_LOG_INFO("Received eak request. (current state : %s)",
                   state.current_state_str().c_str());

    DEF_CDATA_ON_EACH(prvc_dec::CallbackParam);

    auto  kind = prvc_share::SecureKeyFileManager::Kind_t::kKindEAK;
    auto& skm  = *cdata_e->skm_ptr;
    stdsc::Buffer eak(skm.size(kind));
    skm.data(kind, eak.data());
    STDSC_LOG_INFO("Sending eak.");
    sock.send_packet(stdsc::make_data_packet(prvc_share::kControlCodeDataEAK,
                                             skm.size(kind)));
    sock.send_buffer(eak);
    state.set(kEventEAKRequest);
}

// CallbackFunctionParamRequest
DEFUN_DOWNLOAD(CallbackFunctionParamRequest)
{
    STDSC_LOG_INFO("Received param request. (current state : %s)",
                   state.current_state_str().c_str());

    DEF_CDATA_ON_EACH(prvc_dec::CallbackParam);

    prvc_share::PlainData<prvc_share::DecParam> plaindata;
    plaindata.push(cdata_e->param);

    auto sz = plaindata.stream_size();
    stdsc::BufferStream buffstream(sz);
    std::iostream stream(&buffstream);

    plaindata.save_to_stream(stream);

    STDSC_LOG_INFO("Sending param.");
    stdsc::Buffer* buffer = &buffstream;
    sock.send_packet(stdsc::make_data_packet(prvc_share::kControlCodeDataDecParam, sz));
    sock.send_buffer(*buffer);
    state.set(kEventParamRequest);
}

// CallbackFunctionDecryptRequest
DEFUN_DATA(CallbackFunctionEncResult)
{
    STDSC_LOG_INFO("Received enc result. (current state : %s)",
                   state.current_state_str().c_str());

    DEF_CDATA_ON_EACH(prvc_dec::CallbackParam);
    DEF_CDATA_ON_ALL(prvc_dec::CommonCallbackParam);

    auto& skm     = *cdata_e->skm_ptr;
    auto& context = *cdata_e->context_ptr;
    auto& vresult = cdata_a->vresult;

    stdsc::BufferStream buffstream(buffer);
    std::iostream stream(&buffstream);

    prvc_share::EncData vcmpres(context);
    prvc_share::EncData cmpres(context);

    vcmpres.load_from_stream(stream);
    cmpres.load_from_stream(stream);

    const auto& v_c_cmp_res = vcmpres.vdata();
    const auto& c_cmp_res   = cmpres.data();

    const auto& cc = context.get();

    auto& sk = skm.keypair().secretKey;

    prvc_dec::DecResult result;

    STDSC_LOG_INFO("Decrypt enc result.");
    
    for (size_t i=0; i<v_c_cmp_res.size(); ++i) {
        const auto& ctxt = v_c_cmp_res[i];
        int64_t dec_constant = DecryptAndGetConstantTerm(cc, sk, ctxt);
        STDSC_LOG_INFO("Comparison Result on %lu -th chunk: %ld",
                       i, dec_constant);
        result.vdec.push_back(dec_constant);
    }
    int64_t dec_constant = DecryptAndGetConstantTerm(cc, sk, c_cmp_res);
    STDSC_LOG_INFO("Overall Comparison Result (x<=y)?: %ld",
                   dec_constant);
    result.dec = dec_constant;

    vresult.push_back(result);

    fprintf(stderr, "%ld\n", dec_constant);
}

} /* namespace prvc_dec */
