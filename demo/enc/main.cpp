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

#include <unistd.h>
#include <string>
#include <iostream>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <prvc_enc/prvc_enc_dec_client.hpp>
#include <prvc_enc/prvc_enc_eval_client.hpp>
#include <share/define.hpp>

static constexpr const char* CONTEXT_FILENAME = "context.txt";
static constexpr const char* PUBKEY_FILENAME  = "pubkey.txt";

struct Option
{
    prvc_enc::EncryptorKind kind;
};

void init(Option& option, int argc, char* argv[])
{
    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, "k:h")) != -1)
    {
        switch (opt)
        {
            case 'k':
                option.kind = static_cast<prvc_enc::EncryptorKind>(std::stol(optarg));
                break;
            case 'h':
            default:
                printf("Usage: %s [-k kind]\n",
                       argv[0]);
                exit(1);
        }
    }
}

void exec(Option& option)
{
    const char* host = "localhost";

    prvc_enc::DecParam dec_param;
    dec_param.context_filename = CONTEXT_FILENAME; 
    dec_param.pubkey_filename  = PUBKEY_FILENAME;
    prvc_enc::DecClient<> dec_client(host, DEC_PORT_FOR_ENC);
    dec_client.start(dec_param);
    dec_client.wait_for_finish();

    prvc_enc::EvalParam eval_param;
    eval_param.input_value      = (option.kind == prvc_enc::kEncryptorKindX) ? 10 : 6;
    eval_param.numbit           = 30;
    eval_param.logN             = 13;
    eval_param.is_neg           = (option.kind == prvc_enc::kEncryptorKindX) ? false : true;
    eval_param.context_filename = CONTEXT_FILENAME; 
    eval_param.pubkey_filename  = PUBKEY_FILENAME;
    prvc_enc::EvalClient<> eval_client(host, EVAL_PORT_FOR_ENC, option.kind);
    eval_client.start(eval_param);

    eval_client.wait_for_finish();
}

int main(int argc, char* argv[])
{
    STDSC_INIT_LOG();
    try
    {
        Option option;
        init(option, argc, argv);
        STDSC_LOG_INFO("Launched Encryptor demo app");
        exec(option);
    }
    catch (stdsc::AbstractException& e)
    {
        STDSC_LOG_ERR("Err: %s", e.what());
    }
    catch (...)
    {
        STDSC_LOG_ERR("Catch unknown exception");
    }

    return 0;
}
