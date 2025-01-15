#pragma once

#include "Camera.hpp"
#include "Mesh.hpp"
#include "Update.hpp"
#include "World.hpp"
#include "WorldObject.hpp"

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
    obj.rotate = ANGLE_Y(M_PIf);
    auto &camera = obj.add_component<Camera>(&window, Camera::Orthographic);

    obj.add_component<UpdateComponent>([&obj, &camera](UpdateComponent & /*self*/) {
        if (camera.window == nullptr) return; // カメラと紐付いたウィンドウを見て、キー入力を受け取るかどうか決める。
        auto &window = *camera.window;

        const auto fps = float(window.gui().refresh_rate());
        const float speed = 0.12f / fps;
        const float zoom_speed = 1 + (0.6f / fps);

        if (window.key(GLFW_KEY_RIGHT)) {
            obj.position += obj.get_right() * speed;
        }
        if (window.key(GLFW_KEY_LEFT)) {
            obj.position += obj.get_left() * speed;
        }
        if (window.key(GLFW_KEY_DOWN)) {
            obj.position += obj.get_down() * speed;
        }
        if (window.key(GLFW_KEY_UP)) {
            obj.position += obj.get_up() * speed;
        }
        if (window.key(GLFW_KEY_Z)) {
            obj.scale /= zoom_speed;
        }
        if (window.key(GLFW_KEY_X)) {
            obj.scale *= zoom_speed;
        }
        if (window.key(GLFW_KEY_Q)) {
            window.close();
            return;
        }
    });
    return camera;
}

// キー操作およびマウス操作が可能な透視投影カメラを作成する。
inline Camera &mobile_normal_camera(WorldObject &parent, Window &window) { // NOLINT(readability-function-cognitive-complexity)
    auto &body = parent.append_child<WorldObject>();
    auto &head = body.append_child<WorldObject>();
    auto &camera = head.add_component<Camera>(&window);

    body.add_component<UpdateComponent>([&body, &head, &camera](UpdateComponent & /*self*/) {
        if (camera.window == nullptr) return; // カメラと紐付いたウィンドウがあればキー入力を受け取る。
        auto &window = *camera.window;

        const auto rr = float(window.gui().refresh_rate());
        const float speed = 0.3f / rr;
        const float angle_speed = 0.8f / rr;
        const float zoom_speed = 1 + (1.0f / rr);

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
            body.scale *= zoom_speed;
        }
        if (window.key(GLFW_KEY_X)) {
            body.scale /= zoom_speed;
        }
        if (window.key(GLFW_KEY_Q)) {
            window.close();
            return;
        }

        auto [dx, dy] = window.cursor_diff();
        body.rotate *= ANGLE_Y(-angle_speed * float(dx) * 0.2f);
        head.rotate *= ANGLE_X(angle_speed * float(dy) * 0.2f);
    });
    return camera;
}

inline MeshComponent &new_mesh(WorldObject &parent, Material *material = nullptr) {
    auto &gui = parent.get_world().gui;
    auto &mesh = gui.resources.append<Mesh>().get();
    mesh.use_index = true;
    auto &obj = parent.child_component<MeshComponent>(mesh, material);
    return obj;
}

inline Mesh &new_mesh(GUI &gui, GLenum draw_mode = GL_TRIANGLE_STRIP, const vector<glm::vec3> &coords = {}, const vector<RGBA> &colors = {}, const vector<glm::vec2> &uvs = {}) {
    auto &mesh = gui.resources.append<Mesh>(draw_mode, coords, colors, uvs).get();
    return mesh;
}

inline MeshComponent &new_points(WorldObject &parent, Material *material = nullptr) {
    auto &gui = parent.get_world().gui;
    auto &mesh = gui.resources.append<Mesh>().get();
    auto &obj = parent.child_component<MeshComponent>(mesh, material);
    mesh.draw_mode = GL_POINTS;
    return obj;
}

inline MeshComponent &new_line(WorldObject &parent, Material *material = nullptr) {
    auto &gui = parent.get_world().gui;
    auto &mesh = gui.resources.append<Mesh>().get();
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
            constexpr RGBA grid_color = {.r = 0.1, .g = 0.1, .b = 0.1, .a = 1};
            grid.mesh.vertices.push_back(InterleavedVertexInfo{.coord = {i, 0, -10}, .color = grid_color});
            grid.mesh.vertices.push_back(InterleavedVertexInfo{.coord = {i, 0, 10}, .color = grid_color});
            grid.mesh.vertices.push_back(InterleavedVertexInfo{.coord = {-10, 0, i}, .color = grid_color});
            grid.mesh.vertices.push_back(InterleavedVertexInfo{.coord = {10, 0, i}, .color = grid_color});
        }
        grid.mesh.draw_mode = GL_LINES;
        grid.owner().scale = 1;
    }
};
