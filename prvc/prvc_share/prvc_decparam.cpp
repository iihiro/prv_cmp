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

#include <prvc_share/prvc_decparam.hpp>

namespace prvc_share
{

std::ostream& operator<<(std::ostream& os, const DecParam& param)
{
    os << param.mul_depth  << std::endl;
    os << param.logN       << std::endl;
    os << param.bit_len    << std::endl;
    os << param.dcrt_bits  << std::endl;
    os << param.rel_window << std::endl;
    os << param.sigma      << std::endl;
    return os;
}

std::istream& operator>>(std::istream& is, DecParam& param)
{
    is >> param.mul_depth;
    is >> param.logN;
    is >> param.bit_len;
    is >> param.dcrt_bits;
    is >> param.rel_window;
    is >> param.sigma;
    return is;
}
    
} /* namespace prvc_share */
