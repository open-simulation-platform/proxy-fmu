
#ifndef FMU_PROXY_THRIFT_FMUSERVICEHANDLER_HPP
#define FMU_PROXY_THRIFT_FMUSERVICEHANDLER_HPP

#include <fmuproxy/fmi/fmu.hpp>
#include <fmuproxy/thrift/FmuService.h>

namespace fmuproxy::server
{

class fmu_service_handler : virtual public thrift::FmuServiceIf
{

private:
    std::unique_ptr<fmuproxy::fmi::fmu> fmu_;
    std::unique_ptr<fmuproxy::fmi::slave> slave_;

public:
    explicit fmu_service_handler(const std::string& fmu);

    void get_model_description(thrift::ModelDescription& _return) override;
    void create_instance(const std::string& instanceName) override;

    thrift::Status::type setup_experiment(double start, double stop, double tolerance) override;
    thrift::Status::type enter_initialization_mode() override;
    thrift::Status::type exit_initialization_mode() override;
    void step(thrift::StepResult& _return, double currentTime, double stepSize) override;
    thrift::Status::type terminate() override;
    void freeInstance() override;

    void read_integer(thrift::IntegerRead& _return, const thrift::ValueReferences& vr) override;
    void read_real(thrift::RealRead& _return, const thrift::ValueReferences& vr) override;
    void read_string(thrift::StringRead& _return, const thrift::ValueReferences& vr) override;
    void read_boolean(thrift::BooleanRead& _return, const thrift::ValueReferences& vr) override;

    thrift::Status::type write_integer(const thrift::ValueReferences& vr, const thrift::IntArray& value) override;
    thrift::Status::type write_real(const thrift::ValueReferences& vr, const thrift::RealArray& value) override;
    thrift::Status::type write_string(const thrift::ValueReferences& vr, const thrift::StringArray& value) override;
    thrift::Status::type write_boolean(const thrift::ValueReferences& vr, const thrift::BooleanArray& value) override;
};

} // namespace fmuproxy::thrift::server

#endif //FMU_PROXY_THRIFT_FMUSERVICEHANDLER_HPP
