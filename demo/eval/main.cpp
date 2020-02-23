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
#include <prvc_eval/prvc_eval_srv.hpp>
#include <prvc_eval/prvc_eval_state.hpp>
#include <prvc_eval/prvc_eval_callback_param.hpp>
#include <prvc_eval/prvc_eval_callback_function.hpp>
#include <prvc_eval/prvc_eval_client.hpp>
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
                printf("Usage: %s\n", argv[0]);
                exit(1);
        }
    }
}

void exec(const Option& option)
{
    const char* host   = "localhost";

    stdsc::StateContext state(std::make_shared<prvc_eval::StateReady>());

    stdsc::CallbackFunctionContainer callback;
    prvc_eval::CallbackParam param;
    prvc_eval::CommonCallbackParam cparam;
    
    std::shared_ptr<prvc_eval::Client> client(
        new prvc_eval::Client(host, PORT_DEC_SRV));
    param.set_client(client);

    std::shared_ptr<stdsc::CallbackFunction> cb_input(
        new prvc_eval::CallbackFunctionEncInput());
    callback.set(prvc_share::kControlCodeDataEncInput, cb_input);

    callback.set_commondata(static_cast<void*>(&param), sizeof(param),
                            stdsc::kCommonDataOnEachConnection);
    callback.set_commondata(static_cast<void*>(&cparam), sizeof(cparam),
                            stdsc::kCommonDataOnAllConnection);

    std::shared_ptr<prvc_eval::EvalServer> server
        = std::make_shared<prvc_eval::EvalServer>(PORT_EVAL_SRV, callback, state);
    server->start();
    server->wait();
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
