#include <Update.hpp>

Update::Update(Window &window)
    : window(window) {
    update_ = [this] { this->update(); };
    window.updates_.insert(&update_);
}
Update::~Update() {
    window.updates_.erase(&update_);
}
