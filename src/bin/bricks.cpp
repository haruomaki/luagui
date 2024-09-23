#include "utility2.hpp"
#include <master.hpp>

class GlassBall : public MeshObject, public AABB2d {
    static StaticMesh &gen_mesh(Window &window) {
        auto &mesh = window.append_resource<StaticMesh>();
        mesh.use_index = false;
        mesh.vertices.coords = {{-0.1, -0.1, 0}, {0.1, -0.1, 0}, {0.1, 0.1, 0}, {-0.1, 0.1, 0}};
        mesh.vertices.uvs = {{0, 1}, {1, 1}, {1, 0}, {0, 0}};
        mesh.draw_mode = GL_TRIANGLE_FAN;
        mesh.sync_vram();
        return mesh;
    }

  public:
    GlassBall()
        : MeshObject(gen_mesh(this->get_world().window), this->get_world().window.find_resource<Material>("ガラス玉のマテリアル")) {}
};

class Brick : public MeshObject, public AABB2d {
    static StaticMesh &gen_mesh(Window &window) {
        auto &mesh = window.append_resource<StaticMesh>();
        mesh.vertices.coords = {{0, 0, 0}, {0.2, 0, 0}, {0.2, 0.1, 0}, {0, 0.1, 0}};
        mesh.vertices.uvs = {{0, 1}, {1, 1}, {1, 0}, {0, 0}};
        mesh.draw_mode = GL_TRIANGLE_FAN;
        mesh.sync_vram();
        return mesh;
    }

  public:
    Brick()
        : MeshObject(gen_mesh(this->get_world().window), this->get_world().window.find_resource<Material>("レンガのマテリアル")) {
        this->center = {0.1, 0.05};
        this->width = 0.2;
        this->height = 0.1;
    }
};

inline void flick_ball(glm::vec3 power_point, GlassBall &ball) {
    glm::vec2 direction = glm::vec2{ball.get_position() - power_point};
    auto distance = glm::length(direction);
    auto power = double(2 * exp(-distance));
    auto velocity2d = direction / distance * power;
    ball.velocity = {velocity2d.x, velocity2d.y, 0};
}

int main() {
    BufferedSet<int> bset;
    bset.request_set(3);
    bset.request_set(5);
    bset.request_set(8);
    bset.request_set(9);
    bset.request_set(11);
    bset.foreach_flush_combination([](int a, int b) {
        debug(a, b);
    });

    constexpr int width = 600, height = 500;
    GUI gui;
    Window &window = gui.create_window(width, height, "ウィンドウタイトル");
    World &world = window.create_world();
    // auto &camera = world.append_child<MobileOrthoCamera>();
    auto &camera = world.append_child<MobileNormalCamera>();
    camera.rotate = ANGLE_Y(M_PIf);
    camera.position = {0, 0, 2};
    camera.scale = 10;
    camera.set_active();

    // 三角形の表示
    auto &my_triangle_mesh = new_mesh(window, GL_TRIANGLE_FAN, {{-0.8, -0.3, 0}, {-0.2, 0.7, 0}, {0.5, -0.5, 0}},
                                      {
                                          {0.9, 0.3, 0, 1},
                                          {0.9, 0.2, 0.7, 0.3},
                                          {0.3, 0.7, 0.5, 0.5},
                                      });
    auto &my_triangle = world.append_child<MeshObject>(my_triangle_mesh);
    my_triangle.scale = 0.1;
    my_triangle.position = {-0.1, 0, 0};

    auto &stage = world.append_child<WorldObject>();
    stage.position = {0, 0, 0.1};

    // レンガの表示
    GLuint brick_texture = create_texture_from_png_file("assets/images/ピンクレンガ.png");
    auto &brick_material = MaterialBuilder().texture(brick_texture).build(window);
    brick_material.name = "レンガのマテリアル";

    constexpr int num_x = 10, num_y = 5;
    auto bricks = std::vector(num_x, std::vector<MeshObject *>(num_y));
    for (int x = 0; x < num_x; x++) {
        for (int y = 0; y < num_y; y++) {
            bricks[x][y] = &stage.append_child<Brick>();
            bricks[x][y]->position = {0.2 * x, 0.1 * y, 0.1};
        }
    }

    // ガラス玉のマテリアルを準備
    GLuint tex = create_texture_from_png_file("assets/images/青いガラス玉.png");
    auto &glass_ball_material = MaterialBuilder().texture(tex).build(window);
    glass_ball_material.name = "ガラス玉のマテリアル";

    auto gen = [&](glm::vec3 v) -> GlassBall & {
        auto &ball = stage.append_child<GlassBall>();
        ball.position = {0, 0, 0.1};
        ball.velocity = v;
        ball.width = 0.2;
        ball.height = 0.2;
        return ball;
    };

    auto &ball1 = gen({0.6, 0, 0});
    auto &ball2 = gen({0.3, 0.8, 0});
    auto &ball3 = gen({-0.1, -0.3, 0});

    stage.append_child<KeyCallbackObject>([&](int key, int action) {
        if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
            auto power_point = camera.get_position();
            flick_ball(power_point, ball1);
            flick_ball(power_point, ball2);
            flick_ball(power_point, ball3);
        }
        if (key == GLFW_KEY_U) {
            // stage.position += glm::vec3{1, 0, 0};
            stage.erase(); // FIXME: 描画されたままになる
        }
    });

    auto stage_color = glm::vec4{0.5, 0.7, 0.7, 1};
    auto &stage_material = MaterialBuilder().base_color(stage_color).priority(-1).build(window);
    auto &board_material = MaterialBuilder(stage_material).base_color({0.9, 0.6, 0.5, 1}).build(window);
    new_rect(stage, {{-1, 1, 0}, {1, 1, 0}, {1.1, 1.1, 0}, {-1.1, 1.1, 0}}, stage_material);
    new_rect(stage, {{-1.1, -1.1, 0}, {-1, -1, 0}, {-1, 1, 0}, {-1.1, 1.1, 0}}, stage_material);
    new_rect(stage, {{-1.1, -1.1, 0}, {1.1, -1.1, 0}, {1, -1, 0}, {-1, -1, 0}}, stage_material);
    new_rect(stage, {{1, -1, 0}, {1.1, -1.1, 0}, {1.1, 1.1, 0}, {1, 1, 0}}, stage_material);
    new_rect(stage, {{-1, -1, 0}, {1, -1, 0}, {1, 1, 0}, {-1, 1, 0}}, board_material);

    gui.mainloop();
}
