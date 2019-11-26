#include <vector>
#include <fstream>

#include "cryptocontexthelper.h"
#include "utils/debug.h"
#include "encoding/encodings.h"
#include "utils/serializablehelper.h"

#include <stdsc/stdsc_exception.hpp>
#include <prvc_share/prvc_define.hpp>
#include <prvc_share/prvc_utility.hpp>
#include <prvc_share/prvc_encdata.hpp>


namespace prvc_share
{

static inline
bool WriteSerObjToFile(const lbcrypto::Serialized& serObj, const std::string& filename)
{
    std::ofstream ofs(filename, std::ios::binary | std::ios::trunc);
    return lbcrypto::SerializableHelper::SerializationToStream(serObj, ofs);
}

static inline
bool ReadSerObjFromFile(const std::string& filename, lbcrypto::Serialized& serObj)
{
    std::ifstream ifs(filename, std::ios::binary);
    return lbcrypto::SerializableHelper::StreamToSerialization(ifs, &serObj);
}
    
struct EncData::Impl
{
    struct StreamHeader
    {
        size_t num;
        size_t sizes[PRVC_MAX_SPLIT_NUM];
    };
    
    Impl(void)
    {}

    void save_to_stream(std::ostream& os) const
    {
        if (vec_ctxt_.size() == 0 || vec_ctxt_.size() >= PRVC_MAX_SPLIT_NUM) {
            std::ostringstream oss;
            oss << "Err: number of splitted encrypted data is invalid (num: " << vec_ctxt_.size()
                << ", max: " << PRVC_MAX_SPLIT_NUM << ")";
            STDSC_THROW_FAILURE(oss.str().c_str());
        }
        
        StreamHeader hdr;
        hdr.num = vec_ctxt_.size();

        std::vector<std::string> filenames(vec_ctxt_.size());

        for (auto i=0; i<vec_ctxt_.size(); ++i) {
            const auto& ctxt = vec_ctxt_[i];
            
            lbcrypto::Serialized serObj;
            STDSC_THROW_FAILURE_IF_CHECK(ctxt->Serialize(&serObj),
                                         "Err: failed to serialize data");
            
            std::ostringstream oss;
            oss << "ctxt-" << i << ".dat";
            filenames[i] = oss.str();
            
            std::ofstream ofs(filenames[i], std::ios::binary | std::ios::trunc);
            STDSC_THROW_FILE_IF_CHECK(lbcrypto::SerializableHelper::SerializationToStream(serObj, ofs),
                                      "Err: failed to write serialized data");

            hdr.sizes[i] = utility::file_size(oss.str());
        }

        os.write(reinterpret_cast<char*>(&hdr), sizeof(hdr));
        
        for (auto i=0; i<vec_ctxt_.size(); ++i) {
            std::ifstream ifs(filenames[i], std::ios::binary);
            if (!ifs.is_open())
            {
                std::ostringstream oss;
                oss << "failed to open. (" << filenames[i] << ")";
                STDSC_THROW_FILE(oss.str());
            }
            else
            {
                auto size = hdr.sizes[i];
                std::shared_ptr<char> buffer(new char[size]);;
                ifs.read(buffer.get(), size);

                os.write(buffer.get(), size);
            }
        }
    }

    void load_from_stream(std::istream& is, const FHEContext& context)
    {
        StreamHeader hdr;
        is.read(reinterpret_cast<char*>(&hdr), sizeof(hdr));

        vec_ctxt_.resize(hdr.num);

        std::vector<std::string> filenames(vec_ctxt_.size());
        
        for (auto i=0; i<hdr.num; ++i) {
            std::ostringstream oss;
            oss << "out_ctxt-" << i << ".dat";
            filenames[i] = oss.str();
            
            auto size = hdr.sizes[i];
            std::shared_ptr<char> buffer(new char[size]);;
            is.read(buffer.get(), size);

            std::ofstream ofs(filenames[i], std::ios::binary | std::ios::trunc);
            ofs.write(buffer.get(), size);
            ofs.close();
        }

        for (auto i=0; i<hdr.num; ++i) {
            lbcrypto::Serialized serObj;
            std::ifstream ifs(filenames[i], std::ios::binary);
            STDSC_THROW_FILE_IF_CHECK(lbcrypto::SerializableHelper::StreamToSerialization(ifs, &serObj),
                                      "Err: failed to read serialized data");

            Ctxt ctxt(new lbcrypto::CiphertextImpl<PolyType>(context));
            STDSC_THROW_FAILURE_IF_CHECK(ctxt->Deserialize(serObj),
                                         "Err: failed to deserialize data");

            vec_ctxt_[i] = ctxt;
        }
    }

    size_t stream_size(void) const
    {
        std::ostringstream oss;
        save_to_stream(oss);
        return oss.str().size();
    }
    
    const std::vector<Ctxt>& data(void) const
    {
        return vec_ctxt_;
    }
    
    std::vector<Ctxt>& data(void)
    {
        return vec_ctxt_;
    }
    
private:
    std::vector<Ctxt> vec_ctxt_;
};

EncData::EncData(void)
    : pimpl_(new Impl())
{
}

void EncData::save_to_stream(std::ostream& os) const
{
    pimpl_->save_to_stream(os);
}

void EncData::load_from_stream(std::istream& is,
                               const FHEContext& context)
{
    pimpl_->load_from_stream(is, context);
}

size_t EncData::stream_size(void) const
{
    return pimpl_->stream_size();
}

const std::vector<Ctxt>& EncData::data(void) const
{
    return pimpl_->data();
}

std::vector<Ctxt>& EncData::data(void)
{
    return pimpl_->data();
}
    
} /* namespace prvc_share */
