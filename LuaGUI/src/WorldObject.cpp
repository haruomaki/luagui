#include "WorldObject.hpp"
#include <master.hpp>
#include <utility>

static void add_update_component(sol::state &lua, WorldObject *obj, sol::function f) {
    // fをコルーチンとして毎フレーム実行し、コルーチンが終了したらコンポーネントも削除する
    auto runner_thread = std::make_shared<sol::thread>(sol::thread::create(lua));
    auto co = std::make_shared<sol::coroutine>(runner_thread->state(), f);

    // INFO: runner_threadもキャプチャしておかないとSEGV
    auto runner = [runner_thread, co](UpdateComponent &self) {
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

static RigidbodyComponent *add_rigidbody_component(sol::state &lua, WorldObject *obj, const sol::optional<sol::table> &tbl_opt) {
    // tblにはb2::Body::Paramsで設定できる要素が入れられる
    sol::table tbl = tbl_opt.value_or(lua.create_table());

    b2::Body::Params body_params;

    // 演算タイプ
    auto ts = tbl["type"].get_or<std::string_view>("");
    body_params.type = (ts == "static" ? b2_staticBody : (ts == "kinematic" ? b2_kinematicBody : b2_dynamicBody));

    auto pos = obj->get_absolute_position();
    body_params.position = {pos.x, pos.y};
    body_params.sleepThreshold = 0.0005f; // スリープ状態を防ぐ

    auto *rbc = obj->add_component<RigidbodyComponent>(body_params);
    return rbc;
}

static sol::object get_component(sol::state &lua, WorldObject *obj, const std::string &component_type) {
    if (component_type == "Rigidbody") {
        auto *rbc = obj->get_component<RigidbodyComponent>();
        return sol::make_object(lua, rbc);
    }
    return sol::nil;
}

void register_world_object(sol::state &lua) {
    lua.new_usertype<WorldObject>(
        "WorldObject",

        "position",
        sol::property([](WorldObject *obj) { return obj->get_position(); }, [](WorldObject *obj, std::vector<float> pos) { obj->set_position({pos[0], pos[1], 0}); }),

        "add_update_component",
        [&lua](WorldObject *obj, sol::function f) { add_update_component(lua, obj, std::move(f)); },

        "add_rigidbody_component",
        [&lua](WorldObject *obj, const sol::optional<sol::table> &tbl_opt) { return add_rigidbody_component(lua, obj, tbl_opt); },

        "get_component",
        [&lua](WorldObject *obj, const std::string &component_type) { return get_component(lua, obj, component_type); },

        "erase",
        [](WorldObject *obj) { obj->erase(); });

    // Componentクラス
    lua.new_usertype<Component>(
        "Component",
        "owner", sol::readonly_property([](Component *comp) { return comp->get_owner(); }),
        "erase", [](Component *comp) { comp->erase(); });

    lua.new_usertype<MeshObject>("MeshObject", sol::base_classes, sol::bases<WorldObject>());
}
