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
        if (window_.getKey(GLFW_KEY_RIGHT)) {
            debug("RIGHT");
        }
        if (window_.getKey(GLFW_KEY_LEFT)) {
            debug("LEFT");
        }
        if (window_.getKey(GLFW_KEY_DOWN)) {
            debug("DOWN");
        }
        if (window_.getKey(GLFW_KEY_UP)) {
            debug("UP");
        }
    }

  public:
    MobileOrthoCamera(Window &window, World &world, const Viewport &viewport)
        : OrthoCamera(world, viewport)
        , Update(window) {}
};
