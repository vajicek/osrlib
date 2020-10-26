#include "osrlib_rendering.h"

#include <GL/glew.h>

#include <limits>
#include <algorithm>

GLuint createIndicesBuffer(const Mesh &mesh) {
    GLuint elementBufferId;
    glGenBuffers(1, &elementBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        mesh.face_vertices.size() * 3 * sizeof(unsigned int),
        mesh.face_vertices.data(),
        GL_STATIC_DRAW);
    return elementBufferId;
}

void drawMesh(const Mesh &mesh, const Rendering &rendering) {
    auto elementBufferId = createIndicesBuffer(mesh);

    glVertexPointer(3, GL_FLOAT, 0, mesh.vertices.data());
    glEnableClientState(GL_VERTEX_ARRAY);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferId);
    glDrawElements(GL_TRIANGLES,
        mesh.face_vertices.size() * 3,
        GL_UNSIGNED_INT,
        (void*)0);
}

void centerMesh(const Mesh &mesh) {
    auto [min, max] = mesh.getExtents();
    auto translate = 0.5f * (min - max) - min;
    glTranslatef(translate.x, translate.y, translate.z);
}

void renderMesh(const Rendering &rendering, const Mesh &mesh) {
    View view {{0, 0}, {rendering.width(), rendering.height()},
        {-0.2, 0.2}, {-0.2, 0.2},
        0.2, 100000.0};
    Camera camera {{0, 0, -0.3},
        {0, 0, 0},
        {0, -1, 0}};
    setupRasterization();
    setupProjection(view, camera);
    centerMesh(mesh);
    drawMesh(mesh, rendering);
}
