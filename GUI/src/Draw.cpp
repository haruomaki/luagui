#include "Draw.hpp"
#include "World.hpp"

Draw::Draw() {
    this->func_ = [this] { this->draw(); };
    this->get_world().draws.request_set(&this->func_);
}
Draw::~Draw() {
    this->get_world().draws.request_erase(&this->func_);
}
