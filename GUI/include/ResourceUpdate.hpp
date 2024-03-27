#pragma once

#include "FunctionSet.hpp"
#include "WindowObject.hpp"

class ResourceUpdate : virtual public Resource {
    FunctionId func_id_;

  public:
    virtual void update() = 0;
    ResourceUpdate();
    ~ResourceUpdate() override;
    ResourceUpdate(const ResourceUpdate &) = delete;
    ResourceUpdate &operator=(const ResourceUpdate &) const = delete;
    ResourceUpdate(ResourceUpdate &&) = delete;
    ResourceUpdate &operator=(ResourceUpdate &&) const = delete;
};
