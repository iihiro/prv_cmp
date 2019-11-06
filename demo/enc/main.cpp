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
#include <share/define.hpp>

static constexpr const char* PUBKEY_FILENAME = "pubkey.txt";

struct Option
{
    std::string input_filename = "in.txt";
};

void init(Option& option, int argc, char* argv[])
{
    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, "i:h")) != -1)
    {
        switch (opt)
        {
            case 'i':
                option.input_filename = optarg;
                break;
            case 'h':
            default:
                printf("Usage: %s [-i input_filename]\n",
                       argv[0]);
                exit(1);
        }
    }
}

void exec(Option& option)
{
    const char* host = "localhost";

    prvc_enc::DecParam dec_param;
    dec_param.pubkey_filename = PUBKEY_FILENAME;
    prvc_enc::DecClient<> dec_client(host, DEC_PORT_FOR_ENC);
    dec_client.start(dec_param);

    //prvc_enc::PPRSParam pprs_param;
    //pprs_param.input_filename = option.input_filename;
    //pprs_param.pubkey_filename = PUBKEY_FILENAME;
    //prvc_enc::PPRSClient<> pprs_client(host, PPRS_PORT_FOR_USER);
    //pprs_client.start(pprs_param);

    dec_client.wait_for_finish();
    //pprs_client.wait_for_finish();

    //std::vector<long> results;
    //dec_client.get_result(results);
    //
    //std::cout << "The std::vector of recommendation: ";
    //for (auto& v : results)
    //{
    //    std::cout << v << " ";
    //}
    //std::cout << std::endl;
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
