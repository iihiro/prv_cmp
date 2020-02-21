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

#ifndef PRVC_EVAL_CALLBACK_PARAM_HPP
#define PRVC_EVAL_CALLBACK_PARAM_HPP

#include <memory>
#include <vector>

namespace prvc_eval
{

class Client;
    
/**
 * @brief This class is used to hold the callback parameters for Server on Evaluator.
 */
struct CallbackParam
{
    CallbackParam(void);
    ~CallbackParam(void) = default;

    //std::string pubkey_filename;
    //std::string context_filename;
    //std::string encdata_filename;
    //std::string encmodel_filename;

    void set_client(std::shared_ptr<prvc_eval::Client>& client);
    prvc_eval::Client& get_client(void);
    
    //std::shared_ptr<prvc_share::EncData> encdata_ptr;
    //std::shared_ptr<prvc_share::PermVec> permvec_ptr;
    
    //std::vector<long> permvec;
private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

/**
 * @brief This class is used to hold the callback parameters for Server#1 on CS.
 * This parameter to shared on all connections.
 */
//struct CommonCallbackParam
//{
//    std::shared_ptr<prvc_share::EncData> encmodel_ptr;
//};

} /* namespace prvc_eval */

#endif /* PRVC_EVAL_CALLBACK_PARAM_HPP */
