#include "World.hpp"
#include "WorldObject.hpp"

class UpdateObject : virtual public WorldObject {
    std::function<void()> func_;

  public:
    UpdateObject(std::function<void(UpdateObject &)> &&f) {
        this->func_ = [f, this] { f(*this); };
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

class KeyCallbackObject : virtual public WorldObject {
    FunctionId id_;

  public:
    KeyCallbackObject(std::function<void(int key, int action)> &&f) {
        auto id = this->get_world().window.key_callbacks.request_set_function(std::move(f));
        this->id_ = id;
    }
    ~KeyCallbackObject() override {
        auto id = this->id_;
        this->get_world().window.key_callbacks.request_erase_function(id);
    }

    KeyCallbackObject(const KeyCallbackObject &) = delete;
    KeyCallbackObject &operator=(const KeyCallbackObject &) const = delete;
    KeyCallbackObject(KeyCallbackObject &&) = delete;
    KeyCallbackObject &operator=(KeyCallbackObject &&) const = delete;
};
