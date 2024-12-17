#include "WorldObject.hpp"

class UpdateObject : virtual public WorldObject {
    std::function<void()> func_;

  public:
    UpdateObject(std::function<void(UpdateObject &)> &&f) {
        this->func_ = [f, this] { trace("UpdateObjectクラスによるアップデート開始"); f(*this); };
        this->get_world().updates.request_set(&this->func_);
    }
    ~UpdateObject() override {
        this->get_world().updates.request_erase(&this->func_);
    }

    UpdateObject(const UpdateObject &) = delete;
    UpdateObject &operator=(const UpdateObject &) const = delete;
    UpdateObject(UpdateObject &&) = delete;
    UpdateObject &operator=(UpdateObject &&) const = delete;
};
