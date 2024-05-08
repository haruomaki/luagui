#include <box2d.h>

#include <print>
#include <thread>

using namespace std::chrono_literals;

int main() {
    // Box2Dワールドの初期化
    b2Vec2 gravity(0.0f, -9.8f);
    b2World world(gravity);

    // 地面となるボックスを作成
    b2BodyDef ground_body_def;
    ground_body_def.position.Set(0.0f, -10.0f);

    b2Body *ground_body = world.CreateBody(&ground_body_def);

    b2PolygonShape ground_box;
    ground_box.SetAsBox(50.0f, 10.0f);
    ground_body->CreateFixture(&ground_box, 0.0f);

    // 動くボックスを作成
    b2BodyDef body_def;
    body_def.type = b2_dynamicBody; // 力を受けて動くように
    body_def.position.Set(0.0f, 4.0f);
    body_def.angle = 0.5;
    b2Body *body = world.CreateBody(&body_def);

    b2PolygonShape dynamic_box;
    dynamic_box.SetAsBox(1.0f, 1.0f);

    b2FixtureDef fixture_def;
    fixture_def.shape = &dynamic_box;
    fixture_def.density = 1.0f;
    fixture_def.friction = 0.3f;
    body->CreateFixture(&fixture_def);

    // ワールドのシミュレーション
    float time_step = 1.0f / 60.0f;
    int32 velocity_iterations = 8;
    int32 position_iterations = 3;

    for (size_t i = 0; i < 200; i++) {
        world.Step(time_step, velocity_iterations, position_iterations);

        // bodyの位置と回転を取得
        b2Vec2 position = body->GetPosition();
        float angle = body->GetAngle();
        std::println("position: ({}, {}), angle: {}", position.x, position.y, angle);

        std::this_thread::sleep_for(1s / 60.0f);
    }

    return 0;
}
