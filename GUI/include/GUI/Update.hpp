#pragma once

#include "Component.hpp"
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

class UpdateComponent : public Component {
    std::function<void()> func_;

  public:
    UpdateComponent(std::function<void(UpdateComponent &)> &&f);
    ~UpdateComponent() override;
    UpdateComponent(const UpdateComponent &) = delete;
    UpdateComponent &operator=(const UpdateComponent &) const = delete;
    UpdateComponent(UpdateComponent &&) = delete;
    UpdateComponent &operator=(UpdateComponent &&) const = delete;
};
