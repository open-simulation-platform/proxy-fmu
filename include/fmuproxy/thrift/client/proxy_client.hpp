
#ifndef FMU_PROXY_PROXY_CLIENT_HPP
#define FMU_PROXY_PROXY_CLIENT_HPP

#include <fmuproxy/fmi/model_description.hpp>
#include <fmuproxy/fmi/slave.hpp>

#include <memory>

namespace fmuproxy::client
{

class proxy_client
{

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl_;

public:
    proxy_client(const std::string& host, int port);

    [[nodiscard]] const fmi::model_description& get_model_description() const;

    std::unique_ptr<fmi::slave> new_instance(const std::string& name);

    void close();
    ~proxy_client();
};

} // namespace fmuproxy::client


#endif //FMU_PROXY_PROXY_CLIENT_HPP
