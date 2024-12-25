#pragma once

#include <GUI/utility.hpp>

inline MeshComponent &new_rect(WorldObject &parent, const std::vector<glm::vec3> &coords, Material &material) {
    auto &gui = parent.get_world().gui;
    auto &mesh = gui.resources.append<StaticMesh>();
    auto &obj = parent.child_component<MeshComponent>(mesh, &material);

    mesh.vertices.coords = coords;
    mesh.vertices.colors = {{1, 1, 0, 1}};
    mesh.sync_vram();
    mesh.draw_mode = GL_TRIANGLE_FAN;
    mesh.use_index = false;
    return obj;
}
