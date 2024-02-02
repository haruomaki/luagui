#pragma once

#include <GUI.hpp>

class Update {
    std::function<void()> update_;

  public:
    GUI &window;

    virtual void update() = 0;
    Update(GUI &window);
    virtual ~Update();
};
