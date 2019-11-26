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

#ifndef PRVC_EVAL_STATE_HPP
#define PRVC_EVAL_STATE_HPP

#include <cstdbool>

#include <memory>
#include <stdsc/stdsc_state.hpp>

namespace prvc_eval
{

/**
 * @brief Enumeration for state of Evaluator.
 */
enum StateId_t : uint64_t
{
    kStateNil = 0,
    kStateInit,
    kStateReady,
    kStateComputed,
    kStateExit,
};

/**
 * @brief Enumeration for events of Evaluator.
 */
enum Event_t : uint64_t
{
    kEventSendEVKRequest = 0,
    kEventReceivedEncInputX,
    kEventReceivedEncInputY,
};

/**
 * @brief Provides 'Init' state.
 */
struct StateInit : public stdsc::State
{
    static std::shared_ptr<State> create();
    StateInit();
    virtual void set(stdsc::StateContext &sc, uint64_t event) override;
    virtual uint64_t id(void) const override
    {
        return kStateInit;
    }

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};
    
/**
 * @brief Provides 'Ready' state.
 */
struct StateReady : public stdsc::State
{
    static std::shared_ptr<State> create(bool is_received_enc_x=false,
                                         bool is_received_enc_y=false);
    StateReady(bool is_received_enc_x=false,
               bool is_received_enc_y=false);
    virtual void set(stdsc::StateContext &sc, uint64_t event) override;
    virtual uint64_t id(void) const override
    {
        return kStateReady;
    }

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

/**
 * @brief Provides 'Computed' state.
 */
struct StateComputed : public stdsc::State
{
    static std::shared_ptr<State> create();
    StateComputed();
    virtual void set(stdsc::StateContext &sc, uint64_t event) override;
    virtual uint64_t id(void) const override
    {
        return kStateComputed;
    }

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* prvc_eval */

#endif /* PRVC_EVAL_STATE_HPP */
