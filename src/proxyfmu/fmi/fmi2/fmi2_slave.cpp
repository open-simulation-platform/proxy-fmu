
#include "fmi2_slave.hpp"

#include <fmilib.h>

#include <exception>
#include <memory>

namespace
{
void fmilogger(fmi2_component_t /*c*/, fmi2_string_t /*instanceName*/, fmi2_status_t /*status*/, fmi2_string_t /*category*/, fmi2_string_t /*message*/, ...)
{
    //    va_list argp;
    //    va_start(argp, message);
    //    fmi2_log_forwarding_v(c, instanceName, status, category, message, argp);
    //    va_end(argp);
}
} // namespace

namespace proxyfmu::fmi
{

fmi2_slave::fmi2_slave(
    const std::shared_ptr<fmicontext>& ctx,
    const std::string& instanceName,
    model_description md,
    std::shared_ptr<temp_dir> tmpDir)
    : ctx_(ctx)
    , md_(std::move(md))
    , tmpDir_(std::move(tmpDir))
    , handle_(fmi2_import_parse_xml(ctx->ctx_, tmpDir->path().string().c_str(), nullptr))
    , instanceName_(instanceName)
{
    fmi2_callback_functions_t callbackFunctions;
    callbackFunctions.allocateMemory = calloc;
    callbackFunctions.freeMemory = free;
    callbackFunctions.logger = &fmilogger;
    callbackFunctions.componentEnvironment = nullptr;
    callbackFunctions.stepFinished = nullptr;

    if (fmi2_import_create_dllfmu(handle_, fmi2_fmu_kind_cs, &callbackFunctions) != jm_status_success) {
        throw std::runtime_error(std::string("failed to load fmu dll! Error: ") + fmi2_import_get_last_error(handle_));
    }

    const auto rc = fmi2_import_instantiate(
        handle_,
        instanceName.c_str(),
        fmi2_cosimulation,
        nullptr,
        fmi2_false);

    if (rc != jm_status_success) {
        fmi2_import_destroy_dllfmu(handle_);
        fmi2_import_free(handle_);
        throw std::runtime_error("failed to instantiate fmu!");
    }
}

const model_description& fmi2_slave::get_model_description() const
{
    return md_;
}

bool fmi2_slave::setup_experiment(double start_time, double stop_time, double tolerance)
{
    assert(!setupComplete_);
    fmi2_boolean_t stop_defined = (stop_time > 0) ? fmi2_true : fmi2_false;
    fmi2_boolean_t tolerance_defined = (tolerance > 0) ? fmi2_true : fmi2_false;
    auto status = fmi2_import_setup_experiment(handle_, tolerance_defined, tolerance, start_time, stop_defined, stop_time);
    return status == fmi2_status_ok;
}

bool fmi2_slave::enter_initialization_mode()
{
    auto status = fmi2_import_enter_initialization_mode(handle_);
    auto is_okay = status == fmi2_status_ok;

    if (is_okay) {
        setupComplete_ = true;
    }

    return is_okay;
}

bool fmi2_slave::exit_initialization_mode()
{
    assert(setupComplete_);
    assert(!simStarted_);
    auto status = fmi2_import_exit_initialization_mode(handle_);
    auto is_okay = status == fmi2_status_ok;

    if (is_okay) {
        simStarted_ = true;
    }

    return is_okay;
}

bool fmi2_slave::step(double current_time, double step_size)
{
    assert(simStarted_);
    auto status = fmi2_import_do_step(handle_, current_time, step_size, fmi2_true);
    return status == fmi2_status_ok;
}

bool fmi2_slave::terminate()
{
    assert(simStarted_);
    auto status = fmi2_import_terminate(handle_);
    simStarted_ = false;
    return status == fmi2_status_ok;
}

bool fmi2_slave::get_integer(const std::vector<value_ref>& vr, std::vector<int>& values)
{
    auto status = fmi2_import_get_integer(handle_, vr.data(), vr.size(), values.data());
    return status == fmi2_status_ok;
}

bool fmi2_slave::get_real(const std::vector<value_ref>& vr, std::vector<double>& values)
{
    auto status = fmi2_import_get_real(handle_, vr.data(), vr.size(), values.data());
    return status == fmi2_status_ok;
}

bool fmi2_slave::get_string(const std::vector<value_ref>& vr, std::vector<std::string>& values)
{
    auto tmp = std::vector<fmi2_string_t>(vr.size());
    auto status = fmi2_import_get_string(handle_, vr.data(), vr.size(), tmp.data());
    for (auto i = 0; i < tmp.size(); i++) {
        values[i] = tmp[i];
    }
    return status == fmi2_status_ok;
}

bool fmi2_slave::get_boolean(const std::vector<value_ref>& vr, std::vector<bool>& values)
{
    auto tmp = std::vector<fmi2_boolean_t>(vr.size());
    auto status = fmi2_import_get_boolean(handle_, vr.data(), vr.size(), tmp.data());
    for (auto i = 0; i < tmp.size(); i++) {
        values[i] = tmp[i] != 0;
    }
    return status == fmi2_status_ok;
}

bool fmi2_slave::set_integer(const std::vector<value_ref>& vr, const std::vector<int>& values)
{
    auto status = fmi2_import_set_integer(handle_, vr.data(), vr.size(), values.data());
    return status == fmi2_status_ok;
}

bool fmi2_slave::set_real(const std::vector<value_ref>& vr, const std::vector<double>& values)
{
    auto status = fmi2_import_set_real(handle_, vr.data(), vr.size(), values.data());
    return status == fmi2_status_ok;
}

bool fmi2_slave::set_string(const std::vector<value_ref>& vr, const std::vector<std::string>& values)
{
    std::vector<fmi2_string_t> _values(vr.size());
    for (auto i = 0; i < vr.size(); i++) {
        _values[i] = values[i].c_str();
    }
    auto status = fmi2_import_set_string(handle_, vr.data(), vr.size(), _values.data());
    return status == fmi2_status_ok;
}

bool fmi2_slave::set_boolean(const std::vector<value_ref>& vr, const std::vector<bool>& values)
{
    std::vector<fmi2_boolean_t> _values(vr.size());
    for (auto i = 0; i < vr.size(); i++) {
        _values[i] = values[i] ? fmi2_true : fmi2_false;
    }
    auto status = fmi2_import_set_boolean(handle_, vr.data(), vr.size(), _values.data());
    return status == fmi2_status_ok;
}

void fmi2_slave::freeInstance()
{
    if (!freed) {
        freed = true;
        fmi2_import_free_instance(handle_);
        fmi2_import_destroy_dllfmu(handle_);
        fmi2_import_free(handle_);
    }
}

fmi2_slave::~fmi2_slave()
{
    fmi2_slave::freeInstance();
}

void fmi2_slave::copy_current_state(saved_state& state)
{
    if (!fmi2_import_get_capability(handle_, fmi2_cs_canGetAndSetFMUstate)) {
        throw std::runtime_error(instanceName_ + ": FMU does not support state saving");
    }
    const auto status = fmi2_import_get_fmu_state(handle_, &state.fmuState);

    if (status != fmi2_status_ok && status != fmi2_status_warning) {
        throw std::runtime_error(std::string("failed to get FMU state Error: ") + fmi2_import_get_last_error(handle_));
    }
    state.setupComplete = setupComplete_;
    state.simStarted = simStarted_;
}

state_index fmi2_slave::store_new_state(saved_state state)
{
    if (savedStatesFreelist_.empty()) {
        savedStates_.push_back(std::move(state));
        return static_cast<state_index>(savedStates_.size() - 1);
    } else {
        const auto stateIndex = savedStatesFreelist_.front();
        savedStatesFreelist_.pop();
        savedStates_.at(stateIndex) = std::move(state);
        return stateIndex;
    }
}

state_index fmi2_slave::save_state()
{
    saved_state currentState;
    copy_current_state(currentState);
    return store_new_state(std::move(currentState));
}

void fmi2_slave::save_state(state_index stateIndex)
{
    copy_current_state(savedStates_.at(stateIndex));
}

void fmi2_slave::restore_state(state_index stateIndex)
{
    const auto& state  = savedStates_.at(stateIndex);
    const auto status = fmi2_import_set_fmu_state(handle_, state.fmuState);
    if (status != fmi2_status_ok && status != fmi2_status_warning) {
        throw std::runtime_error(std::string("failed to get FMU state Error: ") + fmi2_import_get_last_error(handle_));
    }
    setupComplete_ = state.setupComplete;
    simStarted_ = state.simStarted;
}

void fmi2_slave::release_state(state_index state)
{
    auto fmuState = savedStates_.at(state).fmuState;
    savedStatesFreelist_.push(state);
    const auto status = fmi2_import_free_fmu_state(handle_, &fmuState);
    if (status != fmi2_status_ok && status != fmi2_status_warning) {
        throw std::runtime_error(std::string("failed to free FMU state Error: ") + fmi2_import_get_last_error(handle_));
    }

}

void fmi2_slave::export_state(state_index stateIndex, proxyfmu::thrift::ExportedState& es) const
{
    const auto& savedState = savedStates_.at(stateIndex);

    // Check that the FMU supports state serialization
    if (!fmi2_import_get_capability(handle_, fmi2_cs_canSerializeFMUstate)) {
        throw std::runtime_error(instanceName_ + ": FMU does not support state saving");
    }

    // Get size of serialized FMU state
    std::size_t fmuStateSize = 0;
    const auto sizeStatus = fmi2_import_serialized_fmu_state_size(
        handle_, savedState.fmuState, &fmuStateSize);
    if (sizeStatus != fmi2_status_ok && sizeStatus != fmi2_status_warning) {
        throw std::runtime_error(std::string("failed to get size of serialized FMU state Error: ") + fmi2_import_get_last_error(handle_));
    }

    // Serialize FMU state
    es.fmuState = std::string(fmuStateSize, '\0');
    const auto status = fmi2_import_serialize_fmu_state(
        handle_, savedState.fmuState,
        reinterpret_cast<fmi2_byte_t*>(es.fmuState.data()), fmuStateSize);
    if (status != fmi2_status_ok && status != fmi2_status_warning) {
        throw std::runtime_error(std::string("failed to serialize FMU state Error: ") + fmi2_import_get_last_error(handle_));
    }

    es.schemeVersion = export_scheme_version;
    es.uuid = md_.guid;
    es.setupComplete = savedState.setupComplete;
    es.simStarted = savedState.simStarted;
}

state_index fmi2_slave::import_state(const proxyfmu::thrift::ExportedState& exportedState)
{
    saved_state savedState;
    // First some sanity checks
    const auto schemeVersion = exportedState.schemeVersion;
    if (schemeVersion != export_scheme_version) {
        throw std::runtime_error("The serialized state of subsimulator '" + instanceName_ + "' uses an incompatible scheme");
    }
    const auto fmuUUID = exportedState.uuid;
    if (fmuUUID != md_.guid) {
        throw std::runtime_error("The serialized state of subsimulator '" + instanceName_ + "' was created with a different FMU");
    }
    if (!fmi2_import_get_capability(handle_, fmi2_cs_canSerializeFMUstate)) {
        throw std::runtime_error(instanceName_ + ": FMU does not support state deserialization");
    }

    // Deserialize FMU state
    const auto& serializedFMUState = exportedState.fmuState;
    const auto status = fmi2_import_de_serialize_fmu_state(
        handle_,
        reinterpret_cast<const fmi2_byte_t*>(serializedFMUState.data()),
        serializedFMUState.size(),
        &savedState.fmuState);
    if (status != fmi2_status_ok && status != fmi2_status_warning) {
        throw std::runtime_error(fmi2_import_get_last_error(handle_));
    }

    // Get other data
    savedState.setupComplete = exportedState.setupComplete;
    savedState.simStarted = exportedState.simStarted;

    return store_new_state(std::move(savedState));
}

} // namespace proxyfmu::fmi