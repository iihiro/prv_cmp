#ifndef PRVC_ENCUTILITY_HPP
#define PRVC_ENCUTILITY_HPP

#include <stdsc/stdsc_exception.hpp>

namespace prvc_share {
namespace encutility {

static
void split_values(const size_t numbit,
                  const size_t bit_per_chunk,
                  const uint64_t value,
                  size_t& nsplit,
                  std::vector<uint64_t>& s_values)
{
    if (numbit <= bit_per_chunk) {
        nsplit = 1;
        s_values.push_back(value);
    } else {
        nsplit = std::ceil(static_cast<double>(numbit) / bit_per_chunk);
        uint64_t mask = (1 << (bit_per_chunk + 1)) - 1;
        uint64_t tmp;
        for (size_t i=0; i<nsplit; ++i) {
            tmp = value >> (i * bit_per_chunk);
            s_values.push_back(tmp & mask);
        }
    }
}

static inline
void make_monomial_coeff(const uint64_t d,
                         const uint64_t N,
                         const bool is_neg,
                         std::vector<int64_t>& coeff)
{
    if (!(d <= N)) {
        std::ostringstream oss;
        oss << "Err: invalid params. (d: " << d << ", N: " << N;
        STDSC_THROW_INVPARAM_IF_CHECK(d <= N, oss.str().c_str());
    }
                                  
    if (d == N) {
        coeff[0] = -1;
        return;
    }
    
    if (d == 0) {
        coeff[0] = 1;
        return;
    }
    
    if (is_neg) {
        coeff[N - d] = -1;
    } else {
        coeff[d] = 1;
    }
}

static
void encode_and_encrypt_splitted_values(
    const FHEContext context,
    const lbcrypto::LPPublicKey<PolyType>& pubkey,
    const size_t nsplit,
    const bool is_neg,
    const std::vector<uint64_t>& s_values,
    prvc_share::EncData& encdata)
{
    std::vector<Ctxt>& e_s_values = encdata.data();
    
    const usint N = context->GetCryptoParameters()->GetElementParams()->GetCyclotomicOrder() / 2;
    for (size_t i=0; i<nsplit; ++i) {
        vector<int64_t> coeffs(N, 0);
        make_monomial_coeff(s_values[i], N, is_neg, coeffs);
        auto poly = context->MakeCoefPackedPlaintext(coeffs);
        auto c_monomial = context->Encrypt(pubkey, poly);
        e_s_values.push_back(c_monomial);
    }
}
    
static
bool write_to_file(const lbcrypto::Serialized& serObj, const std::string& filename)
{
    std::ofstream ofs(filename, std::ios::binary | std::ios::trunc);
    return lbcrypto::SerializableHelper::SerializationToStream(serObj, ofs);
}

static
bool read_from_file(const std::string& filename, lbcrypto::Serialized& serObj)
{
    std::ifstream ifs(filename, std::ios::binary);
    return lbcrypto::SerializableHelper::StreamToSerialization(ifs, &serObj);
}

} /* namespace prvc_share */
} /* namespace encutility */


#endif /*PRVC_ENCUTILITY_HPP*/
