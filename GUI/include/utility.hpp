#include <Camera.hpp>
#include <Update.hpp>
#include <Viewport.hpp>

// ヘッダオンリーでお手軽に。virtual関数をヘッダ内で実装するときの警告抑制
#pragma clang diagnostic ignored "-Wweak-vtables"

// 常に画面の左上にある点
class StickyPointTopLeft : public WorldObject, Update {
    const Viewport &viewport_;

    void update() override {
        const auto size = viewport_.get_size();
        const auto width = float(size.x);
        const auto height = float(size.y);

        setPosition({-width / 2, height / 2, 0});
    }

  public:
    StickyPointTopLeft(World &world, const Viewport &viewport)
        : WorldObject(world)
        , Update(world)
        , viewport_(viewport) {}
};

// glm::vec3 operator*(const glm::vec3 &v, const float a) {
//     return v * a;
// }

class MobileOrthoCamera : public OrthoCamera, protected Update {
    void update() override {
        constexpr float speed = 5;
        const Window &window = this->world.window;

        if (window.get_key(GLFW_KEY_RIGHT)) {
            position += getLeft() * speed;
        }
        if (window.get_key(GLFW_KEY_LEFT)) {
            position += getRight() * speed;
        }
        if (window.get_key(GLFW_KEY_DOWN)) {
            position += getDown() * speed;
        }
        if (window.get_key(GLFW_KEY_UP)) {
            position += getUp() * speed;
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
    MobileOrthoCamera(World &world, const Viewport &viewport)
        : OrthoCamera(world, viewport)
        , Update(world) {}
};

class MobileNormalCamera : public Camera, public WorldObject, protected Update {
    // class MobileNormalCameraHead : public NormalCamera {
    //     using NormalCamera::NormalCamera;
    // } camera_head_;
    NormalCamera camera_head_;
    WorldObject camera_base_;

    void update() override {
        constexpr float speed = 5;
        const Window &window = this->world.window;

        if (window.get_key(GLFW_KEY_W)) {
            position += getFront() * speed;
        }
        if (window.get_key(GLFW_KEY_A)) {
            position += getLeft() * speed;
        }
        if (window.get_key(GLFW_KEY_S)) {
            position += getBack() * speed;
        }
        if (window.get_key(GLFW_KEY_D)) {
            position += getRight() * speed;
        }
        if (window.get_key(GLFW_KEY_SPACE)) {
            position += getUp() * speed;
        }
        if (window.get_key(GLFW_KEY_LEFT_SHIFT)) {
            position += getDown() * speed;
        }
        if (window.get_key(GLFW_KEY_RIGHT)) {
            rotate *= ANGLE_Y(-0.01F);
        }
        if (window.get_key(GLFW_KEY_LEFT)) {
            rotate *= ANGLE_Y(0.01F);
        }
        if (window.get_key(GLFW_KEY_DOWN)) {
            camera_head_.rotate *= ANGLE_X(0.01F);
        }
        if (window.get_key(GLFW_KEY_UP)) {
            camera_head_.rotate *= ANGLE_X(-0.01F);
        }
        // if (window_.get_key(GLFW_KEY_Z)) {
        //     setScale(scale / 1.01);
        // }
        // if (window_.get_key(GLFW_KEY_X)) {
        //     setScale(scale * 1.01);
        // }
        if (window.get_key(GLFW_KEY_Q)) {
            window.close();
        }
    }

  public:
    MobileNormalCamera(World &world, const Viewport &viewport)
        : WorldObject(world)
        , Update(world)
        , camera_head_(world, viewport)
        , camera_base_(world) {
        this->append(camera_base_);
        camera_base_.append(camera_head_);
    }

    [[nodiscard]] glm::mat4 getViewMatrix() const override {
        return camera_head_.getViewMatrix();
    }

    [[nodiscard]] glm::mat4 getProjectionMatrix() const override {
        return camera_head_.getProjectionMatrix();
    }
};
