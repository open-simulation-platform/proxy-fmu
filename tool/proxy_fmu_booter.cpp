
#include <proxyfmu/server/boot_service_handler.hpp>

#include <boost/program_options.hpp>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include <iostream>
#include <thread>

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

void wait_for_input()
{
    std::cout << '\n'
              << "Press a key to continue...\n";
    while (std::cin.get() != '\n');
    std::cout << "Done." << std::endl;
}

int run_application(const int port)
{
    std::unique_ptr<TSimpleServer> server;
    std::shared_ptr<boot_service_handler> handler(new boot_service_handler());
    std::shared_ptr<TProcessor> processor(new BootServiceProcessor(handler));

    std::shared_ptr<TTransportFactory> transportFactory(new TFramedTransportFactory());
    std::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

    std::shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
    server = std::make_unique<TSimpleServer>(processor, serverTransport, transportFactory, protocolFactory);

    std::thread t([&server] { server->serve(); });

    wait_for_input();

    server->stop();
    t.join();

    return 0;
}

} // namespace

int printHelp(boost::program_options::options_description& desc)
{
    std::cout << "PROXY-FMU" << '\n'
              << desc << std::endl;
    return SUCCESS;
}

int main(int argc, char** argv)
{
    namespace po = boost::program_options;

    po::options_description desc("Options");
    desc.add_options()("help,h", "Print this help message and quits.");
    desc.add_options()("port", po::value<int>(), "Specify the network port to be used.");

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

        return run_application(port);

    } catch (std::exception& e) {
        std::cerr << "Unhandled Exception reached the top of main: " << e.what() << ", application will now exit" << std::endl;
        return UNHANDLED_ERROR;
    }
}
