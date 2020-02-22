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

#ifndef PRVC_EVAL_ENC_COMPARATOR_HPP
#define PRVC_EVAL_ENC_COMPARATOR_HPP

#include <memory>
#include <cstdbool>
#include <prvc_share/prvc_enctype.hpp>

namespace prvc_share
{
class EncData;
}

namespace prvc_eval
{

/**
 * @brief This class is used to hold the encinput.
 */
class EncComparator
{
public:
    EncComparator(void);
    ~EncComparator(void) = default;

    void initialize(void);
    void push(const prvc_share::EncData& encdata);
    bool is_comparable(void) const;
    void compare(const prvc_share::FHEContext& context,
                 const size_t num_chunk,
                 std::vector<prvc_share::Ctxt>& v_c_cmp_res,
                 prvc_share::Ctxt& c_cmp_res) const;
    
private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace prvc_eval */

#endif /* PRVC_EVAL_ENC_COMPARATOR_HPP */
