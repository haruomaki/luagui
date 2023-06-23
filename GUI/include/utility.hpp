#include <Camera.hpp>
#include <Update.hpp>
#include <Viewport.hpp>

// 常に画面の左上にある点
class StickyPointTopLeft : public WorldObject, Update {
    const Viewport &viewport_;

    void update() override {
        const auto size = viewport_.getSize();
        const auto width = float(size.x);
        const auto height = float(size.y);

        setPosition({-width / 2, height / 2, 0});
    }

  public:
    StickyPointTopLeft(Window &window, World &world, const Viewport &viewport)
        : WorldObject(world)
        , Update(window)
        , viewport_(viewport) {}
};

class MobileOrthoCamera : public OrthoCamera, protected Update {
    void update() override {
        constexpr float speed_base = 5;
        const float scale = getScale().x;
        const float speed = speed_base * scale;

        if (window_.getKey(GLFW_KEY_RIGHT)) {
            position += glm::vec3{speed, 0, 0};
        }
        if (window_.getKey(GLFW_KEY_LEFT)) {
            position += glm::vec3{-speed, 0, 0};
        }
        if (window_.getKey(GLFW_KEY_DOWN)) {
            position += glm::vec3{0, -speed, 0};
        }
        if (window_.getKey(GLFW_KEY_UP)) {
            position += glm::vec3{0, speed, 0};
        }
        if (window_.getKey(GLFW_KEY_Z)) {
            setScale(scale / 1.01);
        }
        if (window_.getKey(GLFW_KEY_X)) {
            setScale(scale * 1.01);
        }
        if (window_.getKey(GLFW_KEY_Q)) {
            exit(0);
        }
    }

  public:
    MobileOrthoCamera(Window &window, World &world, const Viewport &viewport)
        : OrthoCamera(world, viewport)
        , Update(window) {}
};

class MobileNormalCamera : public Camera, public WorldObject, protected Update {
    // class MobileNormalCameraHead : public NormalCamera {
    //     using NormalCamera::NormalCamera;
    // } camera_head_;
    NormalCamera camera_head_;
    WorldObject camera_base_;

    void update() override {
        constexpr float speed = 5;

        if (window_.getKey(GLFW_KEY_W)) {
            position += getFront() * speed;
        }
        if (window_.getKey(GLFW_KEY_A)) {
            position += getLeft() * speed;
        }
        if (window_.getKey(GLFW_KEY_S)) {
            position += getBack() * speed;
        }
        if (window_.getKey(GLFW_KEY_D)) {
            position += getRight() * speed;
        }
        if (window_.getKey(GLFW_KEY_SPACE)) {
            position += getUp() * speed;
        }
        if (window_.getKey(GLFW_KEY_LEFT_SHIFT)) {
            position += getDown() * speed;
        }
        if (window_.getKey(GLFW_KEY_RIGHT)) {
            rotate *= glm::angleAxis(-0.01F, glm::vec3{0, 1, 0});
        }
        if (window_.getKey(GLFW_KEY_LEFT)) {
            rotate *= glm::angleAxis(0.01F, glm::vec3{0, 1, 0});
        }
        if (window_.getKey(GLFW_KEY_DOWN)) {
            camera_head_.rotate *= glm::angleAxis(0.01F, glm::vec3{1, 0, 0});
        }
        if (window_.getKey(GLFW_KEY_UP)) {
            camera_head_.rotate *= glm::angleAxis(-0.01F, glm::vec3{1, 0, 0});
        }
        // if (window_.getKey(GLFW_KEY_Z)) {
        //     setScale(scale / 1.01);
        // }
        // if (window_.getKey(GLFW_KEY_X)) {
        //     setScale(scale * 1.01);
        // }
        if (window_.getKey(GLFW_KEY_Q)) {
            exit(0);
        }
    }

  public:
    MobileNormalCamera(Window &window, World &world, const Viewport &viewport)
        : WorldObject(world)
        , Update(window)
        , camera_head_(world, viewport)
        , camera_base_(world) {
        this->append(&camera_base_);
        camera_base_.append(&camera_head_);
    }

    [[nodiscard]] glm::mat4 getViewMatrix() const override {
        return camera_head_.getViewMatrix();
    }

    [[nodiscard]] glm::mat4 getProjectionMatrix() const override {
        return camera_head_.getProjectionMatrix();
    }
};
