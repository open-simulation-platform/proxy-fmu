
#include <proxyfmu/lib_info.hpp>
#include <proxyfmu/server/boot_service_handler.hpp>

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
              << "Press any key to quit...\n";
    while (std::cin.get() != '\n') { };
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

int printHelp()
{
    // clang-format off
    std::cout << "Usage: proxyfmu [-v] port" << '\n'
              << "  " << "-v, --version" << "    " << "Print version" << std::endl;
    // clang-format on
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
    if (argc == 1) {
        return printHelp();
    }

    std::string cmd = argv[1];
    if (cmd == "-v" || cmd == "--version") {
        return printVersion();
    }

    try {

        int port = std::stoi(argv[1]);
        return run_application(port);

    } catch (std::exception& e) {
        std::cerr << "Unhandled Exception reached the top of main: " << e.what() << ", application will now exit" << std::endl;
        return UNHANDLED_ERROR;
    }
}
