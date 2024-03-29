#include "Draw.hpp"
#include "World.hpp"

Draw::Draw() {
    auto func = [this](const Camera &camera) { this->draw(camera); };
    this->func_id_ = this->get_world().draws.set_function(std::move(func));
}
Draw::~Draw() {
    this->get_world().draws.erase_function(this->func_id_);
}
