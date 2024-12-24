#pragma once

#include "Update.hpp"
#include "Window.hpp"
#include "World.hpp"

// ヘッダオンリーでお手軽に。virtual関数をヘッダ内で実装するときの警告抑制
#pragma clang diagnostic ignored "-Wweak-vtables"

// // 常に画面の左上にある点
// class StickyPointTopLeft : public Update {
//     void update() override {
//         auto vp = this->get_world().viewport_provider();
//         auto ms = this->get_world().window.gui.master_scale();
//         const auto width = float(vp.width) * ms.x;
//         const auto height = float(vp.height) * ms.y;

//         set_position({-width / 2, height / 2, 0});
//     }
// };

// キー操作が可能な正投影カメラを作成する。
inline Camera &mobile_ortho_camera(WorldObject &parent, Window &window) {
    auto &obj = parent.append_child<WorldObject>();
    auto &camera = obj.add_component<Camera>(window, Camera::Orthographic);

    obj.add_component<UpdateComponent>([&](UpdateComponent & /*self*/) {
        constexpr float speed = 0.002;

        if (window.key(GLFW_KEY_RIGHT)) {
            obj.position += obj.get_left() * speed;
        }
        if (window.key(GLFW_KEY_LEFT)) {
            obj.position += obj.get_right() * speed;
        }
        if (window.key(GLFW_KEY_DOWN)) {
            obj.position += obj.get_down() * speed;
        }
        if (window.key(GLFW_KEY_UP)) {
            obj.position += obj.get_up() * speed;
        }
        if (window.key(GLFW_KEY_Z)) {
            obj.scale /= 1.01F;
        }
        if (window.key(GLFW_KEY_X)) {
            obj.scale *= 1.01F;
        }
        if (window.key(GLFW_KEY_Q)) {
            window.close();
        }
    });
    return camera;
}

// キー操作およびマウス操作が可能な透視投影カメラを作成する。
inline Camera &mobile_normal_camera(WorldObject &parent, Window &window) { // NOLINT(readability-function-cognitive-complexity)
    auto &body = parent.append_child<WorldObject>();
    auto &head = body.append_child<WorldObject>();
    auto &camera = head.add_component<Camera>(window);

    constexpr float speed = 0.003;
    constexpr float angle_speed = 0.02;
    body.add_component<UpdateComponent>([&](UpdateComponent & /*self*/) {
        if (window.key(GLFW_KEY_W)) {
            body.position += body.get_front() * speed;
        }
        if (window.key(GLFW_KEY_A)) {
            body.position += body.get_left() * speed;
        }
        if (window.key(GLFW_KEY_S)) {
            body.position += body.get_back() * speed;
        }
        if (window.key(GLFW_KEY_D)) {
            body.position += body.get_right() * speed;
        }
        if (window.key(GLFW_KEY_SPACE)) {
            body.position += body.get_up() * speed;
        }
        if (window.key(GLFW_KEY_LEFT_SHIFT)) {
            body.position += body.get_down() * speed;
        }
        if (window.key(GLFW_KEY_RIGHT)) {
            body.rotate *= ANGLE_Y(-angle_speed);
        }
        if (window.key(GLFW_KEY_LEFT)) {
            body.rotate *= ANGLE_Y(angle_speed);
        }
        if (window.key(GLFW_KEY_DOWN)) {
            head.rotate *= ANGLE_X(angle_speed);
        }
        if (window.key(GLFW_KEY_UP)) {
            head.rotate *= ANGLE_X(-angle_speed);
        }
        if (window.key(GLFW_KEY_Z)) {
            // 移動速度が変わる
            body.scale *= 1.01;
        }
        if (window.key(GLFW_KEY_X)) {
            body.scale /= 1.01;
        }
        if (window.key(GLFW_KEY_Q)) {
            window.close();
        }

        auto [dx, dy] = window.cursor_diff();
        body.rotate *= ANGLE_Y(-angle_speed * float(dx) * 0.1f);
        head.rotate *= ANGLE_X(angle_speed * float(dy) * 0.1f);
    });
    return camera;
}

inline MeshComponent &new_mesh(WorldObject &parent, Material *material = nullptr) {
    auto &gui = parent.get_world().gui;
    auto &mesh = gui.append_resource<Mesh>();
    mesh.use_index = true;
    auto &obj = parent.child_component<MeshComponent>(mesh, material);
    return obj;
}

inline Mesh &new_mesh(GUI &gui, GLenum draw_mode = GL_TRIANGLE_STRIP, const vector<glm::vec3> &coords = {}, const vector<RGBA> &colors = {}, const vector<glm::vec2> &uvs = {}) {
    auto &mesh = gui.append_resource<Mesh>(draw_mode, coords, colors, uvs);
    return mesh;
}

inline MeshComponent &new_points(WorldObject &parent, Material *material = nullptr) {
    auto &gui = parent.get_world().gui;
    auto &mesh = gui.append_resource<Mesh>();
    auto &obj = parent.child_component<MeshComponent>(mesh, material);
    mesh.draw_mode = GL_POINTS;
    return obj;
}

inline MeshComponent &new_line(WorldObject &parent, Material *material = nullptr) {
    auto &gui = parent.get_world().gui;
    auto &mesh = gui.append_resource<Mesh>();
    auto &obj = parent.child_component<MeshComponent>(mesh, material);
    mesh.draw_mode = GL_LINE_STRIP;
    return obj;
}

// xz平面に[-10, 10]の大きさのグリッドを作成する
class GridGround : public WorldObject {
  public:
    GridGround() {
        auto &material = MaterialBuilder().line_width(1).build(this->get_world().gui);
        auto &grid = new_line(*this, &material);
        for (int i = -10; i <= 10; i++) {
            constexpr RGBA grid_color = {0.1, 0.1, 0.1, 1};
            grid.mesh.vertices.push_back(InterleavedVertexInfo{{i, 0, -10}, grid_color});
            grid.mesh.vertices.push_back(InterleavedVertexInfo{{i, 0, 10}, grid_color});
            grid.mesh.vertices.push_back(InterleavedVertexInfo{{-10, 0, i}, grid_color});
            grid.mesh.vertices.push_back(InterleavedVertexInfo{{10, 0, i}, grid_color});
        }
        grid.mesh.draw_mode = GL_LINES;
        grid.owner().scale = 1;
    }
};
