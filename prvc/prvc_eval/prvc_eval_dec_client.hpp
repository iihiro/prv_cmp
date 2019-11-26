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

#ifndef PRVC_EVAL_DEC_CLIENT_HPP
#define PRVC_EVAL_DEC_CLIENT_HPP

#include <memory>

namespace stdsc
{
class StateContext;
class Buffer;
}

namespace prvc_eval
{

/**
 * @brief Provides client for Decryptor.
 */
class DecClient
{
    //friend class CallbackFunctionComputeRequest;

public:
    DecClient(const char* host, const char* port, stdsc::StateContext& state);
    virtual ~DecClient(void) = default;

    void download_evk(const std::string& out_filename="context.txt");

private:
    void upload_enc_result(const stdsc::Buffer& buffer);

    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace prvc_eval */

#endif /*PRVC_EVAL_DEC_CLIENT_HPP*/
