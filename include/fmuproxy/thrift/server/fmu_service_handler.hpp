
#ifndef FMU_PROXY_FMUSERVICEHANDLER_H
#define FMU_PROXY_FMUSERVICEHANDLER_H

#include <vector>
#include <unordered_map>

#include <fmi4cpp/fmi2/fmi2.hpp>

#include "fmuproxy/thrift/common/fmu_service.h"

namespace fmuproxy::thrift::server {

    class fmu_service_handler : virtual public fmu_service_if {

    private:
        std::unordered_map<FmuId, std::shared_ptr<fmi4cpp::fmi2::cs_fmu>> &fmus_;
        std::unordered_map<InstanceId, std::unique_ptr<fmi4cpp::fmu_slave<fmi4cpp::fmi2::cs_model_description>>> slaves_;

    public:
        explicit fmu_service_handler(std::unordered_map<FmuId, std::shared_ptr<fmi4cpp::fmi2::cs_fmu>> &fmus);

        void load_from_file(FmuId &_return, const std::string &name, const std::string &data) override;

        void get_model_description(ModelDescription &_return, const FmuId &fmu_id) override;

        void create_instance(InstanceId &_return, const FmuId &fmu_id) override;

        Status::type setup_experiment(const InstanceId &instanceId, double start, double stop,
                                     double tolerance) override;

        Status::type enter_initialization_mode(const InstanceId &instanceId) override;

        Status::type exit_initialization_mode(const InstanceId &instanceId) override;

        void step(::fmuproxy::thrift::StepResult &_return, const InstanceId &instance_id,
                  double step_size) override;

        Status::type reset(const InstanceId &instance_id) override;

        Status::type terminate(const InstanceId &instance_id) override;

        void freeInstance(const InstanceId& instanceId) override;

        void read_integer(::fmuproxy::thrift::IntegerRead &_return, const InstanceId &instance_id,
                         const ValueReferences &vr) override;

        void read_real(::fmuproxy::thrift::RealRead &_return, const InstanceId &instance_id,
                      const ValueReferences &vr) override;

        void read_string(::fmuproxy::thrift::StringRead &_return, const InstanceId &instance_id,
                        const ValueReferences &vr) override;

        void read_boolean(::fmuproxy::thrift::BooleanRead &_return, const InstanceId &instance_id,
                         const ValueReferences &vr) override;

        Status::type write_integer(const InstanceId &instance_id, const ValueReferences &vr,
                                  const IntArray &value) override;

        Status::type write_real(const InstanceId &instance_id, const ValueReferences &vr,
                               const RealArray &value) override;

        Status::type write_string(const InstanceId &instance_id, const ValueReferences &vr,
                                 const StringArray &values) override;

        Status::type write_boolean(const InstanceId &instance_id, const ValueReferences &vr,
                                  const BooleanArray &value) override;

        void get_directional_derivative(DirectionalDerivativeResult &_return, const InstanceId &slave_id,
                                      const ValueReferences &vUnknownRef, const ValueReferences &vKnownRef,
                                      const std::vector<double> &dvKnownRef) override;

    };

}

#endif //FMU_PROXY_FMUSERVICEHANDLER_H
