#include "rendering.h"

#include <GL/glew.h>

#include <limits>
#include <algorithm>
#include <iostream>

static GLuint createIndicesBuffer(const Mesh &mesh) {
    GLuint element_buffer_id;
    glGenBuffers(1, &element_buffer_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        mesh.face_vertices.size() * 3 * sizeof(unsigned int),
        mesh.face_vertices.data(),
        GL_STATIC_DRAW);
    return element_buffer_id;
}

static void drawMesh(const Mesh &mesh, GLuint element_buffer_id) {
    glVertexPointer(3, GL_FLOAT, 0, mesh.vertices.data());
    glEnableClientState(GL_VERTEX_ARRAY);

    glNormalPointer(GL_FLOAT, 0, mesh.normals.data());
    glEnableClientState(GL_NORMAL_ARRAY);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_id);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLES,
        mesh.face_vertices.size() * 3,
        GL_UNSIGNED_INT,
        (void*)0);
}

static void setupLighting() {
    glEnable(GL_LIGHTING);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    glm::fvec4 ambient(0.5f, 0.5f, 0.5f, 1.0f);
    glm::fvec4 diffuse(1.0f, 1.0f, 1.0f, 1.0f);
    glm::fvec4 position(0.0f, 0.0f, 0.0f, 1.0f);

    glLightfv(GL_LIGHT1, GL_AMBIENT, &ambient[0]);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, &diffuse[0]);
    glLightfv(GL_LIGHT1, GL_POSITION, &position[0]);
    glEnable(GL_LIGHT1);
}

static void centerMesh(const Mesh &mesh) {
    auto [min, max] = mesh.getExtents();
    auto translate = 0.5f * (min - max) - min;
    glTranslatef(translate.x, translate.y, translate.z);
}

void renderMesh(const Rendering &rendering, const Mesh &mesh) {
    View view {{0, 0}, {rendering.width(), rendering.height()},
        {-0.2, 0.2}, {-0.2, 0.2},
        0.2, 100000.0};
    Camera camera {{0, 0, 0.3},
        {0, 0, 0},
        {0, -1, 0}};
    setupRasterization();
    setupProjection(view, camera);
    setupLighting();
    centerMesh(mesh);
    drawMesh(mesh, createIndicesBuffer(mesh));
}

static void initNodes(const std::vector<RenderNode> &render_nodes) {
    for (auto &render_node : render_nodes) {
        if (render_node.elementBufferId.has_value()) {
            continue;
        }
        auto element_buffer_id = std::optional(createIndicesBuffer(*render_node.mesh));
        const_cast<RenderNode&>(render_node).elementBufferId = element_buffer_id;
    }
}

static void drawNode(const RenderNode &render_node) {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    auto &angle_axis = render_node.angle_axis;
    auto &position = render_node.position;

    glTranslatef(position.x, position.y, position.z);
    glRotatef(angle_axis.w, angle_axis.x, angle_axis.y, angle_axis.z);
    drawMesh(*render_node.mesh, render_node.elementBufferId.value());

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

static void drawNodes(const Rendering &rendering, const std::vector<RenderNode> &render_nodes) {
    for (auto render_node : render_nodes) {
        drawNode(render_node);
    }
}

void renderNodes(const Rendering &rendering, const std::vector<RenderNode> &render_nodes) {
    View view {{0, 0}, {rendering.width(), rendering.height()},
        {-0.2, 0.2}, {-0.2, 0.2},
        0.2, 100000.0};
    Camera camera {rendering.camera_position,
        rendering.camera_lookat,
        {0, -1, 0}};
    setupRasterization();
    setupProjection(view, camera);
    setupLighting();
    initNodes(render_nodes);
    drawNodes(rendering, render_nodes);
}
