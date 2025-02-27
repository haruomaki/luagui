#include "WorldObject.hpp"
#include <GUI/Mesh.hpp>
#include <GUI/Rigidbody2D.hpp>
#include <GUI/Update.hpp>
#include <GUI/World.hpp> // worldフィールド（プロパティの戻り値）に必要？
#include <GUI/WorldObject.hpp>
#include <GUI/sound.hpp>

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
            std::cerr << "Error in Lua coroutine: " << err.what() << '\n';
            self.erase();
        }
        trace("[LuaGUI] UpdateComponent runner end");
    };

    auto &uc = obj->add_component<UpdateComponent>(std::move(runner));
    uc.name = std::move(id);
    // print("add_update_componentおわり,", uc, ", ", uc->id);
    return &uc;
}

static Rigidbody2D *add_rigidbody_component(sol::state &lua, WorldObject *obj, const sol::optional<sol::table> &tbl_opt) {
    // tblにはb2::Body::Paramsで設定できる要素が入れられる
    sol::table tbl = tbl_opt.value_or(lua.create_table());

    b2::Body::Params body_params;

    // 演算タイプ
    auto ts = tbl["type"].get_or<std::string_view>("");
    body_params.type = (ts == "dynamic" ? b2_dynamicBody : (ts == "kinematic" ? b2_kinematicBody : b2_staticBody));

    // 連続的衝突判定：CCDをdynamicBody同士で行うかどうか
    body_params.isBullet = tbl["isBullet"].get_or(false);

    auto pos = obj->get_absolute_position();
    body_params.position = {.x = pos.x, .y = pos.y};
    body_params.sleepThreshold = 0.0005f; // スリープ状態を防ぐ

    auto &rbc = obj->add_component<Rigidbody2D>(body_params);
    return &rbc;
}

static sol::object get_component(sol::state &lua, WorldObject *obj, const std::string &component_type) {
    // TODO: あらゆるクラス名に対応
    if (component_type == "Update") return sol::make_object(lua, obj->get_component<UpdateComponent>());
    if (component_type == "Rigidbody2D") {
        auto *rbc = obj->get_component<Rigidbody2D>();
        return sol::make_object(lua, rbc);
    }
    if (component_type == "Rigidbody") return sol::make_object(lua, obj->get_component<Rigidbody>());
    if (component_type == "SoundSource") {
        auto *ss = obj->get_component<SoundSource>();
        return sol::make_object(lua, ss);
    }
    warn("未対応のコンポーネントクラス名です。");
    return sol::nil;
}

static sol::object get_component_by_name(sol::state &lua, WorldObject *obj, const std::string &name, bool warn_not_found) {
    Component *comp = obj->get_component_by_name(name, warn_not_found);
    if (comp == nullptr) {
        if (warn_not_found) warn("コンポーネントが見つかりませんでした。");
        return sol::nil;
    }
    // debug(id, comp);
    if (auto *p = dynamic_cast<UpdateComponent *>(comp)) return sol::make_object(lua, p);
    if (auto *p = dynamic_cast<Rigidbody2D *>(comp)) return sol::make_object(lua, p);
    if (auto *p = dynamic_cast<Collider2D *>(comp)) return sol::make_object(lua, p);
    if (auto *p = dynamic_cast<UpdateComponent *>(comp)) return sol::make_object(lua, p);
    if (auto *p = dynamic_cast<MeshComponent *>(comp)) return sol::make_object(lua, p);
    warn("未対応のコンポーネント型のため、キャストできませんでした。");
    return sol::nil;
}

void register_world_object(sol::state &lua) {
    lua.new_usertype<glm::quat>(
        "quat",
        "angle_axis", [](float angle, glm::vec3 axis) { return glm::angleAxis(angle, axis); },
        sol::meta_function::multiplication, [](glm::quat a, glm::quat b) { return a * b; });

    lua.new_usertype<WorldObject>(
        "WorldObject",

        "append_empty_child",
        [](WorldObject *obj) { return &obj->append_child<WorldObject>(); },

        "world",
        sol::readonly_property([](WorldObject *obj) { return &obj->get_world(); }),
        "parent",
        sol::readonly_property([](WorldObject *obj) { return obj->get_parent(); }),
        "children",
        [](WorldObject *obj) { return sol::as_table(obj->children()); },

        "position",
        sol::property([](WorldObject *obj) { return obj->get_position(); }, [](WorldObject *obj, glm::vec3 pos) { obj->set_position(pos); }),
        "rotation",
        sol::property([](WorldObject *obj) { return obj->get_rotate(); }, [](WorldObject *obj, glm::quat rot) { obj->set_rotate(rot); }),
        "scale_prop",
        sol::property([](WorldObject *obj) { return obj->get_scale_prop(); }, [](WorldObject *obj, float scale) { obj->set_scale_prop(scale); }),
        "absolute_position",
        sol::readonly_property([](WorldObject *obj) { return obj->get_absolute_position(); }),

        "id",
        &WorldObject::id,

        "ptr", &WorldObject::ptr,
        // sol::meta_function::equal_to, [](WorldObject &o1, WorldObject &o2) -> bool { print("は？");return o1 == o2; },
        // sol::meta_function::equal_to, [](WorldObject *o1, WorldObject *o2) -> bool { print("は？");return *o1 == *o2; },
        sol::meta_function::equal_to, &WorldObject::operator==,

        "front", &WorldObject::get_front,
        "back", &WorldObject::get_back,
        "right", &WorldObject::get_right,
        "left", &WorldObject::get_left,
        "up", &WorldObject::get_up,
        "down", &WorldObject::get_down,

        "add_update_component",
        [&lua](WorldObject *obj, std::string id, sol::function f) { return add_update_component(lua, obj, std::move(id), std::move(f)); },

        "add_rigidbody2d_component",
        [&lua](WorldObject *obj, const sol::optional<sol::table> &tbl_opt) { return add_rigidbody_component(lua, obj, tbl_opt); },

        "get_component",
        [&lua](WorldObject *obj, const std::string &component_type) { return get_component(lua, obj, component_type); },

        "get_component_by_name",
        [&lua](WorldObject *obj, const std::string &name) { return get_component_by_name(lua, obj, name, true); },
        "find_component_by_name",
        [&lua](WorldObject *obj, const std::string &name) { return get_component_by_name(lua, obj, name, false); },

        "erase",
        [](WorldObject *obj) { obj->erase(); },

        "force_erase",
        [](WorldObject *obj) { obj->force_erase(); });

    // Componentクラス
    lua.new_usertype<Component>(
        "Component",
        "name", &Component::name,
        "owner", sol::readonly_property([](Component *comp) { return &comp->owner(); }),
        "erase", [](Component *comp) { trace("[LuaGUI] Component:erase id=", comp->name); comp->erase(); });

    // Updateコンポーネント
    lua.new_usertype<UpdateComponent>(
        "Update",
        sol::base_classes, sol::bases<Component>());

    lua.new_usertype<MeshComponent>("MeshComponent", sol::base_classes, sol::bases<Component>());
}
