#pragma once

#include <Window.hpp>

class Update {
    std::function<void()> update_;

  public:
    Window &window;

    virtual void update() = 0;
    Update(Window &window);
    virtual ~Update();
};
