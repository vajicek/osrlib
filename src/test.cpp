#include "test.h"
#include "io.h"
#include "rendering.h"

#include <GL/glew.h>

#include <chrono>

static const GLfloat g_vertex_buffer_data[] = {
    -1.0f,-1.0f,-1.0f, // triangle 1 : begin
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, // triangle 1 : end
    1.0f, 1.0f,-1.0f, // triangle 2 : begin
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f, // triangle 2 : end
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f
};

void drawCube() {
    glColor3f(1, 1, 0);
    glTranslatef(0, 0, 0.0);

    glVertexPointer(3, GL_FLOAT, 0, g_vertex_buffer_data);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_LINE_LOOP, 0, 12 * 3);
}

void renderTest(const Rendering &rendering) {
    View view {{0, 0}, {rendering.width(), rendering.height()},
        {-1.0, 1.0}, {-1.0, 1.0},
        1.0, 100.0};
    Camera camera {{0, 0, 0},
        {0, 0, -1},
        {0, 1, 0}};
    setupRasterization();
    setupProjection(view, camera);
    drawCube();
}

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

    renderToTextureBufferPass({{2 * 512, 2 * 512}},
        [&mesh](const Rendering &rendering){ renderMesh(rendering, mesh); },
        [](const ImageBuffer &image_buffer){ dumpBufferToPpmFile(image_buffer, "bunny.ppm"); });
}
