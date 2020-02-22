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

#ifndef PRVC_ENCPARAM_HPP
#define PRVC_ENCPARAM_HPP

#include <iostream>

namespace prvc_share
{

/**
 * @brief This clas is used to hold the parameters to compute on evaluator.
 */
struct EncParam
{
    size_t num_chunk;
};

std::ostream& operator<<(std::ostream& os, const EncParam& param);
std::istream& operator>>(std::istream& is, EncParam& param);

} /* namespace prvc_share */

#endif /* PRVC_ENCPARAM_HPP */
