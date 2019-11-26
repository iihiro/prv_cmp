#ifndef PRV_ENCTYPE_HPP
#define PRV_ENCTYPE_HPP

#include <memory>
#include "cryptocontexthelper.h"
#include "utils/debug.h"
#include "encoding/encodings.h"
#include "utils/serializablehelper.h"

using PolyType   = lbcrypto::DCRTPoly;
using Ctxt       = lbcrypto::Ciphertext<PolyType>;
using EvkAut     = shared_ptr<map<usint, lbcrypto::LPEvalKey<PolyType>>>;
using FHEContext = lbcrypto::CryptoContext<PolyType>;

#endif /*PRV_ENCTYPE_HPP*/
