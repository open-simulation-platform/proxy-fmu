
#ifndef FMU_PROXY_THRIFT_CLIENT_HPP
#define FMU_PROXY_THRIFT_CLIENT_HPP

#include <memory>

#include <fmuproxy/fmi/slave.hpp>
#include <fmuproxy/fmi/model_description.hpp>

namespace fmuproxy::client
{

class thrift_client
{

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl_;

public:
    thrift_client(const std::string& host, int port);

    [[nodiscard]] const fmi::model_description& get_model_description() const;

    std::unique_ptr<fmi::slave> new_instance(const std::string& name);

    void close();
    ~thrift_client();
};

} // namespace fmuproxy::thrift::client


#endif //FMU_PROXY_THRIFT_CLIENT_HPP
