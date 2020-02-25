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

#include <fstream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_socket.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_log.hpp>
#include <prvc_share/prvc_packet.hpp>
#include <prvc_share/prvc_pubkey.hpp>
#include <prvc_share/prvc_context.hpp>
#include <prvc_share/prvc_encdata.hpp>
#include <prvc_share/prvc_encparam.hpp>
#include <prvc_share/prvc_plaindata.hpp>
#include <prvc_eval/prvc_eval_callback_param.hpp>
#include <prvc_eval/prvc_eval_client.hpp>
#include <prvc_eval/prvc_eval_state.hpp>
#include <prvc_eval/prvc_eval_callback_function.hpp>

namespace prvc_eval
{

// CallbackFunctionEncInput
DEFUN_DATA(CallbackFunctionEncInput)
{
    STDSC_LOG_INFO("Received enc input. (current state : %s)",
                   state.current_state_str().c_str());

    DEF_CDATA_ON_EACH(prvc_eval::CallbackParam);
    DEF_CDATA_ON_ALL(prvc_eval::CommonCallbackParam);
    
    stdsc::BufferStream buffstream(buffer);
    std::iostream stream(&buffstream);
    
    auto& client  = cdata_e->get_client();
    auto& compara = cdata_a->comparator_;
    
    prvc_share::EncData encdata(client.context());
    prvc_share::PlainData<prvc_share::EncParam> pladata;
    
    encdata.load_from_stream(stream);
    pladata.load_from_stream(stream);

    compara->push(encdata);

    if (compara->is_comparable()) {
        STDSC_LOG_INFO("Compare enc results.");
        
        const auto& param = pladata.data();
        std::vector<prvc_share::Ctxt> v_c_cmp_res(param.num_chunk);
        prvc_share::Ctxt c_cmp_res;

        compara->compare(client.context().get(), param.num_chunk,
                         v_c_cmp_res, c_cmp_res);
        compara->initialize();

        client.send_result(v_c_cmp_res, c_cmp_res);
    }
    
#if 0 //test
    encdata.save_to_file("encinput.txt");

    compara->push(encdata);
    printf("compara: is_comparable:%d\n", compara->is_comparable());
    compara->push(encdata);
    printf("compara: is_comparable:%d\n", compara->is_comparable());
    compara->compare();

    const auto& param = pladata.data();
    printf("num_chunk: %lu\n", param.num_chunk);
#endif
}

} /* namespace prvc_eval */
