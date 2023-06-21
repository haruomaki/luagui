#include <Draw.hpp>

static std::set<std::function<void()> *> global_draws;

Draw::Draw() {
    draw_ = [this] { this->draw(); };
    global_draws.insert(&draw_);
}
Draw::~Draw() {
    global_draws.erase(&draw_);
}

void masterDraw() {
    for (auto *draw : global_draws) {
        (*draw)();
    }
    // for (auto it = global_draws.rbegin(); it != global_draws.rend(); it++) {
    //     (*(*it))();
    // }
}
