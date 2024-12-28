#include "Update.hpp"
#include "World.hpp"

UpdateComponent::UpdateComponent(const std::function<void(UpdateComponent &)> &f, std::string category)
    : category_(std::move(category)) {
    this->func_ = [f, this] { trace("lambda from UpdateComponent:", this->category_, " (id: ", this->id, ")"); f(*this); trace("lambda ended"); };
    if (category_ == "Update") {
        world().updates.request_set(&this->func_);
    } else if (category_ == "Draw") {
        world().draws.request_set(&this->func_);
    }
}

UpdateComponent::~UpdateComponent() {
    print("UpdateComponentのデストラクタです。");
    func_ = [] {};
    print("func_解放");
    if (category_ == "Update") {
        world().updates.request_erase(&this->func_);
    } else if (category_ == "Draw") {
        world().draws.request_erase(&this->func_);
    }
}
