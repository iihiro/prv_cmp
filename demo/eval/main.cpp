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
#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_callback_function.hpp>
#include <stdsc/stdsc_callback_function_container.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <prvc_share/prvc_packet.hpp>
#include <prvc_eval/prvc_eval_state.hpp>
#include <prvc_eval/prvc_eval_dec_client.hpp>
#include <prvc_eval/prvc_eval_thread.hpp>
#include <prvc_eval/prvc_eval_callback_param.hpp>
#include <prvc_eval/prvc_eval_callback_function.hpp>
#include <share/define.hpp>

static constexpr char* CONTEXT_FILENAME = (char*)"context.txt";

struct Option
{
    uint32_t dummy = 0;
};

void init(Option& option, int argc, char* argv[])
{
    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, "h")) != -1)
    {
        switch (opt)
        {
            case 'h':
            default:
                printf("Usage: %s [-i input_filename]\n", argv[0]);
                exit(1);
        }
    }
}

void create_enc_server(prvc_eval::CallbackParam& param,
                       stdsc::StateContext& state,
                       const char* port = EVAL_PORT_FOR_ENC)
{
    stdsc::CallbackFunctionContainer callback;

    std::shared_ptr<stdsc::CallbackFunction> cb_enc_x(
      new prvc_eval::CallbackFunctionEncInputX(param));
    callback.set(prvc_share::kControlCodeDataEncInputX, cb_enc_x);
    
    std::shared_ptr<stdsc::CallbackFunction> cb_enc_y(
      new prvc_eval::CallbackFunctionEncInputY(param));
    callback.set(prvc_share::kControlCodeDataEncInputY, cb_enc_y);

    prvc_eval::EvalThread server(port, callback, state);
    server.start();
    server.join();
}

void exec(const Option& option)
{
    std::string context_filepath = CONTEXT_FILENAME;
    
    STDSC_LOG_INFO("Launched Evaluator demo app");
    stdsc::StateContext state(std::make_shared<prvc_eval::StateInit>());

    prvc_eval::DecClient client("localhost", DEC_PORT_FOR_EVAL, state);
    client.download_evk(context_filepath.c_str());

    prvc_eval::CallbackParam param(context_filepath, client);
    state.set(prvc_eval::kEventSendEVKRequest);
    create_enc_server(param, state);
}

int main(int argc, char* argv[])
{
    STDSC_INIT_LOG();
    try
    {
        Option option;
        init(option, argc, argv);
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
