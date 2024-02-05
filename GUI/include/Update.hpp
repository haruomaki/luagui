#pragma once

#include <World.hpp>

class Update {
    FunctionId func_id_;

  public:
    World &world;

    virtual void update() = 0;
    Update(World &world);
    virtual ~Update();
};
