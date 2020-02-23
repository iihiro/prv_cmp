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
    /* Code for Request packet: 0x0201-0x20FF */

    /* Code for Data packet: 0x0401-0x04FF */
    kControlCodeDataPubkey         = 0x0401,
    kControlCodeDataContext        = 0x0402,
    kControlCodeDataEMK            = 0x0403,
    kControlCodeDataEAK            = 0x0404,
    kControlCodeDataDecParam       = 0x0405,
    kControlCodeDataEncInput       = 0x0406,
    kControlCodeDataEncResult      = 0x0407,
    
    /* Code for Download packet: 0x801-0x8FF */
    kControlCodeDownloadPubkey     = 0x0801,
    kControlCodeDownloadContext    = 0x0802,
    kControlCodeDownloadEMK        = 0x0803,
    kControlCodeDownloadEAK        = 0x0804,
    kControlCodeDownloadParam      = 0x0805,

    /* Code for UpDownload packet: 0x1000-0x10FF */
    kControlCodeGroupUpDownload    = 0x1000,
};

struct PubkeyDataHeader
{
    uint64_t pubkey_size;
    uint64_t context_size;
    uint64_t data_size()
    {
        return pubkey_size + context_size;
    }
};


} /* namespace prvc_share */

#endif /* PRVC_PACKET_HPP */
