#pragma once

#include <World.hpp>

class Update : virtual public WorldObject {
    FunctionId func_id_;

  public:
    virtual void update() = 0;
    Update();
    ~Update() override;
    Update(const Update &) = delete;
    Update &operator=(const Update &) const = delete;
    Update(Update &&) = delete;
    Update &operator=(Update &&) const = delete;
};
