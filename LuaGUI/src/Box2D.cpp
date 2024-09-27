#include "Box2D.hpp"

// b2::World::SetGravityなど、「b2Vec2を受け取るメンバ関数」を「テーブルを受け取るメンバ関数」に変換する高階関数。
static std::function<void(b2::World *, const sol::table &)> wrap_table(void (b2::World::*f)(b2Vec2)) {
    return [f](b2::World *self, const sol::table &tbl) {
        b2Vec2 vec = {tbl.get<float>(1), tbl.get<float>(2)};
        (self->*f)(vec);
    };
}

static float to_angle(b2Rot rot) { return atan2(rot.s, rot.c); }

static void add_shape(RigidbodyComponent *rbc, const sol::table &tbl) {
    // tblにはShapeを作るときのオプション（形状の種類とそれに必要なパラメータ）を指定できる

    ColliderComponent *cc = nullptr;

    // Shape間に共通のパラメータをパースする
    b2::Shape::Params shape_params;
    shape_params.friction = tbl["friction"].get_or(0.1f);
    shape_params.restitution = tbl["restitution"].get_or(0.3f);

    // shape（衝突形状）を取得
    // NOTE: GCCではget_or<const char*>がバグで使えない？
    std::string shape = "circle";
    if (tbl["shape"].valid()) shape = tbl["shape"].get<const char *>();

    if (shape == "circle") {
        std::vector<float> center = tbl["center"].get_or(std::vector<float>{0, 0});
        auto x = center.at(0);
        auto y = center.at(1);

        float radius = tbl["radius"].get<float>();

        cc = rbc->get_owner()->add_component<ColliderComponent>(b2Circle{.center = b2Vec2{x, y}, .radius = radius}, shape_params);
    } else if (shape == "edge") {
        using Points = std::vector<std::vector<float>>;
        Points points = tbl["points"].get_or(Points{});
        auto x1 = points.at(0).at(0);
        auto y1 = points.at(0).at(1);
        auto x2 = points.at(1).at(0);
        auto y2 = points.at(1).at(1);

        cc = rbc->get_owner()->add_component<ColliderComponent>(b2Segment{{x1, y1}, {x2, y2}}, shape_params);
    } else if (shape == "rect") {
        float hx = tbl["halfWidth"].get_or(0.01f);
        float hy = tbl["halfHeight"].get_or(0.01f);

        auto poly = b2MakeBox(hx, hy);
        cc = rbc->get_owner()->add_component<ColliderComponent>(poly, shape_params);
    } else {
        warn("未知の形状種です: ", shape);
    }

    // 衝突時のコールバックを設定
    if (cc != nullptr && tbl["on_collision_enter"].valid()) {
        sol::function callback = tbl["on_collision_enter"];
        cc->on_collision_enter = [callback](ColliderComponent &self, ColliderComponent &other) { callback(self, other); };
    }
}

static void add_chain(RigidbodyComponent *rbc, const sol::table &tbl) {
    auto chain_params = b2::Chain::Params();

    // Chainの頂点座標を取得
    auto points = tbl["points"].get<std::vector<std::vector<float>>>();

    std::vector<b2Vec2> p_array;
    p_array.reserve(points.size());
    for (auto &p : points) {
        p_array.emplace_back(p.at(0), p.at(1));
    }

    chain_params.points = p_array.data();
    chain_params.count = (int)p_array.size();

    // ループするかどうか取得
    chain_params.isLoop = tbl["isLoop"].get_or(false);

    // 摩擦と反発
    chain_params.friction = tbl["friction"].get_or(0.1f);
    chain_params.restitution = tbl["restitution"].get_or(0.3f);

    auto *ccc = rbc->get_owner()->add_component<ChainColliderComponent>(chain_params);

    // 衝突時のコールバックを設定
    if (ccc != nullptr && tbl["on_collision_enter"].valid()) {
        sol::function callback = tbl["on_collision_enter"];
        ccc->on_collision_enter = [callback](ChainColliderComponent &self, ColliderComponent &collider) { callback(self, collider); };
    }
}

void register_box2d(sol::state &lua) {
    // Box2Dのシミュレーション縮尺を設定する関数
    lua.set_function("b2SetLengthUnitsPerMeter", b2SetLengthUnitsPerMeter);

    // Rigidbodyコンポーネント
    lua.new_usertype<RigidbodyComponent>(
        "Rigidbody",
        "position", sol::readonly_property([](RigidbodyComponent *rbc) { return rbc->b2body.GetPosition(); }),
        "rotation", sol::readonly_property([](RigidbodyComponent *rbc) { return to_angle(rbc->b2body.GetRotation()); }),
        "transform", sol::property([](RigidbodyComponent *rbc) { return rbc->b2body.GetTransform(); }, [](RigidbodyComponent *rbc, std::vector<float> pos, float angle) { rbc->b2body.SetTransform({pos[0], pos[1]}, {cos(angle), sin(angle)}); }),
        "linear_velocity", sol::property([](RigidbodyComponent *rbc) { return rbc->b2body.GetLinearVelocity(); }, [](RigidbodyComponent *rbc, std::vector<float> pos) { rbc->b2body.SetLinearVelocity({pos[0], pos[1]}); }),
        "angular_velocity", sol::property([](RigidbodyComponent *rbc) { return rbc->b2body.GetAngularVelocity(); }, [](RigidbodyComponent *rbc, float av) { rbc->b2body.SetAngularVelocity(av); }),
        "add_shape", add_shape,
        "add_chain", add_chain,
        sol::base_classes, sol::bases<Component>());

    // Colliderコンポーネント
    lua.new_usertype<ColliderComponent>(
        "Collider",
        "index", sol::readonly_property([](ColliderComponent *cc) { return cc->shape_ref_.Handle().index1; }),
        sol::base_classes, sol::bases<Component>());

    // ChainColliderコンポーネント
    lua.new_usertype<ChainColliderComponent>(
        "ChainCollider",
        "index", sol::readonly_property([](ChainColliderComponent *ccc) { return ccc->chain_ref_.Handle().index1; }),
        sol::base_classes, sol::bases<Component>());

    // Box2Dの各型
    lua.new_usertype<b2::World>(
        "b2World",
        "gravity", sol::property(&b2::World::GetGravity, wrap_table(&b2::World::SetGravity)));

    // TODO: 変数を公開（x,yにLuaからアクセスしたい）
    lua.new_usertype<b2Vec2>(
        "b2Vec2",
        // sol::constructors<b2Vec2(), b2Vec2(float, float)>(),
        // "x", &b2Vec2::x,
        // "y", &b2Vec2::y,
        "__tostring", [](b2Vec2 &self) { return "b2Vec2(" + std::to_string(self.x) + ", " + std::to_string(self.y) + ")"; });
}
