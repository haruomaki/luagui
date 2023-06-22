#pragma once

#include <Window.hpp>

class Update {
    std::function<void()> update_;

  protected:
    Window &window_;

  public:
    virtual void update() = 0;
    Update(Window &window);
    ~Update();
};
