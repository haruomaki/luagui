#pragma once

#include "WorldObject.hpp"

class Update : virtual public WorldObject {
    std::function<void()> func_;

  public:
    virtual void update() = 0;
    Update();
    ~Update() override;
    Update(const Update &);
    Update &operator=(const Update &) const = delete;
    Update(Update &&) = delete;
    Update &operator=(Update &&) const = delete;
};
