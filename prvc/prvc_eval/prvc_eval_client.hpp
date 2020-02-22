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

#ifndef PRVC_EVAL_CLIENT_HPP
#define PRVC_EVAL_CLIENT_HPP

#include <memory>
#include <vector>
#include <prvc_share/prvc_define.hpp>
#include <prvc_share/prvc_enctype.hpp>

namespace prvc_share
{
class PubKey;
class Context;
}

namespace prvc_eval
{
    
/**
 * @brief Provides client.
 */
class Client
{
public:
    Client(const char* dec_host,
           const char* dec_port,
           const bool dl_pubkey = true,
           const uint32_t retry_interval_usec = PRVC_RETRY_INTERVAL_USEC,
           const uint32_t timeout_sec = PRVC_TIMEOUT_SEC);
    virtual ~Client(void) = default;

    const prvc_share::Context& context(void) const;

    void send_result(const std::vector<prvc_share::Ctxt>& v_c_cmp_res,
                     const prvc_share::Ctxt& c_cmp_res);

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace prvc_eval */

#endif /* PRVC_EVAL_CLIENT_HPP */
