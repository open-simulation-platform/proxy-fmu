
#ifndef FMU_PROXY_THRIFT_FMUSERVICEHANDLER_HPP
#define FMU_PROXY_THRIFT_FMUSERVICEHANDLER_HPP

#include <fmuproxy/thrift/FmuService.h>

#include <unordered_map>
#include <vector>

namespace fmuproxy::thrift::server
{

class fmu_service_handler : virtual public FmuServiceIf
{

private:

public:
    fmu_service_handler(const std::string &fmu);
    
    void get_model_description(ModelDescription& _return) override;

    void create_instance() override;

    Status::type setup_experiment(double start, double stop, double tolerance) override;

    Status::type enter_initialization_mode() override;

    Status::type exit_initialization_mode() override;

    void step(::fmuproxy::thrift::StepResult& _return, double step_size) override;

    Status::type reset() override;

    Status::type terminate() override;

    void freeInstance() override;

    void read_integer(::fmuproxy::thrift::IntegerRead& _return, const ValueReferences& vr) override;

    void read_real(::fmuproxy::thrift::RealRead& _return, const ValueReferences& vr) override;

    void read_string(::fmuproxy::thrift::StringRead& _return, const ValueReferences& vr) override;

    void read_boolean(::fmuproxy::thrift::BooleanRead& _return, const ValueReferences& vr) override;

    Status::type write_integer(const ValueReferences& vr, const IntArray& value) override;

    Status::type write_real(const ValueReferences& vr, const RealArray& value) override;

    Status::type write_string(const ValueReferences& vr, const StringArray& values) override;

    Status::type write_boolean(const ValueReferences& vr, const BooleanArray& value) override;

};

} // namespace fmuproxy::thrift::server

#endif //FMU_PROXY_THRIFT_FMUSERVICEHANDLER_HPP
