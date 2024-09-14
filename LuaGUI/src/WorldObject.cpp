#include "WorldObject.hpp"
#include <master.hpp>
#include <utility>

static void add_update_component(sol::state &lua, WorldObject *obj, sol::function f) {
    sol::thread runner_thread = sol::thread::create(lua);
    sol::state_view runner_thread_state = runner_thread.state();
    auto co = std::make_shared<sol::coroutine>(runner_thread_state, f);

    auto runner = [co](UpdateComponent &self) {
        auto result = (*co)();
        auto status = result.status();

        if (status == sol::call_status::yielded) {
            // コルーチンが yield した
        } else if (status == sol::call_status::ok) {
            // コルーチンが終了した
            self.erase();
        } else {
            // エラーが発生した
            sol::error err = result;
            std::cerr << "Error in Lua coroutine: " << err.what() << std::endl;
            self.erase();
        }
    };

    obj->add_component<UpdateComponent>(runner);
}

void register_world_object(sol::state &lua) {
    lua.new_usertype<WorldObject>(
        "WorldObject",

        "position",
        sol::property([](WorldObject *obj) { return obj->get_position(); }, [](WorldObject *obj, std::vector<float> pos) { obj->set_position({pos[0], pos[1], 0}); }),

        "add_update_component",
        [&lua](WorldObject *obj, sol::function f) { add_update_component(lua, obj, std::move(f)); });

    lua.new_usertype<MeshObject>("MeshObject", sol::base_classes, sol::bases<WorldObject>());
}
