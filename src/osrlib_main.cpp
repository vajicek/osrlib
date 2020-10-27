#include "osrlib.h"
#include "osrlib_test.h"
#include "osrlib_io.h"
#include "osrlib_rendering.h"
#include "osrlib_yaml.h"

#include <boost/program_options.hpp>

#include <iostream>

namespace po = boost::program_options;

static const char* ARG_HELP = "help";
static const char* ARG_OUTPUT = "output";
static const char* ARG_YAML = "yaml";

void setupCommandLineInterface(int argc, char* argv[],
        po::options_description *options_description,
        po::variables_map *variables_map) {
    options_description->add_options()
        (ARG_HELP, "Show help")
        (ARG_OUTPUT, po::value<std::string>(), "Output filename")
        (ARG_YAML, po::value<std::string>(), "YAML file name");

    po::store(po::parse_command_line(argc, argv, *options_description), *variables_map);
    po::notify(*variables_map);
}

int interpretCommandLineInterface(po::options_description *options_description,
        po::variables_map *variables_map) {
    if (variables_map->count(ARG_HELP)) {
        std::cout << *options_description << "\n";
    } else if (variables_map->count(ARG_YAML) && variables_map->count(ARG_OUTPUT)) {
        auto yaml_filename = (*variables_map)[ARG_YAML].as<std::string>();
        auto output_filename = (*variables_map)[ARG_OUTPUT].as<std::string>();
        renderYaml(yaml_filename, output_filename);
    }
    return 0;
}

int commandLineInterface(int argc, char* argv[]) {
    po::options_description options_description;
    po::variables_map variables_map;
    setupCommandLineInterface(argc, argv, &options_description, &variables_map);
    return interpretCommandLineInterface(&options_description, &variables_map);
}

/*
int main() {
    Mesh mesh;
    loadObj(&mesh, "../testdata/ChessPawn.obj");
    mesh.computeNormals();

    renderToTextureBufferPass({{2 * 512, 2 * 512}}, "bunny.ppm",
        [&mesh](const Rendering &rendering){ renderMesh(rendering, mesh); });
    return 1;
}
*/

int main(int argc, char* argv[]) { return commandLineInterface(argc, argv); }

//renderToOpenGLWindowLoop({{1 * 512, 1 * 512}}, renderExample);
//renderToOpenGlWindowOffScreenPass(rendering, "filename.ppm", renderExample);
//renderToOpenGlWindowOffScreenPass({{1 * 512, 1 * 512}}, "filename1.ppm", renderExample);
//renderToOpenGlWindowOffScreenPass({{2 * 512, 2 * 512}}, "filename2.ppm", renderExample);
//renderToOpenGlWindowOffScreenPass({{4 * 512, 4 * 512}}, "filename4.ppm", renderExample);
//renderToOpenGlWindowOffScreenPass({{2048, 2048}}, "filename4.ppm", renderExample);
//renderToOpenGlWindowOffScreenPass({{8 * 512, 8 * 512}}, "filename8.ppm", renderExample);

//renderToOpenGLWindowLoop({{4 * 512, 4 * 512}}, RenderExample);

//renderToTextureBufferPass({{1 * 512, 1 * 512}}, "filename1.ppm", renderExample);
// renderToTextureBufferPass({{2 * 512, 2 * 512}}, "filename2.ppm", renderExample);
// renderToTextureBufferPass({{4 * 512, 4 * 512}}, "filename4.ppm", renderExample);
// renderToTextureBufferPass({{8 * 512, 8 * 512}}, "filename8.ppm", renderExample);

// Mesh mesh;
// loadObj(&mesh, "../testdata/bunny.obj");
// mesh.computeNormals();

// renderToTextureBufferPass({{2 * 512, 2 * 512}}, "bunny.ppm",
//     [&mesh](const Rendering &rendering){ renderMesh(rendering, mesh); });

// renderTest();

//renderYaml("../testdata/scene.yaml", "bunny.ppm");
