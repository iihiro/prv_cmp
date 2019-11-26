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
#include <prvc_share/prvc_encdata.hpp>
#include <prvc_eval/prvc_eval_callback_function.hpp>
#include <prvc_eval/prvc_eval_callback_param.hpp>
#include <prvc_eval/prvc_eval_state.hpp>
#include <prvc_eval/prvc_eval_evaluator.hpp>

namespace prvc_eval
{

// CallbackFunctionEncInputX

void CallbackFunctionEncInputX::data_function(uint64_t code,
                                              const stdsc::Buffer& buffer,
                                              stdsc::StateContext& state)
{
    STDSC_LOG_INFO("Received encrypted input X (current state : %lu)",
                   state.current_state());
    STDSC_THROW_CALLBACK_IF_CHECK(
      kStateReady <= state.current_state(),
      "Warn: must be ready state to receive encrypted inptu data.");

    stdsc::BufferStream buffstream(buffer);
    std::iostream stream(&buffstream);

    const auto& context = param_.context();
    param_.enc_input_x_->load_from_stream(stream, context);

    state.set(kEventReceivedEncInputX);

    std::vector<Ctxt> vec_enc_result;
    Ctxt enc_result;
    
    if (kStateComputed == state.current_state()) {
        auto nsplit = param_.enc_input_x_->data().size();
        std::shared_ptr<prvc_eval::Evaluator> eval(new prvc_eval::Evaluator(context, nsplit));
        eval->comparision(*param_.enc_input_x_,
                          *param_.enc_input_y_,
                          vec_enc_result,
                          enc_result);
    }
}
DEFINE_REQUEST_FUNC(CallbackFunctionEncInputX);
DEFINE_DOWNLOAD_FUNC(CallbackFunctionEncInputX);


// CallbackFunctionEncInputY

void CallbackFunctionEncInputY::data_function(uint64_t code,
                                              const stdsc::Buffer& buffer,
                                              stdsc::StateContext& state)
{
    STDSC_LOG_INFO("Received encrypted input Y (current state : %lu)",
                   state.current_state());
    STDSC_THROW_CALLBACK_IF_CHECK(
      kStateReady <= state.current_state(),
      "Warn: must be ready state to receive encrypted inptu data.");

    stdsc::BufferStream buffstream(buffer);
    std::iostream stream(&buffstream);

    const auto& context = param_.context();
    param_.enc_input_y_->load_from_stream(stream, context);

    state.set(kEventReceivedEncInputY);

    std::vector<Ctxt> vec_enc_result;
    Ctxt enc_result;
    
    if (kStateComputed == state.current_state()) {
        auto nsplit = param_.enc_input_x_->data().size();
        std::shared_ptr<prvc_eval::Evaluator> eval(new prvc_eval::Evaluator(context, nsplit));
        eval->comparision(*param_.enc_input_x_,
                          *param_.enc_input_y_,
                          vec_enc_result,
                          enc_result);
    }
}
DEFINE_REQUEST_FUNC(CallbackFunctionEncInputY);
DEFINE_DOWNLOAD_FUNC(CallbackFunctionEncInputY);

} /* namespace prvc_eval */
