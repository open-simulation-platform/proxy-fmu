
#include "proxy_slave.hpp"

#include "../process_helper.hpp"

#include <proxyfmu/thrift/BootService.h>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include <chrono>
#include <cstdio>
#include <utility>
#include <vector>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

using namespace proxyfmu::thrift;

namespace
{

void read_data(std::string const& fileName, std::string& data)
{
    FILE* file = fopen(fileName.c_str(), "rb");
    if (file == nullptr) return;
    fseek(file, 0, SEEK_END);
    long int size = ftell(file);
    fclose(file);
    file = fopen(fileName.c_str(), "rb");

    data.resize(size);
    [[maybe_unused]] size_t bytes_read = fread(data.data(), sizeof(unsigned char), size, file);
    fclose(file);
}

} // namespace


namespace proxyfmu::client
{

const int max_retries = 600;
const int retry_interval = 500;
const int MAX_FRAME_SIZE = 50 * 1024 * 1024;

proxy_slave::proxy_slave(const filesystem::path& fmuPath, const std::string& instanceName, fmi::model_description modelDescription, const std::optional<remote_info>& remote)
    : modelDescription_(std::move(modelDescription))
{
    int port = -1;
    std::string host;

    if (!remote) {
        host = "localhost";
        std::mutex mtx;
        std::condition_variable cv;
        thread_ = std::make_unique<std::thread>(&start_process, fmuPath, instanceName, std::ref(port), std::ref(mtx), std::ref(cv));
        std::unique_lock<std::mutex> lck(mtx);
        while (port == -1) cv.wait(lck);
    } else {
        host = remote->host();
        std::shared_ptr<TTransport> socket(new TSocket(host, remote->port()));
        auto t_config = std::make_shared<TConfiguration>();
        t_config->setMaxFrameSize(MAX_FRAME_SIZE);
        auto transport = std::make_shared<TFramedTransport>(socket, t_config);
        std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        auto client = std::make_shared<BootServiceClient>(protocol);

        int retries = 0;
        while (retries < max_retries) {
            std::cout << "[proxyfmu] Proxy slave: retry loop: retries: '" << retries << "'.." << std::endl;
            try {
                transport->open();
                break;
            } catch (const TTransportException& ex) {
                if (retries++ > max_retries) {
                    throw;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(retry_interval));
            }
        }

        std::string data;
        read_data(fmuPath.string(), data);

        const std::string fmuName = proxyfmu::filesystem::path(fmuPath).stem().string();
        port = client->loadFromBinaryData(fmuName, instanceName, data);
        transport->close();
    }

    if (port == -999) {
        if (thread_) thread_->join();
        throw std::runtime_error("[proxyfmu] Unable to bind to external proxy process!");
    }

    std::shared_ptr<TTransport> socket(new TSocket(host, port));
    auto t_config = std::make_shared<TConfiguration>();
    t_config->setMaxFrameSize(MAX_FRAME_SIZE);
    transport_ = std::make_shared<TFramedTransport>(socket, t_config);
    std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport_));
    client_ = std::make_shared<FmuServiceClient>(protocol);
    transport_->open();

    client_->instantiate();
}

const fmi::model_description& proxy_slave::get_model_description() const
{
    return modelDescription_;
}

bool proxy_slave::setup_experiment(double start_time, double stop_time, double tolerance)
{
    auto status = client_->setup_experiment(start_time, stop_time, tolerance);
    return status == ::proxyfmu::thrift::Status::type::OK_STATUS;
}

bool proxy_slave::enter_initialization_mode()
{
    auto status = client_->enter_initialization_mode();
    return status == ::proxyfmu::thrift::Status::type::OK_STATUS;
}

bool proxy_slave::exit_initialization_mode()
{
    auto status = client_->exit_initialization_mode();
    return status == ::proxyfmu::thrift::Status::type::OK_STATUS;
}

bool proxy_slave::step(double current_time, double step_size)
{
    auto status = client_->step(current_time, step_size);
    return status == ::proxyfmu::thrift::Status::type::OK_STATUS;
}

bool proxy_slave::terminate()
{
    auto status = client_->terminate();
    return status == ::proxyfmu::thrift::Status::type::OK_STATUS;
}

bool proxy_slave::get_integer(const std::vector<fmi::value_ref>& vr, std::vector<int>& values)
{
    IntegerRead read;
    const ValueReferences _vr = std::vector<int64_t>(vr.begin(), vr.end());
    client_->read_integer(read, _vr);
    for (auto i = 0; i < vr.size(); i++) {
        values[i] = read.value[i];
    }
    return read.status == ::proxyfmu::thrift::Status::type::OK_STATUS;
}

