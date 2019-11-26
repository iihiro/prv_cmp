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

#include <prvc_share/prvc_enctype.hpp>
#include <prvc_share/prvc_encdata.hpp>
#include <prvc_share/prvc_utility.hpp>
#include <prvc_share/prvc_encutility.hpp>
#include <prvc_eval/prvc_eval_dec_client.hpp>
#include <prvc_eval/prvc_eval_callback_param.hpp>

namespace prvc_eval
{

CallbackParam::CallbackParam(const std::string& context_filepath, DecClient& client)
    : client_(client),
      //context_(new FHEContext()),
      enc_input_x_(new prvc_share::EncData()),
      enc_input_y_(new prvc_share::EncData())
{
    if (!prvc_share::utility::file_exist(context_filepath)) {
        client_.download_evk(context_filepath.c_str());
    }
    lbcrypto::Serialized ser_context;
    STDSC_THROW_FILE_IF_CHECK(
        prvc_share::encutility::read_from_file(context_filepath, ser_context),
        "failed to read context file");
    //*context_ = lbcrypto::CryptoContextFactory<PolyType>::DeserializeAndCreateContext(ser_context)
    context_ = lbcrypto::CryptoContextFactory<PolyType>::DeserializeAndCreateContext(ser_context);
}

const FHEContext& CallbackParam::context() const
{
    //return *context_;
    return context_;
}

} /* namespace prvc_eval */
