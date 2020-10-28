#include <iostream>

#include <boost/program_options.hpp>

#include "yaml.h"

namespace po = boost::program_options;

static const char* ARG_HELP = "help";
static const char* ARG_OUTPUT = "output";
static const char* ARG_YAML = "yaml";

static void setupCommandLineInterface(int argc, char* argv[],
        po::options_description *options_description,
        po::variables_map *variables_map) {
    options_description->add_options()
        (ARG_HELP, "Show help")
        (ARG_OUTPUT, po::value<std::string>(), "Output filename")
        (ARG_YAML, po::value<std::string>(), "YAML file name");

    po::store(po::parse_command_line(argc, argv, *options_description), *variables_map);
    po::notify(*variables_map);
}

static int interpretCommandLineInterface(po::options_description *options_description,
        po::variables_map *variables_map) {
    if (variables_map->count(ARG_HELP)) {
        std::cout << *options_description << "\n";
    } else if (variables_map->count(ARG_YAML)) {
        auto yaml_filename = (*variables_map)[ARG_YAML].as<std::string>();
        if (variables_map->count(ARG_OUTPUT)) {
            auto output_filename = (*variables_map)[ARG_OUTPUT].as<std::string>();
            renderYaml(yaml_filename, output_filename);
        } else {
            viewYaml(yaml_filename);
        }
    }
    return 0;
}

static int commandLineInterface(int argc, char* argv[]) {
    po::options_description options_description;
    po::variables_map variables_map;
    setupCommandLineInterface(argc, argv, &options_description, &variables_map);
    return interpretCommandLineInterface(&options_description, &variables_map);
}

int main(int argc, char* argv[]) {
    return commandLineInterface(argc, argv);
}
