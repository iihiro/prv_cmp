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
#include <unistd.h>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <prvc_share/prvc_securekey_filemanager.hpp>
#include <prvc_dec/prvc_dec_state_enc.hpp>
#include <prvc_dec/prvc_dec_state_eval.hpp>
#include <dec/server_enc.hpp>
#include <dec/server_eval.hpp>

static constexpr const char* CONTEXT_FILENAME = "context.txt";
static constexpr const char* PUBKEY_FILENAME  = "pubkey.txt";
static constexpr const char* SECKEY_FILENAME  = "seckey.txt";

struct Param
{
    std::string context_filename = CONTEXT_FILENAME;
    std::string pubkey_filename  = PUBKEY_FILENAME;
    std::string seckey_filename  = SECKEY_FILENAME;
    bool is_generate_securekey   = false;
};

void init(Param& param, int argc, char* argv[])
{
    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, "c:p:s:gh")) != -1)
    {
        switch (opt)
        {
            case 'c':
                param.context_filename = optarg;
                break;
            case 'p':
                param.pubkey_filename = optarg;
                break;
            case 's':
                param.seckey_filename = optarg;
                break;
            case 'g':
                param.is_generate_securekey = true;
                break;
            case 'h':
            default:
                printf(
                  "Usage: %s [-c context_filename] [-p pubkey_filename] [-s seckey_filename] [-g]\n",
                  argv[0]);
                exit(1);
        }
    }
}

void exec(const Param& param)
{
    STDSC_LOG_INFO("Launched Decryptor demo app");

    std::shared_ptr<prvc_share::SecureKeyFileManager> skm_ptr(
        new prvc_share::SecureKeyFileManager(param.context_filename,
                                             param.pubkey_filename,
                                             param.seckey_filename));
    
    prvc_dec::CallbackParam cb_param;
    cb_param.set_skm(skm_ptr);

    stdsc::StateContext state_enc(std::make_shared<prvc_dec::enc::StateReady>());
    prvc_demo::ServerEnc server_enc(cb_param, state_enc, DEC_PORT_FOR_ENC,
                                    param.is_generate_securekey);
    server_enc.start();

    stdsc::StateContext state_eval(std::make_shared<prvc_dec::eval::StateReady>());
    prvc_demo::ServerEval server_eval(cb_param, state_eval, DEC_PORT_FOR_EVAL,
                                      param.is_generate_securekey);
    server_eval.start();

    server_enc.join();
    server_eval.join();
}

int main(int argc, char* argv[])
{
    STDSC_INIT_LOG();
    try
    {
        Param param;
        init(param, argc, argv);
        exec(param);
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
