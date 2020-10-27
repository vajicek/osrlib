#include "osrlib_yaml.h"
#include "osrlib_io.h"
#include "osrlib_rendering.h"

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

void renderYaml(const std::string yaml_file, const std::string output_file) {
    const YAML::Node root = YAML::LoadFile(yaml_file);
    const std::map<std::string, Mesh> mesh_map = getMeshMap(root);
    const std::vector<RenderNode> render_nodes = getRenderNodes(root, mesh_map);
    const glm::ivec2 resolution = root["rendering"]["resolution"].as<glm::ivec2>();
    const glm::fvec3 camera_position = root["camera"]["position"].as<glm::fvec3>();
    const glm::fvec3 camera_lookat = root["camera"]["lookat"].as<glm::fvec3>();
    renderToTextureBufferPass({resolution, camera_position, camera_lookat},
        output_file,
        [&render_nodes](const Rendering &rendering){ renderNodes(rendering, render_nodes); });
}
