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
#include <stdsc/stdsc_callback_function_container.hpp>
#include <prvc_share/prvc_securekey_filemanager.hpp>
#include <prvc_share/prvc_packet.hpp>
#include <prvc_dec/prvc_dec_state.hpp>
#include <prvc_dec/prvc_dec_callback_param.hpp>
#include <prvc_dec/prvc_dec_callback_function.hpp>
#include <prvc_dec/prvc_dec_srv.hpp>
#include <prvc_share/prvc_context.hpp>
#include <prvc_share/prvc_pubkey.hpp>
#include <share/define.hpp>

static constexpr const char* CONTEXT_FILENAME = "context.txt";
static constexpr const char* PUBKEY_FILENAME  = "pubkey.txt";
static constexpr const char* SECKEY_FILENAME  = "seckey.txt";

struct Param
{
    std::string pubkey_filename  = PUBKEY_FILENAME;
    std::string seckey_filename  = SECKEY_FILENAME;
    std::string context_filename = CONTEXT_FILENAME;
    std::string config_filename; // set empty if file is specified
    bool is_generate_securekey   = false;
};

void init(Param& param, int argc, char* argv[])
{
    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, "p:s:c:t:gh")) != -1)
    {
        switch (opt)
        {
            case 'p':
                param.pubkey_filename = optarg;
                break;
            case 's':
                param.seckey_filename = optarg;
                break;
            case 'c':
                param.context_filename = optarg;
                break;
            case 't':
                param.config_filename = optarg;
                break;
            case 'g':
                param.is_generate_securekey = true;
                break;
            case 'h':
            default:
                printf(
                  "Usage: %s [-p pubkey_filename] [-s seckey_filename] [-c context_filename] [-t conifg_filename] [-g]\n",
                  argv[0]);
                exit(1);
        }
    }
}

static std::shared_ptr<prvc_dec::DecServer>
start_srv_async(prvc_dec::CallbackParam& cb_param)
{
    stdsc::StateContext state(std::make_shared<prvc_dec::StateReady>());

    stdsc::CallbackFunctionContainer callback;
    {
        std::shared_ptr<stdsc::CallbackFunction> cb_pubkey(
            new prvc_dec::CallbackFunctionPubkeyRequest());
        callback.set(prvc_share::kControlCodeDownloadPubkey, cb_pubkey);
        
        std::shared_ptr<stdsc::CallbackFunction> cb_context(
            new prvc_dec::CallbackFunctionContextRequest());
        callback.set(prvc_share::kControlCodeDownloadContext, cb_context);
        
        std::shared_ptr<stdsc::CallbackFunction> cb_end(
            new prvc_dec::CallbackFunctionDecryptRequest());
        callback.set(prvc_share::kControlCodeDataDecryptRequest, cb_end);

        callback.set_commondata(static_cast<void*>(&cb_param), sizeof(cb_param));
    }

    std::shared_ptr<prvc_dec::DecServer> server = std::make_shared<prvc_dec::DecServer>(
        PORT_DEC_SRV, callback, state, *cb_param.skm_ptr);
    server->start();

    return server;
}    

void exec(const Param& param)
{
    STDSC_LOG_INFO("Launched Decryptor demo app");

    std::shared_ptr<prvc_share::SecureKeyFileManager> skm_ptr;
    if (param.config_filename.empty()) {
        skm_ptr = std::make_shared<prvc_share::SecureKeyFileManager>(
            param.pubkey_filename,
            param.seckey_filename,
            param.context_filename);
    } else {
        skm_ptr = std::make_shared<prvc_share::SecureKeyFileManager>(
            param.pubkey_filename,
            param.seckey_filename,
            param.context_filename,
            param.config_filename);
    }

    if (param.is_generate_securekey) {
        skm_ptr->initialize();
    }

#if 0 // test
    prvc_share::Context ck;
    ck.load_from_file("context.txt");
    ck.save_to_file("context2.txt");

    prvc_share::PubKey pk(ck.get());
    pk.load_from_file("pubkey.txt");
    pk.save_to_file("pubkey2.txt");
#endif

    prvc_dec::CallbackParam cb_param;
    cb_param.skm_ptr = skm_ptr;
    cb_param.context_ptr = std::make_shared<prvc_share::Context>();
    cb_param.context_ptr->load_from_file(param.pubkey_filename);
    cb_param.pubkey_ptr = std::make_shared<prvc_share::PubKey>(cb_param.context_ptr->get());
    cb_param.pubkey_ptr->load_from_file(param.pubkey_filename);

    auto server = start_srv_async(cb_param);
    server->wait();
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
