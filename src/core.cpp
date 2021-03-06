#include "core.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>
#include <tuple>

#include <boost/log/trivial.hpp>

#include "io.h"

static void logMatrix(const float* m) {
    for (int i = 0; i < 4; i ++) {
        std::stringstream ss;
        for (int j = 0; j < 4; j ++) {
            ss << m[j * 4 + i] << ", ";
        }
        BOOST_LOG_TRIVIAL(debug) << ss.str();
    }
}

static void logGlMatrices() {
    std::vector<float> m(16), p(16);
    glGetFloatv(GL_MODELVIEW_MATRIX, m.data());
    glGetFloatv(GL_PROJECTION_MATRIX, p.data());
    logMatrix(m.data());
    logMatrix(p.data());
}

void setupRasterization() {
    glEnable(GL_DEPTH_TEST);
    glDepthMask(1);
    glDepthFunc(GL_LEQUAL);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void setupProjection(const View &view, const Camera &camera) {
    glViewport(view.viewportTopLeftCorner.x, view.viewportTopLeftCorner.y,
        view.viewportBottomRightCorner.x, view.viewportBottomRightCorner.y);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(view.frustumLeftRightClippingPlane.x, view.frustumLeftRightClippingPlane.y,
        view.frustumBottomTopClippingPlane.x, view.frustumBottomTopClippingPlane.y,
        view.nearVal, view.farVal);

    logGlMatrices();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera.position.x, camera.position.y, camera.position.z,
        camera.lookAt.x, camera.lookAt.y, camera.lookAt.z,
        camera.up.x, camera.up.y, camera.up.z);
}

static GLFWwindow* initializeGlfwWindow(const Rendering &rendering, bool offscreen) {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize glfw");
    }

    if (offscreen) {
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    }

    GLFWwindow* window = glfwCreateWindow(
        rendering.resolution[0], rendering.resolution[1],
        "opengl", NULL, NULL);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create glfw window");
    }
    glfwMakeContextCurrent(window);

    return window;
}

static GLFWwindow* initializeOpenGlRuntime(const Rendering &rendering, bool offscreen) {
    GLFWwindow* window = initializeGlfwWindow(rendering, offscreen);
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Failed to initialize GLEW");
    }
    return window;
}

void renderToOpenGLWindowLoop(const Rendering &rendering,
        std::function<void(const Rendering &rendering)> renderFrameFunction,
        std::function<void()> updateFrameFunction) {
    GLFWwindow* window = initializeOpenGlRuntime(rendering, false);
    while (!glfwWindowShouldClose(window)) {
        renderFrameFunction(rendering);
        updateFrameFunction();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
}

static GLuint createFramebuffer() {
    GLuint framebufferId = 0;
    glGenFramebuffers(1, &framebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
    return framebufferId;
}

static GLuint createRenderTextureForFramebuffer(const Rendering &rendering) {
    GLuint renderedTextureId;
    glGenTextures(1, &renderedTextureId);
    glBindTexture(GL_TEXTURE_2D, renderedTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, rendering.width(), rendering.height(),
        0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTextureId, 0);
    return renderedTextureId;
}

static void createDepthBufferForFramebuffer(const Rendering &rendering) {
    GLuint depthRenderBufferId;
    glGenRenderbuffers(1, &depthRenderBufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBufferId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
        rendering.width(), rendering.height());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
        depthRenderBufferId);
}

static void enableAndValidateFramebuffer() {
    GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBuffers);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Framebuffe is invalid");
    }
}

static void readPixels(ImageBuffer *img) {
    glReadPixels(0, 0, img->width(), img->height(),
        GL_RGB, GL_UNSIGNED_BYTE,
        img->data());
}

void renderToTextureBufferPass(const Rendering &rendering,
        std::function<void(const Rendering &rendering)> renderFrameFunction,
        std::function<void(const ImageBuffer &img)> resultFunction) {
    initializeOpenGlRuntime(rendering, true);

    createFramebuffer();
    auto renderedTextureId = createRenderTextureForFramebuffer(rendering);
    createDepthBufferForFramebuffer(rendering);
    enableAndValidateFramebuffer();

    renderFrameFunction(rendering);

    resultFunction(dumpTextureToImageBuffer(renderedTextureId));

    glfwTerminate();
}

std::tuple<glm::fvec3, glm::fvec3> Mesh::getExtents() const {
    glm::fvec3 min_values(std::numeric_limits<float>::max());
    glm::fvec3 max_values(std::numeric_limits<float>::min());
    for (auto v : vertices) {
        min_values = glm::min(v, min_values);
        max_values = glm::max(v, max_values);
    }
    return std::make_tuple(min, max);
}

static glm::fvec3 triangleNormal(const glm::fvec3 &v1, const glm::fvec3 &v2, const glm::fvec3 &v3) {
    return glm::normalize(glm::cross(glm::normalize(v1 - v2), glm::normalize(v3 - v2)));
}

static glm::fvec3 triangleNormal(const std::vector<glm::fvec3> &vertices,
        const glm::u32vec3 &face_vertices) {
    return triangleNormal(vertices[face_vertices[0]],
        vertices[face_vertices[1]],
        vertices[face_vertices[2]]);
}

static std::vector<std::vector<uint32_t>> getVertexToFacesMapping(const Mesh &mesh) {
    std::vector<std::vector<uint32_t>> vertex_faces(mesh.vertices.size());
    for (auto &v : mesh.face_vertices) {
        auto face_index = &v - mesh.face_vertices.data();
        vertex_faces[v[0]].push_back(face_index);
        vertex_faces[v[1]].push_back(face_index);
        vertex_faces[v[2]].push_back(face_index);
    }
    return vertex_faces;
}

static glm::fvec3 computeVertexNormal(const Mesh &mesh,
        const std::vector<uint32_t> &incident_face_indices) {
    glm::fvec3 normal_sum(0);
    for (auto face_index : incident_face_indices) {
        auto face_vertices = mesh.face_vertices[face_index];
        normal_sum += triangleNormal(mesh.vertices, face_vertices);
    }
    return glm::normalize(normal_sum);
}

void Mesh::computeNormals() {
    std::vector<std::vector<uint32_t>> vertex_faces = getVertexToFacesMapping(*this);
    for (auto &v : vertices) {
        auto vertex_index = &v - vertices.data();
        auto incident_face_indices = vertex_faces[vertex_index];
        auto vertex_normal = computeVertexNormal(*this, incident_face_indices);
        this->normals.push_back(vertex_normal);
    }
}
