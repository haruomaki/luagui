#pragma once

#include <utility.hpp>

inline MeshObject &new_rect(WorldObject &parent, const std::vector<glm::vec3> &coords, const glm::vec4 &base_color) {
    auto &window = parent.get_world().window;
    auto &mesh = window.append_resource<StaticMesh>();
    auto &material = MaterialBuilder().base_color(base_color).build(window);
    auto &obj = parent.append_child<MeshObject>(mesh, &material);

    mesh.vertices.coords = coords;
    mesh.vertices.colors = {{1, 1, 0, 1}};
    mesh.sync_vram();
    mesh.draw_mode = GL_TRIANGLE_FAN;
    mesh.use_index = false;
    return obj;
}
