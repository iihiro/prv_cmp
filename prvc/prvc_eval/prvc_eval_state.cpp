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
#include <prvc_eval/prvc_eval_state.hpp>

namespace prvc_eval
{

struct StateInit::Impl
{
    Impl()
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        STDSC_LOG_TRACE("StateInit: event#%lu", event);
        switch (static_cast<Event_t>(event))
        {
            case kEventSendEVKRequest:
                sc.next_state(StateReady::create());
                break;
            default:
                break;
        }
    }

private:
    std::mutex mutex_;
};
    
struct StateReady::Impl
{
    Impl(bool is_received_enc_x, bool is_received_enc_y)
        : is_received_enc_x_(is_received_enc_x),
          is_received_enc_y_(is_received_enc_y)
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        STDSC_LOG_TRACE("StateReady: event#%lu", event);
        switch (static_cast<Event_t>(event))
        {
            case kEventReceivedEncInputX:
                is_received_enc_x_ = true;
                break;
            case kEventReceivedEncInputY:
                is_received_enc_y_ = true;
                break;
            default:
                break;
        }

        if (is_received_enc_x_ && is_received_enc_y_) {
            sc.next_state(StateComputed::create());
        }
    }

private:
    bool is_received_enc_x_;
    bool is_received_enc_y_;
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
            case kEventSendEVKRequest:
            case kEventReceivedEncInputX:
            case kEventReceivedEncInputY:
                sc.next_state(StateReady::create(true, true));
                break;
            default:
                break;
        }
    }

private:
    std::mutex mutex_;
};    

// Init
std::shared_ptr<stdsc::State> StateInit::create()
{
    auto s = std::shared_ptr<stdsc::State>(new StateInit());
    return s;
}

StateInit::StateInit()
    : pimpl_(new Impl())
{
}

void StateInit::set(stdsc::StateContext& sc, uint64_t event)
{
    pimpl_->set(sc, event);
}
    
// Ready
std::shared_ptr<stdsc::State> StateReady::create(
    bool is_received_enc_x, bool is_received_enc_y)
{
    auto s = std::shared_ptr<stdsc::State>(
        new StateReady(is_received_enc_x, is_received_enc_y));
    return s;
}

StateReady::StateReady(bool is_received_enc_x, bool is_received_enc_y)
  : pimpl_(new Impl(is_received_enc_x, is_received_enc_y))
{
}

void StateReady::set(stdsc::StateContext& sc, uint64_t event)
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
    
} /* prvc_eval */
