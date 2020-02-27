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

#ifndef PRVC_DECPARAM_HPP
#define PRVC_DECPARAM_HPP

#include <iostream>

namespace prvc_share
{

/**
 * @brief This clas is used to hold the parameters to compute on encryptor.
 */
struct DecParam
{
    std::size_t mul_depth  = DefaultMulDepth;
    std::size_t logN       = DefaultLogN;
    std::size_t bit_len    = DefaultBitLen;
    std::size_t dcrt_bits  = DefaultDcrtBits;
    std::size_t rel_window = DefaultRelWindow;
    double      sigma      = DefaultSigma;

    static constexpr std::size_t DefaultMulDepth  = 4;
    static constexpr std::size_t DefaultLogN      = 13;
    static constexpr std::size_t DefaultBitLen    = 30;
    static constexpr std::size_t DefaultDcrtBits  = 60;
    static constexpr std::size_t DefaultRelWindow = 0;
    static constexpr double      DefaultSigma     = 3.2;
};

std::ostream& operator<<(std::ostream& os, const DecParam& param);
std::istream& operator>>(std::istream& is, DecParam& param);

} /* namespace prvc_share */

#endif /* PRVC_DECPARAM_HPP */
