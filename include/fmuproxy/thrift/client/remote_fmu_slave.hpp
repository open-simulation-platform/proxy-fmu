
#ifndef FMU_PROXY_THRIFT_REMOTEFMUSLAVE_HPP
#define FMU_PROXY_THRIFT_REMOTEFMUSLAVE_HPP

#include <fmuproxy/thrift/FmuService.h>

#include <fmi4cpp/fmi2/fmi2.hpp>

namespace fmuproxy::thrift::client
{

class remote_fmu_slave : public fmi4cpp::fmu_slave<fmi4cpp::fmi2::cs_model_description>
{

private:
    const InstanceId instanceId_;
    FmuServiceClient& client_;
    std::shared_ptr<const fmi4cpp::fmi2::cs_model_description> modelDescription_;

    bool terminated_ = false;
    Status::type lastStatus_;

    bool update_status_and_return_true_on_ok(Status::type status);

public:
    remote_fmu_slave(InstanceId instanceId, FmuServiceClient& client,
        std::shared_ptr<const fmi4cpp::fmi2::cs_model_description>& modelDescription);

    fmi4cpp::status last_status() const override;

    std::shared_ptr<const fmi4cpp::fmi2::cs_model_description> get_model_description() const override;

    bool setup_experiment(double startTime = 0, double stopTime = 0, double tolerance = 0) override;

    bool enter_initialization_mode() override;

    bool exit_initialization_mode() override;

    bool reset() override;

    bool terminate() override;

    bool get_fmu_state(fmi2FMUstate& state) override;

    bool set_fmu_state(fmi2FMUstate state) override;

    bool free_fmu_state(fmi2FMUstate& state) override;

    bool serialize_fmu_state(const fmi2FMUstate& state, std::vector<fmi2Byte>& serializedState) override;

    bool de_serialize_fmu_state(fmi2FMUstate& state, const std::vector<fmi2Byte>& serializedState) override;

    bool get_directional_derivative(const std::vector<fmi2ValueReference>& vUnkownRef,
        const std::vector<fmi2ValueReference>& vKnownRef,
        const std::vector<fmi2Real>& dvKnownRef,
        std::vector<fmi2Real>& dvUnknownRef) override;

    bool step(double stepSize) override;

    bool cancel_step() override;

    bool read_integer(fmi2ValueReference vr, fmi2Integer& ref) override;

    bool read_integer(const std::vector<fmi2ValueReference>& vr, std::vector<fmi2Integer>& ref) override;

    bool read_real(fmi2ValueReference vr, fmi2Real& ref) override;

    bool read_real(const std::vector<fmi2ValueReference>& vr, std::vector<fmi2Real>& ref) override;

    bool read_string(fmi2ValueReference vr, fmi2String& ref) override;

    bool read_string(const std::vector<fmi2ValueReference>& vr, std::vector<fmi2String>& ref) override;

    bool read_boolean(fmi2ValueReference vr, fmi2Boolean& ref) override;

    bool read_boolean(const std::vector<fmi2ValueReference>& vr, std::vector<fmi2Boolean>& ref) override;

    bool write_integer(fmi2ValueReference vr, fmi2Integer value) override;

    bool write_integer(const std::vector<fmi2ValueReference>& vr, const std::vector<fmi2Integer>& values) override;

    bool write_real(fmi2ValueReference vr, fmi2Real value) override;

    bool write_real(const std::vector<fmi2ValueReference>& vr, const std::vector<fmi2Real>& values) override;

    bool write_string(fmi2ValueReference vr, fmi2String value) override;

    bool write_string(const std::vector<fmi2ValueReference>& vr, const std::vector<fmi2String>& values) override;

    bool write_boolean(fmi2ValueReference vr, fmi2Boolean value) override;

    bool write_boolean(const std::vector<fmi2ValueReference>& vr, const std::vector<fmi2Boolean>& values) override;

    ~remote_fmu_slave() override;
};

} // namespace fmuproxy::thrift::client

#endif //FMU_PROXY_THRIFT_REMOTEFMUSLAVE_HPP
