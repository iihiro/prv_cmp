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

#include <sstream>
#include <stdsc/stdsc_server.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <prvc_eval/prvc_eval_state.hpp>
#include <prvc_eval/prvc_eval_thread.hpp>

namespace prvc_eval
{

struct EvalThread::Impl
{
    Impl(const char* port, stdsc::CallbackFunctionContainer& callback,
         stdsc::StateContext& state)
        : server_(new stdsc::Server<>(port, state, callback)), state_(state)
    {
        STDSC_LOG_INFO("Lanched Evaluator server (%s)", port);
    }

    ~Impl(void) = default;

    void start(void)
    {
        server_->start();
    }

    void join(void)
    {
        server_->wait();
    }

private:
    std::shared_ptr<stdsc::Server<>> server_;
    stdsc::StateContext& state_;
};

EvalThread::EvalThread(const char* port,
                       stdsc::CallbackFunctionContainer& callback,
                       stdsc::StateContext& state)
  : pimpl_(new Impl(port, callback, state))
{
}

void EvalThread::start(void)
{
    pimpl_->start();
}

void EvalThread::join(void)
{
    pimpl_->join();
}

} /* namespace prvc_eval */
