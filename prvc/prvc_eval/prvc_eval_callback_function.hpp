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

#ifndef PRVC_EVAL_CALLBACK_FUNCTION_HPP
#define PRVC_EVAL_CALLBACK_FUNCTION_HPP

#include <stdsc/stdsc_callback_function.hpp>

namespace prvc_eval
{
struct CallbackParam;

/**
 * @brief Provides callback function in receiving encrypted input X
 */
class CallbackFunctionEncInputX : public stdsc::CallbackFunction
{
public:
    CallbackFunctionEncInputX(CallbackParam& param) : param_(param)
    {
    }

protected:
    virtual void request_function(uint64_t code,
                                  stdsc::StateContext& state) override;
    virtual void data_function(uint64_t code, const stdsc::Buffer& buffer,
                               stdsc::StateContext& state) override;
    virtual void download_function(uint64_t code, const stdsc::Socket& sock,
                                   stdsc::StateContext& state) override;

private:
    prvc_eval::CallbackParam& param_;
};

/**
 * @brief Provides callback function in receiving encrypted input Y
 */
class CallbackFunctionEncInputY : public stdsc::CallbackFunction
{
public:
    CallbackFunctionEncInputY(CallbackParam& param) : param_(param)
    {
    }

protected:
    virtual void request_function(uint64_t code,
                                  stdsc::StateContext& state) override;
    virtual void data_function(uint64_t code, const stdsc::Buffer& buffer,
                               stdsc::StateContext& state) override;
    virtual void download_function(uint64_t code, const stdsc::Socket& sock,
                                   stdsc::StateContext& state) override;

private:
    prvc_eval::CallbackParam& param_;
};

} /* namespace prvc_eval */

#endif /* PRVC_EVAL_CALLBACK_FUNCTION_HPP */
