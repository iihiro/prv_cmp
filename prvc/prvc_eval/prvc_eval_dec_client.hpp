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

#ifndef PRVC_EVAL_DEC_CLIENT_HPP
#define PRVC_EVAL_DEC_CLIENT_HPP

#include <memory>
#include <string>
#include <prvc_share/prvc_dec_client_base.hpp>

namespace prvc_share
{
class EncData;
}

namespace prvc_eval
{
    
/**
 * @brief Provides client for Decryptor.
 */
class DecClient : public prvc_share::DecClientBase
{
    using super = prvc_share::DecClientBase;
    
public:
    DecClient(const char* host, const char* port);
    virtual ~DecClient(void) = default;

    void send_result(const prvc_share::EncData& vcmpres,
                     const prvc_share::EncData& cmpres);
    
private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};
    
} /* namespace prvc_eval */

#endif /* PRVC_EVAL_DEC_CLIENT_HPP */
