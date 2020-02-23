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
#include <prvc_enc/prvc_enc.hpp>
#include <prvc_enc/prvc_enc_dec_client.hpp>
#include <share/define.hpp>

#define PRINT_USAGE() printf("Usage: %s <value>\n", argv[0])

static constexpr const char* CONTEXT_FILENAME = "context.txt";
static constexpr const char* PUBKEY_FILENAME  = "pubkey.txt";

struct Option
{
    uint64_t input_value;
    bool is_neg_mononical_coef = false;
};

void init(Option& option, int argc, char* argv[])
{
    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, "t:h")) != -1)
    {
        switch (opt)
        {
            case 't':
                option.is_neg_mononical_coef = std::stol(optarg) == 0 ? false : true;
                break;
            case 'h':
            default:
                PRINT_USAGE();
                exit(1);
        }
    }

    if (optind < argc) {
        option.input_value = std::stol(argv[optind]);
    } else {
        PRINT_USAGE();
        exit(1);
    }
}

void exec(Option& option)
{
    const char* host = "localhost";

    prvc_enc::Encryptor enc(host, PORT_DEC_SRV,
                            host, PORT_EVAL_SRV,
                            option.is_neg_mononical_coef);
    enc.compute(option.input_value);
}

int main(int argc, char* argv[])
{
    STDSC_INIT_LOG();
    try
    {
        Option option;
        init(option, argc, argv);
        STDSC_LOG_INFO("Launched Encryptor demo app. (input value: %lu)",
                       option.input_value);
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
