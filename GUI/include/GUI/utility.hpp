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
inline Camera &mobile_ortho_camera(WorldObject &parent, ResourceHandle<Window> rh) {
    auto &obj = parent.append_child<WorldObject>();
    auto &camera = obj.add_component<Camera>(rh, Camera::Orthographic);

    obj.add_component<UpdateComponent>([&, rh](UpdateComponent & /*self*/) {
        auto &window = rh.get();
        const float speed = 0.12 / window.gui().refresh_rate();
        const float zoom_speed = 1 + 0.6 / window.gui().refresh_rate();

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
            obj.scale /= zoom_speed;
        }
        if (window.key(GLFW_KEY_X)) {
            obj.scale *= zoom_speed;
        }
        if (window.key(GLFW_KEY_Q)) {
            window.close();
        }
    });
    return camera;
}

// キー操作およびマウス操作が可能な透視投影カメラを作成する。
inline Camera &mobile_normal_camera(WorldObject &parent, ResourceHandle<Window> rh) { // NOLINT(readability-function-cognitive-complexity)
    auto &body = parent.append_child<WorldObject>();
    auto &head = body.append_child<WorldObject>();
    auto &camera = head.add_component<Camera>(rh);

    body.add_component<UpdateComponent>([&, rh](UpdateComponent & /*self*/) {
        auto &window = rh.get();
        const int rr = window.gui().refresh_rate();
        const float speed = 0.3 / rr;
        const float angle_speed = 0.8 / rr;
        const float zoom_speed = 1 + 1.0 / rr;

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

inline ResourceHandle<Window> create_window(GUI &gui, int width, int height, const std::string &title) {
    auto h = gui.resources.append<Window>(gui, width, height, title.c_str());
    return h;
}
