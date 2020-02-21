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

#ifndef PRVC_CONTEXT_HPP
#define PRVC_CONTEXT_HPP

#include <iostream>
#include <string>
#include <memory>

#include <prvc_share/prvc_enctype.hpp>

namespace prvc_share
{

/**
 * @brief This class is used to hold the context.
 */
struct Context
{
    Context(void);
    ~Context(void) = default;

    void generate(FHEKeyPair& keypair,
                  const std::size_t mul_depth  = DefaultMulDepth,
                  const std::size_t logN       = DefaultLogN,
                  const std::size_t rel_window = DefaultRelWindow,
                  const std::size_t dcrt_bits  = DefaultDcrtBits);
    
    void save_to_stream(std::ostream& os) const;
    void load_from_stream(std::istream& is);

    void save_to_file(const std::string& filepath) const;
    void load_from_file(const std::string& filepath);

    const FHEcontext& get(void) const;

private:
    static constexpr std::size_t DefaultMulDepth  = 4;
    static constexpr std::size_t DefaultLogN      = 13;
    static constexpr std::size_t DefaultDcrtBits  = 60;
    static constexpr std::size_t DefaultRelWindow = 0;
    static constexpr double      DefaultSigma     = 32;
    
private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace prvc_share */

#endif /* PRVC_CONTEXT_HPP */