bool proxy_slave::get_real(const std::vector<fmi::value_ref>& vr, std::vector<double>& values)
{
    assert(values.size() == vr.size());
    RealRead read;
    const ValueReferences _vr = std::vector<int64_t>(vr.begin(), vr.end());
    client_->read_real(read, _vr);
    for (auto i = 0; i < vr.size(); i++) {
        values[i] = read.value[i];
    }
    return read.status == ::proxyfmu::thrift::Status::type::OK_STATUS;
}

bool proxy_slave::get_string(const std::vector<fmi::value_ref>& vr, std::vector<std::string>& values)
{
    assert(values.size() == vr.size());
    StringRead read;
    const ValueReferences _vr = std::vector<int64_t>(vr.begin(), vr.end());
    client_->read_string(read, _vr);
    for (auto i = 0; i < vr.size(); i++) {
        values[i] = read.value[i];
    }
    return read.status == ::proxyfmu::thrift::Status::type::OK_STATUS;
}

bool proxy_slave::get_boolean(const std::vector<fmi::value_ref>& vr, std::vector<bool>& values)
{
    assert(values.size() == vr.size());
    BooleanRead read;
    const ValueReferences _vr = std::vector<int64_t>(vr.begin(), vr.end());
    client_->read_boolean(read, _vr);
    for (auto i = 0; i < vr.size(); i++) {
        values[i] = read.value[i];
    }
    return read.status == ::proxyfmu::thrift::Status::type::OK_STATUS;
}

bool proxy_slave::set_integer(const std::vector<fmi::value_ref>& vr, const std::vector<int>& values)
{
    assert(values.size() == vr.size());
    const ValueReferences _vr = std::vector<int64_t>(vr.begin(), vr.end());
    auto status = client_->write_integer(_vr, values);
    return status == ::proxyfmu::thrift::Status::type::OK_STATUS;
}

bool proxy_slave::set_real(const std::vector<fmi::value_ref>& vr, const std::vector<double>& values)
{
    assert(values.size() == vr.size());
    const ValueReferences _vr = std::vector<int64_t>(vr.begin(), vr.end());
    auto status = client_->write_real(_vr, values);
    return status == ::proxyfmu::thrift::Status::type::OK_STATUS;
}

bool proxy_slave::set_string(const std::vector<fmi::value_ref>& vr, const std::vector<std::string>& values)
{
    assert(values.size() == vr.size());
    const ValueReferences _vr = std::vector<int64_t>(vr.begin(), vr.end());
    auto status = client_->write_string(_vr, values);
    return status == ::proxyfmu::thrift::Status::type::OK_STATUS;
}

bool proxy_slave::set_boolean(const std::vector<fmi::value_ref>& vr, const std::vector<bool>& values)
{
    assert(values.size() == vr.size());
    const ValueReferences _vr = std::vector<int64_t>(vr.begin(), vr.end());
    auto status = client_->write_boolean(_vr, values);
    return status == ::proxyfmu::thrift::Status::type::OK_STATUS;
}

void proxy_slave::freeInstance()
{
    if (!freed) {
        freed = true;
        if (client_) client_->freeInstance();
        if (thread_ && thread_->joinable()) thread_->join();
    }
}

proxy_slave::~proxy_slave()
{
    proxy_slave::freeInstance();
}

state_index proxy_slave::save_state()
{
    return client_->save_state();
}

void proxy_slave::save_state(state_index stateIndex)
{
    client_->save_state_by_index(stateIndex);
}

void proxy_slave::restore_state(state_index stateIndex)
{
    client_->restore_state(stateIndex);
}

void proxy_slave::release_state(state_index stateIndex)
{
    client_->release_state(stateIndex);
}

void proxy_slave::export_state(state_index stateIndex, state::exported_state& es) const
{
    ExportedState es_;

    client_->export_state(es_, stateIndex);

    es.simStarted = es_.simStarted;
    es.setupComplete = es_.setupComplete;
    es.fmuState = es_.fmuState;
    es.schemeVersion = es_.schemeVersion;
    es.uuid = es_.uuid;
}

state_index proxy_slave::import_state(const state::exported_state& exportedState)
{
    ExportedState es_;
    es_.simStarted = exportedState.simStarted;
    es_.setupComplete = exportedState.setupComplete;
    es_.fmuState = exportedState.fmuState;
    es_.schemeVersion = exportedState.schemeVersion;
    es_.uuid = exportedState.uuid;

    return client_->import_state(es_);
}

} // namespace proxyfmu::client
