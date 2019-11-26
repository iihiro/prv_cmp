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
#include <prvc_dec/prvc_dec_server.hpp>
//#include <prvc_dec/prvc_dec_state.hpp>

namespace prvc_dec
{

struct DecServer::Impl
{
    Impl(const char* port, stdsc::CallbackFunctionContainer& callback,
         stdsc::StateContext& state, prvc_share::SecureKeyFileManager& skm,
         bool is_generate_securekey = false)
        : server_(new stdsc::Server<>(port, state, callback)), state_(state), skm_(skm)
    {
        STDSC_LOG_INFO("Lanched Dec server (%s)", port);

        if (is_generate_securekey) {
            skm_.initialize();
        }
    }

    ~Impl(void) = default;

    void start(void)
    {
        if (!skm_.is_exist(SKM_DATAKIND_PUBKEY))
        {
            std::ostringstream oss;
            oss << "Err: public key file was not found. (" << skm_.filename(SKM_DATAKIND_PUBKEY)
                << ")" << std::endl;
            STDSC_THROW_FILE(oss.str());
        }
        if (!skm_.is_exist(SKM_DATAKIND_SECKEY))
        {
            std::ostringstream oss;
            oss << "Err: security key file was not found. (" << skm_.filename(SKM_DATAKIND_SECKEY)
                << ")" << std::endl;
            STDSC_THROW_FILE(oss.str());
        }

        bool async = true;
        server_->start(async);
    }

    void join(void)
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
                     prvc_share::SecureKeyFileManager& skm,
                     bool is_generate_securekey)
  : pimpl_(new Impl(port, callback, state, skm, is_generate_securekey))
{
}

void DecServer::start(void)
{
    pimpl_->start();
}

void DecServer::join(void)
{
    pimpl_->join();
}

} /* namespace prvc_dec */
