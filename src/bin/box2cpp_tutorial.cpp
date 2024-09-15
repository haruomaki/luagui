#include <box2cpp/box2cpp.h>

int main() {
    b2::World w(b2::World::Params{});

    b2::Body::Params bp;
    bp.type = b2_dynamicBody;

    b2::Body b = w.CreateBody(b2::OwningHandle, bp);

    b.CreateShape(
        b2::DestroyWithParent,
        b2::Shape::Params{},
        b2Circle{.center = b2Vec2(), .radius = 3});

    for (int i = 0; i < 10; i++) {
        w.Step(1 / 60.f, 4);
        std::cout << b.GetPosition().y << "\n";
    }

    // No cleanup needed, everything is destroyed automatically.
}
