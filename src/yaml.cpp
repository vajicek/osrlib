#include "yaml.h"
#include "io.h"
#include "rendering.h"

#include <iostream>

#include <yaml-cpp/yaml.h>

namespace YAML {
    template<class T, int C>
    struct convert<glm::vec<C, T>> {
        static Node encode(const glm::vec<C, T>& rhs) {
            Node node;
            for (int i = 0; i < C; i++) {
                node.push_back(rhs[i]);
            }
            return node;
        }
        static bool decode(const Node& node, glm::vec<C, T>& rhs) {
            if(!node.IsSequence() || node.size() != C) {
                return false;
            }
            for (int i = 0; i < C; i++) {
                rhs[i] = node[i].as<T>();
            }
            return true;
        }
    };
}

std::map<std::string, Mesh> getMeshMap(const YAML::Node &root) {
    std::map<std::string, Mesh> mesh_map;
    for (auto mesh_node : root["meshes"]) {
        auto mesh_id = mesh_node["id"].as<std::string>();
        auto file = mesh_node["file"].as<std::string>();
        loadObj(&mesh_map[mesh_id], file);
        mesh_map[mesh_id].computeNormals();
    }
    return mesh_map;
}

std::vector<RenderNode> getRenderNodes(const YAML::Node &root, const std::map<std::string, Mesh> &mesh_map) {
    std::vector<RenderNode> render_nodes;
    for (auto yaml_render_node : root["nodes"]) {
        auto mesh_id = yaml_render_node["mesh"].as<std::string>();
        auto translation = yaml_render_node["translation"].as<glm::fvec3>();
        auto rotation = yaml_render_node["rotation"].as<glm::fvec4>();
        RenderNode render_node {&mesh_map.at(mesh_id),
            std::nullopt,
            rotation,
            translation};
        render_nodes.push_back(render_node);
    }
    return render_nodes;
}

void loadYaml(Rendering *rendering, const std::string yaml_file) {
    const YAML::Node root = YAML::LoadFile(yaml_file);
    rendering->mesh_map = getMeshMap(root);
    rendering->render_nodes = getRenderNodes(root, rendering->mesh_map);
    rendering->resolution = root["rendering"]["resolution"].as<glm::ivec2>();
    rendering->camera_position = root["rendering"]["camera"]["position"].as<glm::fvec3>();
    rendering->camera_lookat = root["rendering"]["camera"]["lookat"].as<glm::fvec3>();
}

void renderYaml(const std::string yaml_file, const std::string output_file) {
    Rendering rendering;
    loadYaml(&rendering, yaml_file);
    renderToTextureBufferPass(rendering,
        [](const Rendering &rendering){ renderNodes(rendering, rendering.render_nodes); },
        [&output_file](const ImageBuffer &image_buffer){ dumpToPpmFile(image_buffer, output_file); });
}

void viewYaml(const std::string yaml_file) {
    Rendering rendering;
    loadYaml(&rendering, yaml_file);
    renderToOpenGLWindowLoop(rendering,
        [](const Rendering &rendering) {
            renderNodes(rendering, rendering.render_nodes);
        },
        []() {
        });
}
