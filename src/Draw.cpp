#include <Draw.hpp>

static std::set<std::function<void()> *> global_draws;

Draw::Draw() {
    draw_ = bind(&Draw::draw, this);
    global_draws.insert(&draw_);
}
Draw::~Draw() {
    global_draws.erase(&draw_);
}

void masterDraw() {
    for (auto *draw : global_draws) {
        (*draw)();
    }
}
