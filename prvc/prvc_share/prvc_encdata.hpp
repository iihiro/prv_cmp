#ifndef PRVC_ENCDATA_HPP
#define PRVC_ENCDATA_HPP

#include <iostream>
#include <memory>
#include <vector>

#include <prvc_share/prvc_enctype.hpp>

namespace prvc_share
{

/**
 * @brief This class is used to hold the encrypted data
 */
struct EncData
{
    EncData(void);
    ~EncData(void) = default;

    void save_to_stream(std::ostream& os) const;

    void load_from_stream(std::istream& is, const FHEContext& context);

    size_t stream_size(void) const;
    
    const std::vector<Ctxt>& data(void) const;
    std::vector<Ctxt>& data(void);
    
private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace prvc_share */

#endif /* PRVC_ENCDATA_HPP */
