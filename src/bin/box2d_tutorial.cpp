#include <box2d.h>

int main() {
    // Box2D world initialization
    b2Vec2 gravity(0.0f, -9.8f);
    b2World world(gravity);

    // Create a box shape
    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;

    b2PolygonShape box_shape;
    box_shape.SetAsBox(0.5f, 0.5f);

    // Create a body and attach the shape
    b2Body *body = world.CreateBody(&body_def);
    body->CreateFixture(&box_shape, 1.0f);

    // Apply a force to the body
    b2Vec2 force(5.0f, 5.0f);
    body->ApplyForceToCenter(force, true);

    // Step the simulation
    float time_step = 1.0f / 60.0f;
    int32 velocity_iterations = 8;
    int32 position_iterations = 3;

    world.Step(time_step, velocity_iterations, position_iterations);

    // Get the body's position and rotation
    b2Vec2 position = body->GetPosition();
    float angle = body->GetAngle();

    // std::cout << position.x << std::endl;
    // std::print("Hello {} World\n", 42);

    // Clean up
    // delete &world;

    return 0;
}
