#include <vector>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_log.hpp>
#include <prvc_share/prvc_utility.hpp>
#include <prvc_share/prvc_encdata.hpp>
#include <prvc_share/prvc_pubkey.hpp>
#include <prvc_share/prvc_context.hpp>

namespace prvc_share
{

struct EncData::Impl
{
    explicit Impl(const Context& context)
        : context_(context)
    {}
    
    const Context& context_;
};

EncData::EncData(const Context& context)
    : pimpl_(new Impl(context))
{}

void EncData::encrypt(const std::vector<lbcrypto::Plaintext>& inputdata,
                      const PubKey& pubkey)
{
    auto& cc = pimpl_->context_.get();
    for (const auto& ptxt : inputdata) {
        auto ctxt = cc->Encrypt(pubkey.get(), ptxt);
        vec_.push_back(ctxt);
    }
}

void EncData::decrypt(const FHESecKey& seckey,
                      std::vector<lbcrypto::Plaintext>& outputdata) const
{
    auto& cc = pimpl_->context_.get();
    for (const auto& ctxt : vec_) {
        lbcrypto::Plaintext ptxt;
        cc->Decrypt(seckey, ctxt, &ptxt);
        outputdata.push_back(ptxt);
    }
}

void EncData::save_to_stream(std::ostream& os) const
{
    os << vec_.size() << std::endl;
    
    for (const auto& v : vec_) {
        lbcrypto::Serialized ser;
        v->Serialize(&ser);
        lbcrypto::SerializableHelper::SerializationToStream(ser, os);
    }
}

void EncData::load_from_stream(std::istream& is)
{
    size_t sz;
    is >> sz;
    
    clear();

    for (size_t i=0; i<sz; ++i) {
        lbcrypto::Serialized ser;
        lbcrypto::SerializableHelper::StreamToSerialization(is, &ser);
        Ctxt ctxt(new lbcrypto::CiphertextImpl(pimpl_->context_.get()));
        ctxt->Deserialize(ser);
        vec_.push_back(ctxt);
    }
}

void EncData::save_to_file(const std::string& filepath) const
{
    std::ofstream ofs(filepath);
    save_to_stream(ofs);
    ofs.close();
}

void EncData::load_from_file(const std::string& filepath)
{
    if (!prvc_share::utility::file_exist(filepath)) {
        std::ostringstream oss;
        oss << "File not found. (" << filepath << ")";
        STDSC_THROW_FILE(oss.str());
    }
    std::ifstream ifs(filepath, std::ios::binary);
    load_from_stream(ifs);
    ifs.close();
}

} /* namespace prvc_share */
