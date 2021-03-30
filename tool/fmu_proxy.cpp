
#include <fmuproxy/thrift/server/thrift_fmu_server.hpp>

#include <boost/program_options.hpp>

#include <iostream>

using fmuproxy::server::thrift_fmu_server;

namespace
{

const int SUCCESS = 0;
const int COMMANDLINE_ERROR = 1;
const int UNHANDLED_ERROR = 2;

void wait_for_input()
{
    std::cout << "Press a key to continue...\n";
    while (std::cin.get() != '\n');
}

int run_application(const std::string &fmu, const int port)
{
    auto thrift_socket_server = std::make_unique<thrift_fmu_server>(fmu, port);
    thrift_socket_server->start();

    wait_for_input();

    thrift_socket_server->stop();

    return 0;
}

} // namespace

int printHelp( boost::program_options::options_description& desc)
{
    std::cout << "FMU-proxy" << std::endl
         << desc << std::endl;
    return SUCCESS;
}

int main(int argc, char** argv)
{

    namespace po = boost::program_options;

    po::options_description desc("Options");
    desc.add_options()("help,h", "Print this help message and quits.");
    desc.add_options()("port", po::value<int>(), "Specify the network port to be used.");
    desc.add_options()("fmu", po::value<std::string>(), "Location of the fmu to load.");

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

        return run_application(fmu, port);

    } catch (std::exception& e) {
        std::cerr << "Unhandled Exception reached the top of main: " << e.what() << ", application will now exit" << std::endl;
        return UNHANDLED_ERROR;
    }

}
