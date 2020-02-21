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
#include <prvc_eval/prvc_eval_callback_param.hpp>
#include <prvc_eval/prvc_eval_client.hpp>
#include <prvc_eval/prvc_eval_state.hpp>
#include <prvc_eval/prvc_eval_callback_function.hpp>

namespace prvc_eval
{

// CallbackFunctionEncInput
DEFUN_DATA(CallbackFunctionEncInput)
{
    STDSC_LOG_INFO("Received input data. (current state : %s)",
                   state.current_state_str().c_str());

    DEF_CDATA_ON_EACH(prvc_eval::CallbackParam);

    //stdsc::BufferStream buffstream(buffer);
    //std::iostream stream(&buffstream);
    //
    //auto& client = cdata_e->get_client();
    //
    //std::shared_ptr<prvc_share::EncData> encdata_ptr(new prvc_share::EncData(client.pubkey()));
    //encdata_ptr->load_from_stream(stream);
    //cdata_e->encdata_ptr = encdata_ptr;
    //
    //std::shared_ptr<prvc_share::PermVec> permvec_ptr(new prvc_share::PermVec());
    //permvec_ptr->load_from_stream(stream);
    //cdata_e->permvec_ptr = permvec_ptr;
    //
    //encdata_ptr->save_to_file(cdata_e->encdata_filename);
    //
    //{
    //    const auto& vec = permvec_ptr->vdata();
    //    cdata_e->permvec.resize(vec.size(), -1);
    //    std::memcpy(cdata_e->permvec.data(), vec.data(), sizeof(long) * vec.size());
    //}
    //
    //{        
    //    std::ostringstream oss;
    //    oss << "permvec: sz=" << cdata_e->permvec.size();
    //    oss << ", data=";
    //    for (auto& v : cdata_e->permvec) oss << " " << v;
    //    STDSC_LOG_DEBUG(oss.str().c_str());
    //}
    //
    //state.set(kEventEncInput);
    //state.set(kEventPermVec);
}

} /* namespace prvc_eval */
