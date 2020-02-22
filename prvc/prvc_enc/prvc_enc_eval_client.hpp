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

#ifndef PRVC_ENC_EVAL_CLINET_HPP
#define PRVC_ENC_EVAL_CLINET_HPP

#include <memory>
#include <string>
#include <vector>
#include <prvc_share/prvc_define.hpp>

namespace prvc_share
{
class EncData;
class EncParam;
template <class T> class PlainData;
}

namespace prvc_enc
{

/**
 * @brief Provides client for Evaluator.
 */
class EvalClient
{
public:
    EvalClient(const char* host, const char* port);
    virtual ~EvalClient(void) = default;

    void connect(const uint32_t retry_interval_usec = PRVC_RETRY_INTERVAL_USEC,
                 const uint32_t timeout_sec = PRVC_TIMEOUT_SEC);
    void disconnect();
    
    void send_input(const prvc_share::EncData& encdata,
                    const prvc_share::PlainData<prvc_share::EncParam>& plaindata);

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace prvc_enc */

#endif /* PRVC_ENC_EVAL_CLINET_HPP */
