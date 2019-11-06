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

#include <mutex>
#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_log.hpp>
#include <prvc_dec/prvc_dec_state.hpp>

namespace prvc_dec
{

/* Encryptor x2, Evaluator */
static constexpr std::size_t MAX_CLIENT_NUM = 3;

struct StateInit::Impl
{
    Impl(size_t initial_connection_count) : count_(initial_connection_count)
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        STDSC_LOG_TRACE("StateInit: event#%lu, conn_count:%ld", event, count_);
        switch (static_cast<Event_t>(event))
        {
            case kEventConnectSocket:
                //if (++count_ >= MAX_CLIENT_NUM)
                if (++count_ >= 1) // for test
                {
                    sc.next_state(StateConnected::create());
                }
                break;
            case kEventDisconnectSocket:
                --count_;
                break;
            default:
                break;
        }
    }

private:
    std::size_t count_;
    std::mutex mutex_;
};

struct StateConnected::Impl
{
    Impl(void)
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        STDSC_LOG_TRACE("StateConnected: event#%lu", event);
        switch (static_cast<Event_t>(event))
        {
            case kEventReceivedEncResult:
                sc.next_state(StateComputed::create());
                break;
            case kEventDisconnectSocket:
                sc.next_state(StateInit::create(MAX_CLIENT_NUM - 1));
                break;
            default:
                break;
        }
    }

private:
    std::mutex mutex_;
};

struct StateComputed::Impl
{
    Impl(void)
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        STDSC_LOG_TRACE("StateComputed(%lu): event#%lu", event);
        switch (static_cast<Event_t>(event))
        {
            case kEventDisconnectSocket:
                sc.next_state(StateInit::create(MAX_CLIENT_NUM - 1));
                break;
            default:
                break;
        }
    }

private:
    std::mutex mutex_;
};

struct StateExit::Impl
{
    Impl(void)
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
    }

private:
    std::mutex mutex_;
};

// Init

std::shared_ptr<stdsc::State> StateInit::create(size_t initial_connection_count)
{
    auto s = 
      std::shared_ptr<stdsc::State>(new StateInit(initial_connection_count));
    return s;
}

StateInit::StateInit(size_t initial_connection_count)
  : pimpl_(new Impl(initial_connection_count))
{
}

void StateInit::set(stdsc::StateContext& sc, uint64_t event)
{
    pimpl_->set(sc, event);
}

// Connected

    std::shared_ptr<stdsc::State> StateConnected::create(void)
{
    auto s = std::shared_ptr<stdsc::State>(new StateConnected());
    return s;
}

StateConnected::StateConnected(void) : pimpl_(new Impl())
{
}

void StateConnected::set(stdsc::StateContext& sc, uint64_t event)
{
    pimpl_->set(sc, event);
}

// Computed

std::shared_ptr<stdsc::State> StateComputed::create()
{
    auto s = std::shared_ptr<stdsc::State>(new StateComputed());
    return s;
}

StateComputed::StateComputed(void) : pimpl_(new Impl())
{
}

void StateComputed::set(stdsc::StateContext& sc, uint64_t event)
{
    pimpl_->set(sc, event);
}

// Exit

std::shared_ptr<stdsc::State> StateExit::create()
{
    auto s = std::shared_ptr<stdsc::State>(new StateExit());
    return s;
}

StateExit::StateExit(void) : pimpl_(new Impl())
{
}

void StateExit::set(stdsc::StateContext& sc, uint64_t event)
{
    pimpl_->set(sc, event);
}

} /* prvc_dec */
