#pragma once

#include <World.hpp>

class Update {
    std::function<void()> update_;

  public:
    World &world;

    virtual void update() = 0;
    Update(World &world);
    virtual ~Update();
};
