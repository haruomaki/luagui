#include <Update.hpp>
#include <Viewport.hpp>
#include <WorldObject.hpp>

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
