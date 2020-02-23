#include <fstream>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_log.hpp>
#include <prvc_share/prvc_utility.hpp>
#include <prvc_share/prvc_enctype.hpp>
#include <prvc_share/prvc_cmp_param_list.hpp>
#include <prvc_share/prvc_context.hpp>

namespace prvc_share
{

struct Context::Impl
{
    Impl()
    {}

#if 0
    void generate(FHEKeyPair& keypair,
                  const std::size_t mul_depth,
                  const std::size_t logN,
                  const std::size_t rel_window,
                  const std::size_t dcrt_bits)
    {
        double root_hermit = 0.0;
        GetRootHermit(mul_depth, logN, rel_window, dcrt_bits, root_hermit);

        lbcrypto::PlaintextModulus ptm = (1 << logN) - 1;
        double sigma = SecureKeyFileManager::DefaultSigma;

        data_ = lbcrypto::CryptoContextFactory<PolyType>::genCryptoContextBFVrns(
            ptm, root_hermit, sigma, 0, mul_depth, 0, OPTIMIZED, 2, rel_window, dcrt_bits);
        data_->Enable(ENCRYPTION);
        data_->Enable(SHE);

        keypair = data_->KeyGen();

        data_->EvalMultKeysGen(keypair.secretKey);
        STDSC_THROW_FAILURE_IF_CHECK(keypair.good(), "failed to generate key");
        auto N = data_->GetCryptoParameters()->GetElementParams()->GetCyclotomicOrder() / 2;
        
        // Evalkey
        EvkAut eval_automorph_ks;
        vector<usint> index_list{N + 1}; // now just for x^{N+1}
        for (usint i=2; i!=N; i=i<<1) {
            index_list.push_back(i + 1);
        }
        index_list.push_back(2 * N - 1);
        eval_automorph_ks = data_->EvalAutomorphismKeyGen(keypair.secretKey, index_list);
        data_->InsertEvalAutomorphismKey(eval_automorph_ks);
    }
#endif
    
    void save_to_stream(std::ostream& os, const Kind_t kind) const
    {
        STDSC_IF_CHECK(data_, "resource is empty");
        
        lbcrypto::Serialized ser;

        if (kind == kKindEMK) {
            data_->SerializeEvalMultKey(&ser);
        } else if (kind == kKindEAK) {
            data_->SerializeEvalAutomorphismKey(&ser);
        } else {
            data_->Serialize(&ser);
        }
        lbcrypto::SerializableHelper::SerializationToStream(ser, os);
    }
    
    void load_from_stream(std::istream& is, const Kind_t kind)
    {
        lbcrypto::Serialized ser;

        if (kind == kKindEMK) {
            STDSC_IF_CHECK(data_, "resource is empty");
            lbcrypto::SerializableHelper::StreamToSerialization(is, &ser);
            data_->DeserializeEvalMultKey(ser);
        } else if (kind == kKindEAK) {
            STDSC_IF_CHECK(data_, "resource is empty");
            lbcrypto::SerializableHelper::StreamToSerialization(is, &ser);
            data_->DeserializeEvalAutomorphismKey(ser);
        } else {
            lbcrypto::SerializableHelper::StreamToSerialization(is, &ser);
            data_ = lbcrypto::CryptoContextFactory<PolyType>::DeserializeAndCreateContext(ser);
        }
    }
    
    void save_to_file(const std::string& filepath, const Kind_t kind) const
    {
        std::ofstream ofs(filepath);
        save_to_stream(ofs, kind);
        ofs.close();
    }
    
    void load_from_file(const std::string& filepath, const Kind_t kind)
    {
        if (!prvc_share::utility::file_exist(filepath)) {
            std::ostringstream oss;
            oss << "File not found. (" << filepath << ")";
            STDSC_THROW_FILE(oss.str());
        }
        std::ifstream ifs(filepath, std::ios::binary);
        load_from_stream(ifs, kind);
        ifs.close();
    }

    const FHEContext& get(void) const
    {
        return data_;
    }
    
private:
    FHEContext data_;
};

Context::Context(void) : pimpl_(new Impl())
{}

#if 0
void Context::generate(FHEKeyPair& keypair,
                       const std::size_t mul_depth,
                       const std::size_t logN,
                       const std::size_t rel_window,
                       const std::size_t dcrt_bits)
{
    pimpl_->generate(keypair, mul_depth, logN, rel_window, dcrt_bits);
}
#endif

void Context::save_to_stream(std::ostream& os, const Kind_t kind) const
{
    pimpl_->save_to_stream(os, kind);
}

void Context::load_from_stream(std::istream& is, const Kind_t kind)
{
    pimpl_->load_from_stream(is, kind);
}

void Context::save_to_file(const std::string& filepath, const Kind_t kind) const
{
    pimpl_->save_to_file(filepath, kind);
}
    
void Context::load_from_file(const std::string& filepath, const Kind_t kind)
{
    pimpl_->load_from_file(filepath, kind);
}

const FHEContext& Context::get(void) const
{
    return pimpl_->get();
}
    
} /* namespace prvc_share */
