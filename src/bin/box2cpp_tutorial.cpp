#include <box2cpp/box2cpp.h>

int main() {
    b2::World::Params world_params;
    world_params.gravity = {0, 0};
    b2::World w(world_params);

    // 直径3と直径5のボールが衝突するシミュレーション

    // 球その１
    b2::Body::Params bp1;
    bp1.type = b2_dynamicBody;
    bp1.position = {0, 0};
    bp1.linearVelocity = {1, 0};

    b2::Body ball1;
    b2::Body ball_tmp = w.CreateBody(b2::OwningHandle, bp1);
    ball1 = std::move(ball_tmp);

    ball1.CreateShape(
        b2::DestroyWithParent,
        b2::Shape::Params{},
        b2Circle{.center = b2Vec2(), .radius = 3});

    // 球その２
    b2::Body::Params bp2;
    bp2.type = b2_dynamicBody;
    bp2.position = {10, 0};
    bp2.linearVelocity = {-1, 0};

    b2::Body ball2 = w.CreateBody(b2::OwningHandle, bp2);

    ball2.CreateShape(
        b2::DestroyWithParent,
        b2::Shape::Params{},
        b2Circle{.center = {}, .radius = 5});

    for (int i = 0; i < 100; i++) {
        w.Step(1 / 60.f, 4);
        std::cout << ball1.GetPosition().x << " " << ball2.GetPosition().x << "\n";
    }

    // No cleanup needed, everything is destroyed automatically.
}
