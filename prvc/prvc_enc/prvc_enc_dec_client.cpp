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

#include <memory>
#include <fstream>
#include <vector>
#include <cstring>
#include <stdsc/stdsc_client.hpp>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <prvc_share/prvc_packet.hpp>
#include <prvc_share/prvc_define.hpp>
#include <prvc_share/prvc_pubkey.hpp>
#include <prvc_share/prvc_context.hpp>
#include <prvc_enc/prvc_enc_dec_client.hpp>

namespace prvc_enc
{
    
struct DecClient::Impl
{
    Impl(stdsc::Client& client)
        : client_(client)
    {}

    ~Impl(void)
    {}

    void get_results(int64_t& result_overall,
                     std::vector<int64_t>& result_chunks) const
    {
    }

private:
    stdsc::Client& client_;
};

DecClient::DecClient(const char* host, const char* port)
    : super(host, port)
{
    auto& client = super::client();
    pimpl_ = std::make_shared<Impl>(client);
}

void DecClient::get_results(int64_t& result_overall,
                            std::vector<int64_t>& result_chunks) const
{
    pimpl_->get_results(result_overall, result_chunks);
}

} /* namespace prvc_enc */
