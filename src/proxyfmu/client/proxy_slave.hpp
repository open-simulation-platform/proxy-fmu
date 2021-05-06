
#ifndef PROXY_FMU_PROXY_SLAVE_HPP
#define PROXY_FMU_PROXY_SLAVE_HPP

#include <proxyfmu/fmi/slave.hpp>
#include <proxyfmu/fs_portability.hpp>
#include <proxyfmu/remote_info.hpp>
#include <proxyfmu/thrift/FmuService.h>

#include <optional>
#include <thread>

using namespace apache::thrift;
using namespace apache::thrift::transport;

namespace proxyfmu::client
{

class proxy_slave : public fmi::slave
{

private:
    const fmi::model_description modelDescription_;
    std::shared_ptr<thrift::FmuServiceClient> client_;
    std::shared_ptr<TTransport> transport_;
    std::unique_ptr<std::thread> thread_;

    bool freed = false;

public:
    proxy_slave(
        const filesystem::path& fmuPath,
        const std::string& instanceName,
        fmi::model_description modelDescription,
        const std::optional<remote_info>& remote);

    [[nodiscard]] const fmi::model_description& get_model_description() const override;

    bool setup_experiment(double start_time, double stop_time, double tolerance) override;
    bool enter_initialization_mode() override;
    bool exit_initialization_mode() override;

    bool step(double current_time, double step_size) override;

    bool terminate() override;
    void freeInstance() override;

    bool get_integer(const std::vector<fmi::value_ref>& vr, std::vector<int>& values) override;
    bool get_real(const std::vector<fmi::value_ref>& vr, std::vector<double>& values) override;
    bool get_string(const std::vector<fmi::value_ref>& vr, std::vector<std::string>& values) override;
    bool get_boolean(const std::vector<fmi::value_ref>& vr, std::vector<bool>& values) override;

    bool set_integer(const std::vector<fmi::value_ref>& vr, const std::vector<int>& values) override;
    bool set_real(const std::vector<fmi::value_ref>& vr, const std::vector<double>& values) override;
    bool set_string(const std::vector<fmi::value_ref>& vr, const std::vector<std::string>& values) override;
    bool set_boolean(const std::vector<fmi::value_ref>& vr, const std::vector<bool>& values) override;

    ~proxy_slave() override;
};

} // namespace proxyfmu::client

#endif //PROXY_FMU_PROXY_SLAVE_HPP
