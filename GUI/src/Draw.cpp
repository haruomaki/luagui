#include <Draw.hpp>

Draw::Draw(World &world)
    : world_(world) {
    draw_ = [this](const Camera &camera) { this->draw(camera); };
    world_.draws_.insert(&draw_);
}
Draw::~Draw() {
    world_.draws_.erase(&draw_);
}
