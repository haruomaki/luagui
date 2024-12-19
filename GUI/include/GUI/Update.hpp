#pragma once

#include "Component.hpp"

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
