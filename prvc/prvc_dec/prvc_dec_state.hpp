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

#ifndef PRVC_DEC_STATE_HPP
#define PRVC_DEC_STATE_HPP

#include <cstdbool>

#include <memory>
#include <stdsc/stdsc_state.hpp>

namespace prvc_dec
{

/**
 * @brief Enumeration for state of Decryptor.
 */
enum StateId_t : uint64_t
{
    kStateNil = 0,
    kStateInit,
    kStateConnected,
    kStateComputed,
    kStateExit,
};

/**
 * @brief Enumeration for events of Decryptor.
 */
enum Event_t : uint64_t
{
    kEventConnectSocket = 0,
    kEventDisconnectSocket,
    kEventReceivedPubkeyReq,
    kEventReceivedEncResult,
};

/**
 * @brief Provides 'Init' state.
 */
struct StateInit : public stdsc::State
{
    static std::shared_ptr<State> create(size_t initial_connection_count = 0);
    StateInit(size_t initial_connection_count = 0);
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
 * @brief Provides 'Connected' state.
 */
struct StateConnected : public stdsc::State
{
    static std::shared_ptr<stdsc::State> create(void);

    StateConnected(void);
    virtual void set(stdsc::StateContext &sc, uint64_t event) override;
    virtual uint64_t id(void) const override
    {
        return kStateConnected;
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
    static std::shared_ptr<State> create(void);
    StateComputed(void);
    virtual void set(stdsc::StateContext &sc, uint64_t event) override;
    virtual uint64_t id(void) const override
    {
        return kStateComputed;
    }

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

/**
 * @brief Provides 'Exit' state.
 */
struct StateExit : public stdsc::State
{
    static std::shared_ptr<State> create();
    StateExit(void);
    virtual void set(stdsc::StateContext &sc, uint64_t event) override;
    virtual uint64_t id(void) const override
    {
        return kStateExit;
    }

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* prvc_dec */

#endif /* PRVC_DEC_STATE_HPP */
