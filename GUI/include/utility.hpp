#include <Camera.hpp>
#include <Update.hpp>
#include <Viewport.hpp>

// ヘッダオンリーでお手軽に。virtual関数をヘッダ内で実装するときの警告抑制
#pragma clang diagnostic ignored "-Wweak-vtables"

// 常に画面の左上にある点
class StickyPointTopLeft : public Update {
    const Viewport &viewport_;

    void update() override {
        const auto size = viewport_.get_size();
        const auto width = float(size.x);
        const auto height = float(size.y);

        set_position({-width / 2, height / 2, 0});
    }

  public:
    StickyPointTopLeft(const Viewport &viewport)
        : viewport_(viewport) {}
};

// glm::vec3 operator*(const glm::vec3 &v, const float a) {
//     return v * a;
// }

class MobileOrthoCamera : public OrthoCamera, protected Update {
    void update() override {
        constexpr float speed = 5;
        const Window &window = this->get_world().window;

        if (window.get_key(GLFW_KEY_RIGHT)) {
            position += get_left() * speed;
        }
        if (window.get_key(GLFW_KEY_LEFT)) {
            position += get_right() * speed;
        }
        if (window.get_key(GLFW_KEY_DOWN)) {
            position += get_down() * speed;
        }
        if (window.get_key(GLFW_KEY_UP)) {
            position += get_up() * speed;
        }
        if (window.get_key(GLFW_KEY_Z)) {
            scale /= 1.01F;
        }
        if (window.get_key(GLFW_KEY_X)) {
            scale *= 1.01F;
        }
        if (window.get_key(GLFW_KEY_Q)) {
            window.close();
        }

        // scale *= 1.002F;
        // scale *= "a";
        // scale + "a";
    }

  public:
    MobileOrthoCamera(const Viewport &viewport)
        : OrthoCamera(viewport) {}
};

class MobileNormalCamera : public Camera, protected Update {
    NormalCamera &camera_head_;

    void update() override {
        const Window &window = this->get_world().window;

        if (window.get_key(GLFW_KEY_W)) {
            position += get_front() * speed;
        }
        if (window.get_key(GLFW_KEY_A)) {
            position += get_left() * speed;
        }
        if (window.get_key(GLFW_KEY_S)) {
            position += get_back() * speed;
        }
        if (window.get_key(GLFW_KEY_D)) {
            position += get_right() * speed;
        }
        if (window.get_key(GLFW_KEY_SPACE)) {
            position += get_up() * speed;
        }
        if (window.get_key(GLFW_KEY_LEFT_SHIFT)) {
            position += get_down() * speed;
        }
        if (window.get_key(GLFW_KEY_RIGHT)) {
            rotate *= ANGLE_Y(-angle_speed);
        }
        if (window.get_key(GLFW_KEY_LEFT)) {
            rotate *= ANGLE_Y(angle_speed);
        }
        if (window.get_key(GLFW_KEY_DOWN)) {
            camera_head_.rotate *= ANGLE_X(angle_speed);
        }
        if (window.get_key(GLFW_KEY_UP)) {
            camera_head_.rotate *= ANGLE_X(-angle_speed);
        }
        if (window.get_key(GLFW_KEY_Z)) {
            // 移動速度が変わる
            scale /= 1.01;
        }
        if (window.get_key(GLFW_KEY_X)) {
            scale *= 1.01;
        }
        if (window.get_key(GLFW_KEY_Q)) {
            window.close();
        }

        auto [new_x, new_y] = window.get_cursor_pos();
        auto dx = new_x - this->cursor_pos.first;
        auto dy = new_y - this->cursor_pos.second;
        rotate *= ANGLE_Y(-angle_speed * float(dx) * 0.1f);
        camera_head_.rotate *= ANGLE_X(angle_speed * float(dy) * 0.1f);
        this->cursor_pos = {new_x, new_y};
    }

  public:
    float speed = 0.1;
    float angle_speed = 0.02;
    pair<double, double> cursor_pos = this->get_world().window.get_cursor_pos();

    MobileNormalCamera(const Viewport &viewport)
        : camera_head_(this->append_child<NormalCamera>(viewport)) {}

    [[nodiscard]] glm::mat4 get_view_matrix() const override {
        return camera_head_.get_view_matrix();
    }

    [[nodiscard]] glm::mat4 get_projection_matrix() const override {
        return camera_head_.get_projection_matrix();
    }
};
