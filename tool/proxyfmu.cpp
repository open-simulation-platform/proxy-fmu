
#include <proxyfmu/server/fmu_service_handler.hpp>

#include <boost/program_options.hpp>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include <iostream>

using namespace proxyfmu::thrift;
using namespace proxyfmu::server;

using namespace ::apache::thrift;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

namespace
{

const int SUCCESS = 0;
const int COMMANDLINE_ERROR = 1;
const int UNHANDLED_ERROR = 2;

int run_application(const std::string& fmu, const std::string& instanceName, const int port)
{
    std::unique_ptr<TSimpleServer> server;
    auto stop = [&]() {
        server->stop();
    };
    std::shared_ptr<fmu_service_handler> handler(new fmu_service_handler(fmu, instanceName, stop));
    std::shared_ptr<TProcessor> processor(new FmuServiceProcessor(handler));

    std::shared_ptr<TTransportFactory> transportFactory(new TFramedTransportFactory());
    std::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

    std::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
    server = std::make_unique<TSimpleServer>(processor, serverTransport, transportFactory, protocolFactory);
    server->serve();

    return 0;
}

int printHelp(boost::program_options::options_description& desc)
{
    std::cout << "proxyfmu" << '\n'
              << desc << std::endl;
    return SUCCESS;
}

} // namespace

int main(int argc, char** argv)
{

    namespace po = boost::program_options;

    po::options_description desc("Options");
    desc.add_options()("help,h", "Print this help message and quits.");
    desc.add_options()("port", po::value<int>(), "Specify the network port to be used.");
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
            }

            po::notify(vm);

        } catch (po::error& e) {
            std::cerr << "ERROR: " << e.what() << std::endl
                      << std::endl;
            std::cerr << desc << std::endl;
            return COMMANDLINE_ERROR;
        }

        auto port = vm["port"].as<int>();
        auto fmu = vm["fmu"].as<std::string>();
        auto instanceName = vm["instanceName"].as<std::string>();

        return run_application(fmu, instanceName, port);

    } catch (std::exception& e) {
        std::cerr << "Unhandled Exception reached the top of main: " << e.what() << ", application will now exit" << std::endl;
        return UNHANDLED_ERROR;
    }
}
