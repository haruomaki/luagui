#pragma once

#include <GUI/utility.hpp>

inline MeshComponent &new_rect(WorldObject &parent, const std::vector<glm::vec3> &coords, Material &material) {
    auto &window = parent.get_world().window;
    auto &mesh = window.append_resource<StaticMesh>();
    auto &obj = parent.append_child<MeshComponent>(mesh, &material);

    mesh.vertices.coords = coords;
    mesh.vertices.colors = {{1, 1, 0, 1}};
    mesh.sync_vram();
    mesh.draw_mode = GL_TRIANGLE_FAN;
    mesh.use_index = false;
    return obj;
}
