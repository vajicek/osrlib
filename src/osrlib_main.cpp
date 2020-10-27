#include "osrlib.h"
#include "osrlib_test.h"
#include "osrlib_io.h"
#include "osrlib_rendering.h"

#include "yaml-cpp/yaml.h"

#include <iostream>
#include <chrono>

void interactiveTest() {
    Mesh mesh;
    loadObj(&mesh, "../testdata/bunny.obj");
    mesh.computeNormals();

    std::vector<RenderNode> nodes {{&mesh, std::nullopt, {0, 1, 0, 0}, {0, -0.1, -0.02}}};
    auto start = std::chrono::system_clock::now();

    renderToOpenGLWindowLoop({{1 * 512, 1 * 512}},
        [&nodes](const Rendering &rendering) {
            renderNodes(rendering, nodes);
        },
        [&nodes, &start]() {
            auto now = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = now - start;
            for (auto &render_node: nodes) {
                render_node.angle_axis.w = elapsed_seconds.count() * 20;
            }
        });
}

void renderTest() {
    Mesh mesh;
    loadObj(&mesh, "../testdata/bunny.obj");
    mesh.computeNormals();

    renderToTextureBufferPass({{2 * 512, 2 * 512}}, "bunny.ppm",
        [&mesh](const Rendering &rendering){ renderMesh(rendering, mesh); });
}

void renderYaml(const std::string yaml_file, const std::string output_file) {
    YAML::Node scene = YAML::LoadFile(yaml_file);
    std::map<std::string, Mesh> meshMap;
    for (auto mesh_node: scene["meshes"]) {
        auto mesh_id = mesh_node["id"].as<std::string>();
        auto file = mesh_node["file"].as<std::string>();
        loadObj(&meshMap[mesh_id], file);
        meshMap[mesh_id].computeNormals();
    }
    std::vector<RenderNode> render_nodes;
    for (auto yaml_render_node: scene["nodes"]) {
        auto mesh_id = yaml_render_node["mesh"].as<std::string>();
        auto tr = yaml_render_node["translation"];
        RenderNode render_node {&meshMap[mesh_id],
            std::nullopt,
            {0, 1, 0, 0},
            {tr[0].as<double>(), tr[1].as<double>(), tr[2].as<double>()}};
        render_nodes.push_back(render_node);
    }

    renderToTextureBufferPass({{2 * 512, 2 * 512}}, output_file,
        [&render_nodes](const Rendering &rendering){ renderNodes(rendering, render_nodes); });
}

int main(void) {
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

    renderYaml("../testdata/scene.yaml", "bunny.ppm");

    return 0;
}
