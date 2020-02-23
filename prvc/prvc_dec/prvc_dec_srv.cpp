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

#include <sstream>
#include <stdsc/stdsc_server.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <prvc_share/prvc_utility.hpp>
#include <prvc_share/prvc_securekey_filemanager.hpp>
#include <prvc_dec/prvc_dec_srv.hpp>

namespace prvc_dec
{

struct DecServer::Impl
{
    Impl(const char* port, stdsc::CallbackFunctionContainer& callback,
         stdsc::StateContext& state,
         prvc_share::SecureKeyFileManager& skm)
        : server_(new stdsc::Server<>(port, state, callback)),
          state_(state),
          skm_(skm)
    {
        STDSC_LOG_INFO("Lanched Dec server (%s)", port);
    }

    ~Impl(void) = default;

    void start(void)
    {
        auto kind_pubkey = static_cast<int32_t>(prvc_share::SecureKeyFileManager::Kind_t::kKindPubKey);
        auto kind_seckey = static_cast<int32_t>(prvc_share::SecureKeyFileManager::Kind_t::kKindSecKey);

        for (int32_t k=kind_pubkey; k<=kind_seckey; ++k) {
            auto kind = static_cast<prvc_share::SecureKeyFileManager::Kind_t>(k);
            if (!skm_.is_exist(kind))
            {
                std::ostringstream oss;
                oss << "Err: key file not found. (" << skm_.filename(kind) << ")" << std::endl;
                STDSC_THROW_FILE(oss.str());
            }
        }

        bool enable_async_mode = true;
        server_->start(enable_async_mode);
    }

    void stop(void)
    {
        server_->stop();
    }

    void wait(void)
    {
        server_->wait();
    }

private:
    std::shared_ptr<stdsc::Server<>> server_;
    stdsc::StateContext& state_;
    prvc_share::SecureKeyFileManager& skm_;
};

DecServer::DecServer(const char* port,
                     stdsc::CallbackFunctionContainer& callback,
                     stdsc::StateContext& state,
                     prvc_share::SecureKeyFileManager& skm)
    : pimpl_(new Impl(port, callback, state, skm))
{
}

void DecServer::start(void)
{
    pimpl_->start();
}

void DecServer::stop(void)
{
    pimpl_->stop();
}

void DecServer::wait(void)
{
    pimpl_->wait();
}

} /* namespace prvc_dec */
