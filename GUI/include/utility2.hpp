#pragma once

#include "utility.hpp"

inline MeshObject &new_rect(WorldObject &parent) {
    auto &obj = new_mesh(parent);
    auto &mesh = obj.mesh;
    // auto &material = obj.material;
    mesh.vertices.coords = {{-1, -1, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
    mesh.vertices.colors = {{1, 1, 0, 1}};
    mesh.draw_mode = GL_TRIANGLE_FAN;
    mesh.use_index = false;
    obj.position = {0, 0, -0.2};
    return obj;
}
