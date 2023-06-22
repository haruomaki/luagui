#include <Draw.hpp>
#include <World.hpp>

Draw::Draw(World &world)
    : WorldObject(world) {
    draw_ = [this](const Camera &camera) { this->draw(camera); };
    world.draws_.insert(&draw_);
}
Draw::~Draw() {
    auto *world = getWorld();
    world->draws_.erase(&draw_);
}
