#include <fstream>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_log.hpp>
#include <prvc_share/prvc_utility.hpp>
#include <prvc_share/prvc_enctype.hpp>
#include <prvc_share/prvc_seckey.hpp>

namespace prvc_share
{

struct SecKey::Impl
{
    Impl(const prvc_share::FHEcontext& context)
    // 以下を有効にしてビルドすると、以下のエラーが出る問題を解決できていない
    // -----
    //   [ 97%] Linking CXX executable dec
    //   usr/bin/ld: _ZN8lbcrypto27PseudoRandomNumberGenerator6m_prngE: /home/iizuka/palisade-release/bin/lib/libPALISADEpke.so 内の TLS 参照は ../../prvc/prvc_share/libprvc_share.so 内の非 TLS 参照と一致しません
    //   home/iizuka/palisade-release/bin/lib/libPALISADEpke.so: error adding symbols: 不正な値です
    //   collect2: error: ld returned 1 exit status
    // -----
    // ただ、prvcmpの実装上はDecryptorで生成したcontextとそこから作ったkeypairがあれば
    // decrypt処理はできるため、一旦このクラスは使わない方針とする
    //    : data_(new lbcrypto::LPPrivateKeyImpl<prvc_share::PolyType>(context))
    {}

    void save_to_stream(std::ostream& os) const
    {
        STDSC_LOG_INFO("saving secret key.");
        lbcrypto::Serialized ser;
        data_->Serialize(&ser);
        lbcrypto::SerializableHelper::SerializationToStream(ser, os);
    }
    
    void load_from_stream(std::istream& is)
    {
        STDSC_LOG_INFO("loading secret key.");
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

    const prvc_share::FHESecKey& get(void) const
    {
        return data_;
    }
    
private:
    FHESecKey data_;
};

SecKey::SecKey(const prvc_share::FHEcontext& context)
    : pimpl_(new Impl(context))
{}

void SecKey::save_to_stream(std::ostream& os) const
{
    pimpl_->save_to_stream(os);
}

void SecKey::load_from_stream(std::istream& is)
{
    pimpl_->load_from_stream(is);
}

void SecKey::save_to_file(const std::string& filepath) const
{
    pimpl_->save_to_file(filepath);
}
    
void SecKey::load_from_file(const std::string& filepath)
{
    pimpl_->load_from_file(filepath);
}

const prvc_share::FHESecKey& SecKey::get(void) const
{
    return pimpl_->get();
}
    
} /* namespace prvc_share */
