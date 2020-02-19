#include <fstream>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_log.hpp>
#include <prvc_share/prvc_context.hpp>
#include <prvc_share/prvc_utility.hpp>
#include <prvc_share/prvc_enctype.hpp>

namespace prvc_share
{

struct Context::Impl
{
    Impl()
    {}

    void save_to_stream(std::ostream& os) const
    {
        STDSC_LOG_WARN("%s is not implemented.", __FUNCTION__);
    }
    
    void load_from_stream(std::istream& is)
    {
        lbcrypto::Serialized ser;
        lbcrypto::SerializableHelper::StreamToSerialization(is, &ser);
        data_ = lbcrypto::CryptoContextFactory<PolyType>::DeserializeAndCreateContext(ser);
    }
    
    void save_to_file(const std::string& filepath) const
    {
        STDSC_LOG_WARN("%s is not implemented.", __FUNCTION__);
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

    const FHEcontext& get(void) const
    {
        return data_;
    }
    
private:
    FHEcontext data_;
};

Context::Context(void) : pimpl_(new Impl())
{}

void Context::save_to_stream(std::ostream& os) const
{
    pimpl_->save_to_stream(os);
}

void Context::load_from_stream(std::istream& is)
{
    pimpl_->load_from_stream(is);
}

void Context::save_to_file(const std::string& filepath) const
{
    pimpl_->save_to_file(filepath);
}
    
void Context::load_from_file(const std::string& filepath)
{
    pimpl_->load_from_file(filepath);
}

const FHEcontext& Context::get(void) const
{
    return pimpl_->get();
}
    
} /* namespace prvc_share */
