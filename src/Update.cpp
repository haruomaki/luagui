#include <Update.hpp>

static std::set<std::function<void()> *> global_updates;

Update::Update() {
    update_ = [this] { this->update(); };
    global_updates.insert(&update_);
}
Update::~Update() {
    global_updates.erase(&update_);
}

void masterUpdate() {
    for (auto *update : global_updates) {
        (*update)();
    }
}
