#include <Drawable.hpp>
#include <World.hpp>

DrawableWorldObject::DrawableWorldObject() {
    draw_ = [this](const Camera &camera) { this->draw(camera); };
    this->get_world().draws_.insert(&draw_);
}
DrawableWorldObject::~DrawableWorldObject() {
    auto &world = get_world();
    world.draws_.erase(&draw_);
}
