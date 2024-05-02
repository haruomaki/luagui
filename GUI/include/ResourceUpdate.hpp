#pragma once

#include "Resource.hpp"

class ResourceUpdate : virtual public Resource {
    std::function<void()> func_;

  public:
    virtual void update() = 0;
    ResourceUpdate();
    ~ResourceUpdate() override;
    ResourceUpdate(const ResourceUpdate &) = delete;
    ResourceUpdate &operator=(const ResourceUpdate &) const = delete;
    ResourceUpdate(ResourceUpdate &&) = delete;
    ResourceUpdate &operator=(ResourceUpdate &&) const = delete;
};
