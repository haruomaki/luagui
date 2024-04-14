#include "World.hpp"
#include "WorldObject.hpp"

class UpdateObject : virtual public WorldObject {
    FunctionId id_;

  public:
    UpdateObject(std::function<void()> &&f) {
        auto id = this->get_world().updates.set_function(std::move(f));
        this->id_ = id;
    }
    ~UpdateObject() override {
        auto id = this->id_;
        this->get_world().updates.erase_function(id);
    }

    UpdateObject(const UpdateObject &) = delete;
    UpdateObject &operator=(const UpdateObject &) const = delete;
    UpdateObject(UpdateObject &&) = delete;
    UpdateObject &operator=(UpdateObject &&) const = delete;
};
