#include <Update.hpp>

Update::Update(Window &window)
    : window_(window) {
    update_ = [this] { this->update(); };
    window.updates_.insert(&update_);
}
Update::~Update() {
    window_.updates_.erase(&update_);
}
