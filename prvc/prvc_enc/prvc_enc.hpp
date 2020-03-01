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

#ifndef PRVC_ENC_HPP
#define PRVC_ENC_HPP

#include <memory>
#include <vector>
#include <prvc_share/prvc_define.hpp>

namespace prvc_enc
{
    
/**
 * @brief Provides encryptor.
 */
class Encryptor
{
public:
    
    Encryptor(const char* dec_host, const char* dec_port,
              const char* eval_host, const char* eval_port,
              const bool is_neg_mononical_coef,
              const bool dl_pubkey = true,
              const uint32_t retry_interval_usec = PRVC_RETRY_INTERVAL_USEC,
              const uint32_t timeout_sec = PRVC_TIMEOUT_SEC);
    virtual ~Encryptor(void) = default;

    void compute(const int64_t val);
    
private:
    static constexpr std::size_t  DefaultLogN   = 13;
    static constexpr std::size_t  DefaultNumBit = 30;
                 
private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace prvc_enc */

#endif /* PRVC_ENC_HPP */
