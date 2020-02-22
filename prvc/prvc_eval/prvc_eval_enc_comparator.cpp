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

#include <stdsc/stdsc_exception.hpp>
#include <prvc_share/prvc_enctype.hpp>
#include <prvc_share/prvc_encdata.hpp>
#include <prvc_eval/prvc_eval_enc_comparator.hpp>

namespace prvc_eval
{

struct EncComparator::Impl
{
    Impl(void) = default;
    ~Impl(void) = default;

    void initialize(void)
    {
        vencdata_.clear();
    }
    
    void push(const prvc_share::EncData& encdata)
    {
        if (is_comparable()) {
            vencdata_.erase(vencdata_.begin());
        }
        vencdata_.push_back(encdata);
    }

    bool is_comparable(void) const
    {
        return vencdata_.size() >= 2;
    }

    void compare(void) const
    {
        const auto& c_x_chunks = vencdata_.at(0).data();
        const auto& c_y_chunks = vencdata_.at(1).data();

        
    }

private:
    std::vector<prvc_share::EncData> vencdata_;
};

EncComparator::EncComparator(void)
    : pimpl_(new Impl())
{}

void EncComparator::initialize(void)
{
    pimpl_->initialize();
}

void EncComparator::push(const prvc_share::EncData& encdata)
{
    pimpl_->push(encdata);
}

bool EncComparator::is_comparable(void) const
{
    return pimpl_->is_comparable();
}

void EncComparator::compare(void) const
{
    pimpl_->compare();
}

} /* namespace prvc_eval */
