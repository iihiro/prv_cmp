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

#ifndef PRVC_PACKET_HPP
#define PRVC_PACKET_HPP

#include <cstdint>

namespace prvc_share
{

/**
 * @brief Enumeration for control code of packet.
 */
enum ControlCode_t : uint64_t
{
    /* Code for Request packet: 0x201-0x2FF */
    kControlCodeRequestConnect = 0x201,
    kControlCodeRequestDisconnect = 0x201,

    /* Code for Data packet: 0x401-0x4FF */
    kControlCodeDataPubkey = 0x401,
    kControlCodeDataEncResult = 0x402,
    /* Code for Download packet: 0x801-0x8FF */
    kControlCodeDownloadPubkey = 0x801,
};

} /* namespace prvc_share */

#endif /* PRVC_PACKET_HPP */
