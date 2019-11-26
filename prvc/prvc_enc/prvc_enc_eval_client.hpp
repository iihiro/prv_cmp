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

#ifndef PRVC_ENC_EVAL_CLIENT_HPP
#define PRVC_ENC_EVAL_CLIENT_HPP

#include <memory>
#include <vector>
#include <string>
#include <stdsc/stdsc_thread.hpp>

namespace prvc_enc
{

class EvalParam;

/**
 * @brief Enumeration for kind of Encryptor.
 */
enum EncryptorKind : uint32_t
{
    kEncryptorKindNil = 0,
    kEncryptorKindX,
    kEncryptorKindY,
    kNumOfEncryptorKind,
};

    
/**
 * @brief Provides client for Evaluator.
 */
template <class T = EvalParam>
class EvalClient : public stdsc::Thread<T>
{
    using super = stdsc::Thread<T>;

public:
    EvalClient(const char* host, const char* port,
               const EncryptorKind kind);
    virtual ~EvalClient(void);

    void start(T& param);
    void wait_for_finish(void);

private:
    virtual void exec(T& args,
                      std::shared_ptr<stdsc::ThreadException> te) const;

    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

struct EvalParam
{
    uint64_t input_value;
    size_t numbit;
    size_t logN;
    bool is_neg;
    std::string context_filename = "context.txt";
    std::string pubkey_filename  = "pukey.txt";
};

} /* namespace prvc_enc */

#endif /* PRVC_ENC_EVAL_CLIENT_HPP */
