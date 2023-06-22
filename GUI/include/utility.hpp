#include <Camera.hpp>
#include <KeyCallback.hpp>
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
    StickyPointTopLeft(World &world, const Viewport &viewport)
        : WorldObject(world)
        , viewport_(viewport) {}
};

class MobileOrthoCamera : public OrthoCamera, KeyCallback {
    void keyCallback(int key, int action) override {
        if (action == GLFW_PRESS) {
            switch (key) {
            case GLFW_KEY_RIGHT:
                debug("RIGHT!");
                break;
            case GLFW_KEY_LEFT:
                debug("LEFT!");
                break;
            case GLFW_KEY_DOWN:
                debug("DOWN!");
                break;
            case GLFW_KEY_UP:
                debug("UP!");
                break;
            }
        }
    }

  public:
    MobileOrthoCamera(Window &window, World &world, const Viewport &viewport)
        : OrthoCamera(world, viewport)
        , KeyCallback(window) {}
};
