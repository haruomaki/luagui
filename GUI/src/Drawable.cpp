#include <Drawable.hpp>
#include <World.hpp>

DrawableWorldObject::DrawableWorldObject() {
    auto func = [this](const Camera &camera) { this->draw(camera); };
    this->func_id_ = this->get_world().draws.set_function(std::move(func));
}
DrawableWorldObject::~DrawableWorldObject() {
    this->get_world().draws.erase_function(this->func_id_);
}
