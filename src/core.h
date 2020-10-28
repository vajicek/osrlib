#ifndef CORE_H_
#define CORE_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <functional>
#include <map>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

class View {
 public:
    glm::ivec2 viewportTopLeftCorner;
    glm::ivec2 viewportBottomRightCorner;
    glm::dvec2 frustumLeftRightClippingPlane;
    glm::dvec2 frustumBottomTopClippingPlane;
    double nearVal, farVal;
};

class Camera {
 public:
    glm::dvec3 position;
    glm::dvec3 lookAt;
    glm::dvec3 up;
};

class Mesh {
 public:
    std::vector<glm::fvec3> vertices;
    std::vector<glm::fvec3> normals;
    std::vector<glm::fvec3> texcoords;
    std::vector<glm::u32vec3> face_vertices;
    std::vector<glm::u32vec3> face_normals;
    std::vector<glm::u32vec3> face_texcoords;

    std::tuple<glm::fvec3, glm::fvec3> getExtents() const;
    void computeNormals();
};

class RenderNode {
 public:
    const Mesh *mesh;
    std::optional<GLuint> elementBufferId;
    glm::fvec4 angle_axis;
    glm::fvec3 position;
};

class ImageBuffer {
    const int w, h;
    std::vector<u_int8_t> buffer;
 public:
    ImageBuffer(int _w, int _h) : w(_w), h(_h), buffer(w * h * 3) {}
    const u_int8_t *data() const { return buffer.data(); }
    u_int8_t *data() { return buffer.data(); }
    int width() const { return w; }
    int height() const { return h; }
    int size() const { return buffer.size(); }
};

class Rendering {
 public:
    glm::ivec2 resolution;
    glm::fvec3 camera_position;
    glm::fvec3 camera_lookat;
    std::vector<RenderNode> render_nodes;
    std::map<std::string, Mesh> mesh_map;
    int width() const { return resolution[0]; }
    int height() const { return resolution[1]; }
};

void setupRasterization();
void setupProjection(const View &view, const Camera &camera);
void renderToOpenGLWindowLoop(const Rendering &rendering,
        std::function<void(const Rendering &rendering)> renderFrameFunction,
        std::function<void()> updateFrameFunction);
void renderToTextureBufferPass(const Rendering &rendering,
        std::function<void(const Rendering &rendering)> renderFrameFunction,
        std::function<void(const ImageBuffer &img)> resultFunction);

#endif  // CORE_H_
