#include <fstream>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_log.hpp>
#include <prvc_share/prvc_utility.hpp>
#include <prvc_share/prvc_enctype.hpp>
#include <prvc_share/prvc_pubkey.hpp>

namespace prvc_share
{

struct PubKey::Impl
{
    Impl(const prvc_share::FHEContext& context)
        : data_(new lbcrypto::LPPublicKeyImpl<prvc_share::PolyType>(context))
    {}

    void save_to_stream(std::ostream& os) const
    {
        STDSC_LOG_INFO("saving public key.");
        lbcrypto::Serialized ser;
        data_->Serialize(&ser);
        lbcrypto::SerializableHelper::SerializationToStream(ser, os);
    }
    
    void load_from_stream(std::istream& is)
    {
        STDSC_LOG_INFO("loading public key.");
        lbcrypto::Serialized ser;
        lbcrypto::SerializableHelper::StreamToSerialization(is, &ser);
        data_->Deserialize(ser);
    }
    
    void save_to_file(const std::string& filepath) const
    {
        std::ofstream ofs(filepath);
        save_to_stream(ofs);
        ofs.close();
    }
    
    void load_from_file(const std::string& filepath)
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

    const prvc_share::FHEPubKey& get(void) const
    {
        return data_;
    }
    
private:
    FHEPubKey data_;
};

PubKey::PubKey(const prvc_share::FHEContext& context)
    : pimpl_(new Impl(context))
{}

void PubKey::save_to_stream(std::ostream& os) const
{
    pimpl_->save_to_stream(os);
}

void PubKey::load_from_stream(std::istream& is)
{
    pimpl_->load_from_stream(is);
}

void PubKey::save_to_file(const std::string& filepath) const
{
    pimpl_->save_to_file(filepath);
}
    
void PubKey::load_from_file(const std::string& filepath)
{
    pimpl_->load_from_file(filepath);
}

const prvc_share::FHEPubKey& PubKey::get(void) const
{
    return pimpl_->get();
}
    
} /* namespace prvc_share */
