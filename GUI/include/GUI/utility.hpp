#pragma once

#include "Camera.hpp"
#include "Mesh.hpp"
#include "Update.hpp"
#include <SumiGL/Context.hpp> // TODO: px_meter()をWindowへ移行できたらこの行は消せる
#include <SumiGL/Window.hpp>

// ヘッダオンリーでお手軽に。virtual関数をヘッダ内で実装するときの警告抑制
#pragma clang diagnostic ignored "-Wweak-vtables"

// 常に画面の左上にある点
class StickyPointTopLeft : public Update {
    void update() override {
        auto vp = this->get_world().viewport_provider();
        auto ms = this->get_world().window.gui.master_scale();
        const auto width = float(vp.width) * ms.x;
        const auto height = float(vp.height) * ms.y;

        set_position({-width / 2, height / 2, 0});
    }
};

class MobileOrthoCamera : public OrthoCamera, protected Update {
    void update() override {
        constexpr float speed = 0.002;
        const Window &window = this->get_world().window;

        if (window.key(GLFW_KEY_RIGHT)) {
            position += get_left() * speed;
        }
        if (window.key(GLFW_KEY_LEFT)) {
            position += get_right() * speed;
        }
        if (window.key(GLFW_KEY_DOWN)) {
            position += get_down() * speed;
        }
        if (window.key(GLFW_KEY_UP)) {
            position += get_up() * speed;
        }
        if (window.key(GLFW_KEY_Z)) {
            scale /= 1.01F;
        }
        if (window.key(GLFW_KEY_X)) {
            scale *= 1.01F;
        }
        if (window.key(GLFW_KEY_Q)) {
            window.close();
        }

        // scale *= 1.002F;
        // scale *= "a";
        // scale + "a";
    }

  public:
    MobileOrthoCamera() = default;
};

class MobileNormalCamera : public Camera, protected Update {
    NormalCamera &camera_head_;

    void update() override {
        const Window &window = this->get_world().window;

        if (window.key(GLFW_KEY_W)) {
            position += get_front() * speed;
        }
        if (window.key(GLFW_KEY_A)) {
            position += get_left() * speed;
        }
        if (window.key(GLFW_KEY_S)) {
            position += get_back() * speed;
        }
        if (window.key(GLFW_KEY_D)) {
            position += get_right() * speed;
        }
        if (window.key(GLFW_KEY_SPACE)) {
            position += get_up() * speed;
        }
        if (window.key(GLFW_KEY_LEFT_SHIFT)) {
            position += get_down() * speed;
        }
        if (window.key(GLFW_KEY_RIGHT)) {
            rotate *= ANGLE_Y(-angle_speed);
        }
        if (window.key(GLFW_KEY_LEFT)) {
            rotate *= ANGLE_Y(angle_speed);
        }
        if (window.key(GLFW_KEY_DOWN)) {
            camera_head_.rotate *= ANGLE_X(angle_speed);
        }
        if (window.key(GLFW_KEY_UP)) {
            camera_head_.rotate *= ANGLE_X(-angle_speed);
        }
        if (window.key(GLFW_KEY_Z)) {
            // 移動速度が変わる
            scale *= 1.01;
        }
        if (window.key(GLFW_KEY_X)) {
            scale /= 1.01;
        }
        if (window.key(GLFW_KEY_Q)) {
            window.close();
        }

        auto [new_x, new_y] = window.cursor_pos();
        auto dx = new_x - this->cursor_pos.first;
        auto dy = new_y - this->cursor_pos.second;
        rotate *= ANGLE_Y(-angle_speed * float(dx) * 0.1f);
        camera_head_.rotate *= ANGLE_X(angle_speed * float(dy) * 0.1f);
        this->cursor_pos = {new_x, new_y};
    }

  public:
    float speed = 0.003;
    float angle_speed = 0.02;
    pair<double, double> cursor_pos = this->get_world().window.cursor_pos();

    MobileNormalCamera()
        : camera_head_(this->append_child<NormalCamera>()) {}

    [[nodiscard]] glm::mat4 get_view_matrix() const override {
        return camera_head_.get_view_matrix();
    }

    [[nodiscard]] glm::mat4 get_projection_matrix() const override {
        return camera_head_.get_projection_matrix();
    }
};

inline MeshObject &new_mesh(WorldObject &parent, Material *material = nullptr) {
    auto &window = parent.get_world().window;
    auto &mesh = window.append_resource<Mesh>();
    mesh.use_index = true;
    auto &obj = parent.append_child<MeshObject>(mesh, material);
    return obj;
}

inline Mesh &new_mesh(Window &window, GLenum draw_mode = GL_TRIANGLE_STRIP, const vector<glm::vec3> &coords = {}, const vector<RGBA> &colors = {}, const vector<glm::vec2> &uvs = {}) {
    auto &mesh = window.append_resource<Mesh>(draw_mode, coords, colors, uvs);
    return mesh;
}

inline MeshObject &new_points(WorldObject &parent, Material *material = nullptr) {
    auto &window = parent.get_world().window;
    auto &mesh = window.append_resource<Mesh>();
    auto &obj = parent.append_child<MeshObject>(mesh, material);
    mesh.draw_mode = GL_POINTS;
    return obj;
}

inline MeshObject &new_line(WorldObject &parent, Material *material = nullptr) {
    auto &window = parent.get_world().window;
    auto &mesh = window.append_resource<Mesh>();
    auto &obj = parent.append_child<MeshObject>(mesh, material);
    mesh.draw_mode = GL_LINE_STRIP;
    return obj;
}

// xz平面に[-10, 10]の大きさのグリッドを作成する
class GridGround : public WorldObject {
  public:
    GridGround() {
        auto &material = MaterialBuilder().line_width(1).build(this->get_world().window);
        auto &grid = new_line(*this, &material);
        for (int i = -10; i <= 10; i++) {
            constexpr RGBA grid_color = {0.1, 0.1, 0.1, 1};
            grid.mesh.vertices.push_back(InterleavedVertexInfo{{i, 0, -10}, grid_color});
            grid.mesh.vertices.push_back(InterleavedVertexInfo{{i, 0, 10}, grid_color});
            grid.mesh.vertices.push_back(InterleavedVertexInfo{{-10, 0, i}, grid_color});
            grid.mesh.vertices.push_back(InterleavedVertexInfo{{10, 0, i}, grid_color});
        }
        grid.mesh.draw_mode = GL_LINES;
        grid.scale = 1;
    }
};
