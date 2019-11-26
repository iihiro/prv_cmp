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
#include <cmath>
#include <stdsc/stdsc_client.hpp>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <prvc_share/prvc_packet.hpp>
#include <prvc_share/prvc_define.hpp>
#include <prvc_share/prvc_enctype.hpp>
#include <prvc_share/prvc_encdata.hpp>
#include <prvc_share/prvc_encutility.hpp>
#include <prvc_enc/prvc_enc_eval_client.hpp>

namespace prvc_enc
{
    
template <class T>
struct EvalClient<T>::Impl
{
    std::shared_ptr<stdsc::ThreadException> te_;

    Impl(const char* host, const char* port, const EncryptorKind kind)
        : host_(host), port_(port), kind_(kind)
    {
        te_ = stdsc::ThreadException::create();
    }

    void exec(T& args, std::shared_ptr<stdsc::ThreadException> te)
    {
        try
        {
            constexpr uint32_t retry_interval_usec = PRVC_RETRY_INTERVAL_USEC;
            constexpr uint32_t timeout_sec = PRVC_TIMEOUT_SEC;

            STDSC_LOG_INFO("Connecting to Evaluator.");
            client_.connect(host_, port_, retry_interval_usec, timeout_sec);
            STDSC_LOG_INFO("Connected to Evaluator.");

            FHEContext context;
            {
                lbcrypto::Serialized ser_context;
                STDSC_THROW_FILE_IF_CHECK(
                    prvc_share::encutility::read_from_file(args.context_filename, ser_context),
                    "failed to read context file");
                context = lbcrypto::CryptoContextFactory<PolyType>::DeserializeAndCreateContext(ser_context);
            }

            lbcrypto::LPPublicKey<PolyType> pubkey;
            {
                lbcrypto::Serialized ser_pubkey;
                STDSC_THROW_FILE_IF_CHECK(
                    prvc_share::encutility::read_from_file(args.pubkey_filename, ser_pubkey),
                    "failed to read pubkey file");
                pubkey = context->deserializePublicKey(ser_pubkey);
            }

            size_t nsplit = 0;
            std::vector<uint64_t> s_values;
            prvc_share::encutility::split_values(args.numbit, args.logN, args.input_value, nsplit, s_values);
            STDSC_LOG_DEBUG("split_values: numbit: %u, nsplit: %u\n", args.numbit, nsplit);

            prvc_share::EncData encdata;
            prvc_share::encutility::encode_and_encrypt_splitted_values(
                context, pubkey, nsplit, args.is_neg, s_values, encdata);
            for(size_t i=0; i<nsplit; ++i) {
                STDSC_LOG_DEBUG("encrypt[%u]: %lu\n", i, s_values[i]);
            }

            stdsc::BufferStream buffstream(encdata.stream_size());
            std::iostream stream(&buffstream);
            encdata.save_to_stream(stream);

            STDSC_LOG_INFO("Sending encrypted input data.");
            stdsc::Buffer* buffer = &buffstream;
            prvc_share::ControlCode_t control_code
                = (kind_ == kEncryptorKindX) ?
                prvc_share::kControlCodeDataEncInputX :
                prvc_share::kControlCodeDataEncInputY;
            client_.send_data_blocking(control_code, *buffer);

            client_.close();
        }
        catch (const stdsc::AbstractException& e)
        {
            STDSC_LOG_TRACE("Failed to client process (%s)", e.what());
            te->set_current_exception();
        }
    }

private:
    const char* host_;
    const char* port_;
    const EncryptorKind kind_;
    stdsc::Client client_;
};

template <class T>
EvalClient<T>::EvalClient(const char* host, const char* port,
                          const EncryptorKind kind)
    : pimpl_(new Impl(host, port, kind))
{
}

template <class T>
EvalClient<T>::~EvalClient(void)
{
    super::join();
}

template <class T>
void EvalClient<T>::start(T& param)
{
    super::start(param, pimpl_->te_);
}

template <class T>
void EvalClient<T>::wait_for_finish(void)
{
    super::join();
    pimpl_->te_->rethrow_if_has_exception();
}

template <class T>
void EvalClient<T>::exec(T& args,
                        std::shared_ptr<stdsc::ThreadException> te) const
{
    try
    {
        pimpl_->exec(args, te);
    }
    catch (...)
    {
        te->set_current_exception();
    }
}

template class EvalClient<EvalParam>;

} /* namespace prvc_enc */
