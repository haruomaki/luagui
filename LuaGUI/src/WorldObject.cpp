#include "WorldObject.hpp"
#include <GUI/master.hpp>

static UpdateComponent *add_update_component(sol::state &lua, WorldObject *obj, std::string id, sol::function f) {
    // fをコルーチンとして毎フレーム実行し、コルーチンが終了したらコンポーネントも削除する
    auto runner_thread = std::make_shared<sol::thread>(sol::thread::create(lua));
    auto co = std::make_shared<sol::coroutine>(runner_thread->state(), f);

    // INFO: runner_threadもキャプチャしておかないとSEGV
    auto runner = [runner_thread, co](UpdateComponent &self) {
        trace("[LuaGUI] UpdateComponent runner start");
        trace("[LuaGUI] UpdateComponent runner x");
        auto result = (*co)(self);
        trace("[LuaGUI] UpdateComponent runner 1");
        auto status = result.status();

        trace("[LuaGUI] UpdateComponent runner 2");
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
        trace("[LuaGUI] UpdateComponent runner end");
    };

    auto *uc = obj->add_component<UpdateComponent>(runner);
    uc->id = std::move(id);
    // print("add_update_componentおわり,", uc, ", ", uc->id);
    return uc;
}

static RigidbodyComponent *add_rigidbody_component(sol::state &lua, WorldObject *obj, const sol::optional<sol::table> &tbl_opt) {
    // tblにはb2::Body::Paramsで設定できる要素が入れられる
    sol::table tbl = tbl_opt.value_or(lua.create_table());

    b2::Body::Params body_params;

    // 演算タイプ
    auto ts = tbl["type"].get_or<std::string_view>("");
    body_params.type = (ts == "dynamic" ? b2_dynamicBody : (ts == "kinematic" ? b2_kinematicBody : b2_staticBody));

    // 連続的衝突判定：CCDをdynamicBody同士で行うかどうか
    body_params.isBullet = tbl["isBullet"].get_or(false);

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

static sol::object get_component_by_id(sol::state &lua, WorldObject *obj, const std::string &id) {
    Component *comp = obj->get_component_by_id(id);
    // debug(id, comp);
    if (auto *p = dynamic_cast<RigidbodyComponent *>(comp)) return sol::make_object(lua, p);
    if (auto *p = dynamic_cast<ColliderComponent *>(comp)) return sol::make_object(lua, p);
    if (auto *p = dynamic_cast<UpdateComponent *>(comp)) return sol::make_object(lua, p);
    return sol::nil;
}

void register_world_object(sol::state &lua) {
    lua.new_usertype<WorldObject>(
        "WorldObject",

        "append_empty_child",
        [](WorldObject *obj) { return &obj->append_child<WorldObject>(); },

        "children",
        [](WorldObject *obj) { return sol::as_table(obj->children()); },

        "position",
        sol::property([](WorldObject *obj) { return obj->get_position(); }, [](WorldObject *obj, std::vector<float> pos) { obj->set_position({pos[0], pos[1], 0}); }),

        "scale_prop",
        sol::property([](WorldObject *obj) { return obj->get_scale_prop(); }, [](WorldObject *obj, float scale) { obj->set_scale_prop(scale); }),

        "add_update_component",
        [&lua](WorldObject *obj, std::string id, sol::function f) { return add_update_component(lua, obj, std::move(id), std::move(f)); },

        "add_rigidbody_component",
        [&lua](WorldObject *obj, const sol::optional<sol::table> &tbl_opt) { return add_rigidbody_component(lua, obj, tbl_opt); },

        "get_component",
        [&lua](WorldObject *obj, const std::string &component_type) { return get_component(lua, obj, component_type); },

        "get_component_by_id",
        [&lua](WorldObject *obj, const std::string &id) { return get_component_by_id(lua, obj, id); },

        "erase",
        [](WorldObject *obj) { obj->erase(); });

    // Componentクラス
    lua.new_usertype<Component>(
        "Component",
        "id", &Component::id,
        "owner", sol::readonly_property([](Component *comp) { return &comp->owner(); }),
        "erase", [](Component *comp) { trace("[LuaGUI] Component:erase id=", comp->id); comp->erase(); });

    // Updateコンポーネント
    lua.new_usertype<UpdateComponent>(
        "Update",
        sol::base_classes, sol::bases<Component>());

    lua.new_usertype<MeshObject>("MeshObject", sol::base_classes, sol::bases<WorldObject>());

    // Cameraクラス
    lua.new_usertype<Camera>("Camera", sol::base_classes, sol::bases<WorldObject>());
}
