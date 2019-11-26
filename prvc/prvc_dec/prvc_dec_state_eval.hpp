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

#ifndef PRVC_DEC_STATE_EVAL_HPP
#define PRVC_DEC_STATE_EVAL_HPP

#include <cstdbool>

#include <memory>
#include <stdsc/stdsc_state.hpp>

namespace prvc_dec
{
namespace eval
{

/**
 * @brief Enumeration for state of Decryptor.
 */
enum StateId_t : uint64_t
{
    kStateNil = 0,
    kStateReady,
    kStateExit,
};

/**
 * @brief Enumeration for events of Decryptor.
 */
enum Event_t : uint64_t
{
    kEventConnectSocket = 0,
    kEventDisconnectSocket,
    kEventReceivedEVKReq,
    kEventReceivedEncResult,
};

/**
 * @brief Provides 'Ready' state.
 */
struct StateReady : public stdsc::State
{
    static std::shared_ptr<State> create();
    StateReady();
    virtual void set(stdsc::StateContext &sc, uint64_t event) override;
    virtual uint64_t id(void) const override
    {
        return kStateReady;
    }

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* eval */
} /* prvc_dec */

#endif /* PRVC_DEC_STATE_EVAL_HPP */
