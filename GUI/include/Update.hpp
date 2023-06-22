#pragma once

#include <Window.hpp>

class Update {
    Window &window_;
    std::function<void()> update_;

  public:
    virtual void update() = 0;
    Update(Window &window);
    ~Update();
};
