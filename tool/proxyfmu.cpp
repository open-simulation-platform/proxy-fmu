
#include <proxyfmu/fixed_range_random_generator.hpp>
#include <proxyfmu/lib_info.hpp>
#include <proxyfmu/server/fmu_service_handler.hpp>

#include <boost/program_options.hpp>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include <functional>
#include <iostream>
#include <utility>

using namespace proxyfmu::thrift;
using namespace proxyfmu::server;

using namespace ::apache::thrift;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

namespace
{

class ServerReadyEventHandler : public TServerEventHandler
{

private:
    std::function<void()> callback_;

public:
    explicit ServerReadyEventHandler(std::function<void()> callback)
        : callback_(std::move(callback))
    { }

    void preServe() override
    {
        callback_();
    }
};

const int port_range_min = 49152;
const int port_range_max = 65535;

const int max_port_retries = 10;

const int SUCCESS = 0;
const int COMMANDLINE_ERROR = 1;
const int UNHANDLED_ERROR = 2;

int run_application(const std::string& fmu, const std::string& instanceName)
{
    std::unique_ptr<TSimpleServer> server;
    auto stop = [&]() {
        server->stop();
    };
    std::shared_ptr<fmu_service_handler> handler(new fmu_service_handler(fmu, instanceName, stop));
    std::shared_ptr<TProcessor> processor(new FmuServiceProcessor(handler));

    std::shared_ptr<TTransportFactory> transportFactory(new TFramedTransportFactory());
    std::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

    proxyfmu::fixed_range_random_generator rng(port_range_min, port_range_max);

    int port;
    int final_port = -1;
    for (auto i = 0; i < max_port_retries; i++) {
        port = rng.next();

        std::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
        server = std::make_unique<TSimpleServer>(processor, serverTransport, transportFactory, protocolFactory);
        server->setServerEventHandler(std::make_shared<ServerReadyEventHandler>([port, &final_port] {
            final_port = port;
            std::cout << "[proxyfmu] port=" << std::to_string(final_port) << std::endl;
        }));

        try {

            server->serve();
            break;

        } catch (TTransportException& ex) {
            std::cout << "[proxyfmu] " << ex.what()
                      << ". Failed to bind to port " << std::to_string(port)
                      << ". Retrying with another one. Attempt " << std::to_string(i + 1)
                      << " of " << std::to_string(max_port_retries) << ".." << std::endl;
        }
    }

    return final_port != -1 ? 0 : -999;
}

int printHelp(boost::program_options::options_description& desc)
{
    std::cout << "proxyfmu" << '\n'
              << desc << std::endl;
    return SUCCESS;
}

int printVersion()
{
    const auto v = proxyfmu::library_version();
    std::cout << v.major << "." << v.minor << "." << v.patch;
    return SUCCESS;
}

} // namespace

int main(int argc, char** argv)
{

    namespace po = boost::program_options;

    po::options_description desc("Options");
    desc.add_options()("help,h", "Print this help message and quits.");
    desc.add_options()("version,v", "Print program version.");
    desc.add_options()("fmu", po::value<std::string>(), "Location of the fmu to load.");
    desc.add_options()("instanceName", po::value<std::string>(), "Name of the slave instance.");

    if (argc == 1) {
        return printHelp(desc);
    }

    try {

        po::variables_map vm;
        try {

            po::store(po::parse_command_line(argc, argv, desc), vm);

            if (vm.count("help")) {
                return printHelp(desc);
            } else if (vm.count("version")) {
               return printVersion();
            }

            po::notify(vm);

        } catch (po::error& e) {
            std::cerr << "ERROR: " << e.what() << std::endl
                      << std::endl;
            std::cerr << desc << std::endl;
            return COMMANDLINE_ERROR;
        }

        const auto fmu = vm["fmu"].as<std::string>();
        const auto fmuPath = proxyfmu::filesystem::path(fmu);
        if (!proxyfmu::filesystem::exists(fmuPath)) {
            std::cerr << "[proxyfmu] No such file " << proxyfmu::filesystem::absolute(fmuPath);
            return COMMANDLINE_ERROR;
        }

        const auto instanceName = vm["instanceName"].as<std::string>();

        return run_application(fmu, instanceName);

    } catch (std::exception& e) {
        std::cerr << "[proxyfmu] Unhandled Exception reached the top of main: " << e.what() << ", application will now exit" << std::endl;
        return UNHANDLED_ERROR;
    }
}
