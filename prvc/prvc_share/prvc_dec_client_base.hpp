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

#ifndef PRVC_DEC_CLIENT_BASE_HPP
#define PRVC_DEC_CLIENT_BASE_HPP

#include <memory>
#include <string>
#include <prvc_share/prvc_define.hpp>

namespace stdsc
{
class Client;
}

namespace prvc_share
{
    
class PubKey;
class Context;
    
/**
 * @brief Provides client for Decryptor.
 */
class DecClientBase
{
public:
    DecClientBase(const char* host, const char* port);
    virtual ~DecClientBase(void) = default;

    void connect(const uint32_t retry_interval_usec = PRVC_RETRY_INTERVAL_USEC,
                 const uint32_t timeout_sec = PRVC_TIMEOUT_SEC);
    void disconnect();
    
    void get_pubkey(prvc_share::PubKey& pubkey);

    void get_context(prvc_share::Context& context,
                     const bool is_receive_emk=false,
                     const bool is_receive_eak=false);

protected:
    stdsc::Client& client(void);
    
private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace prvc_share */

#endif /* PRVC_DEC_CLIENT_BASE_HPP */
