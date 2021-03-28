
#ifndef FMU_PROXY_THRIFT_FMUSERVICEHANDLER_HPP
#define FMU_PROXY_THRIFT_FMUSERVICEHANDLER_HPP

#include <fmi/fmu.hpp>
#include <fmuproxy/thrift/FmuService.h>

namespace fmuproxy::thrift::server
{

class fmu_service_handler : virtual public FmuServiceIf
{

private:
    std::unique_ptr<fmi::fmu> fmu_;
    std::unique_ptr<fmi::slave> slave_;

public:

    explicit fmu_service_handler(const std::string& fmu);

    void get_model_description(ModelDescription& _return) override;
    void create_instance(const std::string& instanceName) override;

    Status::type setup_experiment(double start, double stop, double tolerance) override;
    Status::type enter_initialization_mode() override;
    Status::type exit_initialization_mode() override;
    void step(StepResult& _return, double currentTime, double stepSize) override;
    Status::type terminate() override;
    void freeInstance() override;

    void read_integer(IntegerRead& _return, const ValueReferences& vr) override;
    void read_real(RealRead& _return, const ValueReferences& vr) override;
    void read_string(StringRead& _return, const ValueReferences& vr) override;
    void read_boolean(BooleanRead& _return, const ValueReferences& vr) override;

    Status::type write_integer(const ValueReferences& vr, const IntArray& value) override;
    Status::type write_real(const ValueReferences& vr, const RealArray& value) override;
    Status::type write_string(const ValueReferences& vr, const StringArray& value) override;
    Status::type write_boolean(const ValueReferences& vr, const BooleanArray& value) override;

};

} // namespace fmuproxy::thrift::server

#endif //FMU_PROXY_THRIFT_FMUSERVICEHANDLER_HPP
