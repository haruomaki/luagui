#include "Update.hpp"
#include "World.hpp"

Update::Update() {
    auto func = [this] { this->update(); };
    this->func_id_ = this->get_world().updates.set_function(std::move(func));
}
Update::~Update() {
    this->get_world().updates.erase_function(this->func_id_);
}
Update::Update(const Update & /*other*/) {
    auto func = [this] { this->update(); };
    this->func_id_ = this->get_world().updates.set_function(std::move(func));
}
