
#include <fmuproxy/thrift/server/thrift_fmu_server.hpp>

#include <boost/program_options.hpp>

#include <iostream>
#include <memory>
#include <optional>
#include <unordered_map>

using fmuproxy::thrift::server::thrift_fmu_server;

using namespace std;

namespace
{

const int SUCCESS = 0;
const int COMMANDLINE_ERROR = 1;
const int UNHANDLED_ERROR = 2;

void wait_for_input()
{
    do {
        cout << '\n'
             << "Press a key to continue...\n";
    } while (cin.get() != '\n');
    cout << "Done." << endl;
}

int run_application(const vector<shared_ptr<fmi4cpp::fmi2::cs_fmu>>& fmus, unsigned int port)
{

    unordered_map<string, shared_ptr<fmi4cpp::fmi2::cs_fmu>> fmu_map;
    vector<string> modelDescriptions;
    for (const auto& fmu : fmus) {
        fmu_map[fmu->get_model_description()->guid] = fmu;
        modelDescriptions.push_back(fmu->get_model_description_xml());
    }

    auto thrift_socket_server = make_unique<thrift_fmu_server>(fmu_map, port);
    thrift_socket_server->start();
    std::cout << "Thrift/tcp listening for connections on port " << std::to_string(port) << std::endl;

    wait_for_input();

    thrift_socket_server->stop();

    return 0;
}

} // namespace

int printHelp( boost::program_options::options_description& desc)
{
    cout << "FMU-proxy" << endl
         << desc << endl;
    return SUCCESS;
}

int main(int argc, char** argv)
{

    namespace po = boost::program_options;

    po::options_description desc("Options");
    desc.add_options()("help,h", "Print this help message and quits.")("fmu,f", po::value<vector<string>>()->multitoken(), "Path to FMUs.");
    desc.add_options()("port", po::value<unsigned int>(), "Specify the network port to be used by the Thrift (TCP/IP) server.");

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

        vector<shared_ptr<fmi4cpp::fmi2::cs_fmu>> fmus;
        if (vm.count("fmu")) {
            const vector<string> fmu_paths = vm["fmu"].as<vector<string>>();
            for (const auto& fmu_path : fmu_paths) {
                fmus.push_back(std::move(fmi4cpp::fmi2::fmu(fmu_path).as_cs_fmu()));
            }
        }

        auto port = vm["port"].as<unsigned int>();

        return run_application(fmus, port);

    } catch (std::exception& e) {
        std::cerr << "Unhandled Exception reached the top of main: " << e.what() << ", application will now exit" << std::endl;
        return UNHANDLED_ERROR;
    }

}
