#include "osrlib.h"
#include "osrlib_test.h"
#include "osrlib_io.h"
#include "osrlib_rendering.h"

#include <iostream>
#include <chrono>

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

    Mesh mesh;
    loadObj(&mesh, "../bunny.obj");
    mesh.computeNormals();

    // renderToTextureBufferPass({{2 * 512, 2 * 512}}, "bunny.ppm",
    //     [&mesh](const Rendering &rendering){ renderMesh(rendering, mesh); });

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

    return 0;
}
