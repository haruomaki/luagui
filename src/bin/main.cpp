#include <master.hpp>
#include <utility.hpp>

class MovingShape : public Shape, Update {
    static constexpr float speed = 1.5;
    float velocity_ = -speed;

    void update() override {
        auto next_pos = get_position() + glm::vec3(0, velocity_, 0);
        set_position(next_pos);
        if (next_pos.y < -300 || 300 < next_pos.y) {
            velocity_ *= -1.1f;
        }
    }

  public:
    MovingShape(Polygon polygon)
        : Shape(polygon) {}
};

int main() {
    GUI gui;
    Window &window = gui.create_window(600, 500, "めいん");
    auto &viewport = window.make_child<MaximumViewport>();
    World &main_world = window.create_world();

    // バーテックスシェーダのコンパイル
    auto vsh_string = load_string("assets/shaders/default.vsh");
    auto vsh_id = create_shader(GL_VERTEX_SHADER, vsh_string);

    // フラグメントシェーダのコンパイル
    auto fsh_string = load_string("assets/shaders/default.fsh");
    auto fsh_id = create_shader(GL_FRAGMENT_SHADER, fsh_string);

    auto main_shader = ProgramObject{vsh_id, fsh_id};

    auto &camera = main_world.append_child<MobileNormalCamera>(viewport);
    camera.position = {0, 0, 1000};
    camera.rotate = ANGLE_Y(M_PIf);
    camera.scale = 1;
    camera.set_active();

    Font migmix_font;
    auto &sample_text = main_world.append_child<Text>(migmix_font, "This is sample text 123456789", RGBA{0.5, 0.8, 0.2, 0.4});
    auto &credit_text = main_world.append_child<Text>(migmix_font, "(C) LearnOpenGL.com", RGBA{0.3, 0.7, 0.9, 0.4});

    sample_text.set_position({-200, 50, 200});
    credit_text.set_position({200, 400, 1});

    GLuint tex_id = loadTexture("assets/images/cat.raw");

    Polygon gon(main_shader, {{0.9f, 0.9f, 0.4}, {0.5f, 0.f, 0}, {0.f, 0.f, 0.2}, {0.f, 0.5f, 0}},
                {
                    {0.9, 0.3, 0, 1},
                    {0.1, 0.2, 0.7, 0.3},
                    {0.9, 0.2, 0.7, 0.3},
                    {0.3, 0.7, 0.5f, 0.5},
                },
                {{1, 0}, {0, 0}, {0, 1}, {1, 1}},
                tex_id);

    Polygon gon2(main_shader, {{-0.8, -0.3, -0.1}, {-0.2, 0.7, 0}, {0.5, -0.5, 0}},
                 {
                     {0.9, 0.3, 0, 1},
                     {0.9, 0.2, 0.7, 0.3},
                     {0.3, 0.7, 0.5f, 0.5},
                 });

    Polygon gon3(main_shader, {{0.4, 0.3, 0}, {0.5, 0.3, 0}, {0.5, 0.4, 0}, {0.45, 0.35, 0}, {0.4, 0.4, 0}},
                 {
                     {0.9, 0.3, 0, 1},
                     {0.9, 0.2, 0.7, 0.3},
                     {0.9, 0.2, 0.7, 0.3},
                     {0.9, 0.2, 0.7, 0.3},
                     {0.3, 0.7, 0.5f, 0.5},
                 });

    Polygon poly(main_shader, {{-50, -50, 0}, {-50, 50, 0}, {50, 50, 0}, {50, -50, 0}}, {{0.3, 0.7, 0.1, 0.5}});

    auto &ins3 = main_world.append_child<Shape>(gon3);
    auto &ins = ins3.append_child<Shape>(gon);
    auto &ins2 = main_world.append_child<Shape>(gon2);
    auto &inspoly = main_world.append_child<Shape>(poly);
    ins3.scale = 1000;
    ins2.scale = 1500;
    inspoly.set_position({0, 100, 100});

    vector<std::unique_ptr<Shape>> poly_instances;
    for (int i = -500; i <= 500; i += 100) {
        auto &instance = main_world.append_child<Shape>(poly);
        instance.set_position({-200, -50, i});
    }
    for (int i = -300; i <= 300; i += 100) {
        auto &instance = main_world.append_child<Shape>(poly);
        instance.set_position({-0, 300, i});
    }

    // main_world.timer.task(1, [&] {
    //     debug(ins3.get_absolute_transform());
    // });

    const vector<glm::vec3> vertices = {
        glm::vec3(0, -900, 500),
        glm::vec3(500, 0, 0),
        glm::vec3(0, 500, 0),
        glm::vec3(-500, 0, 0),
    };
    const vector<RGBA> colors = {
        {0.1, 0.4, 0.7, 0.5},
        {0.1, 0.4, 0.7, 1},
        {0.1, 0.4, 0.7, 0.5},
        {0.1, 0.4, 0.7, 1},
    };

    // const auto &shader = hello_shader;
    auto vbo = VertexBufferObject::gen(sizeof(glm::vec3) * 4, vertices.data(), GL_STATIC_DRAW);
    auto vbo_color = VertexBufferObject::gen(sizeof(RGBA) * 4, colors.data(), GL_STATIC_DRAW);

    // VAOを作成。頂点の座標と色、uvを関連付ける
    auto vao = VertexArrayObject::gen();
    vao.bind([&] {
        vbo.bind([&] {
            main_shader.set_attribute("position", 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr); // 位置
        });
        vbo_color.bind([&] {
            main_shader.set_attribute("color", 4, GL_FLOAT, GL_FALSE, sizeof(RGBA), nullptr); // 位置
        });
        getErrors();
    });

    auto &line = main_world.append_child<DynamicArray>(&main_shader, std::vector{glm::vec3{0, 0, 0}}, std::vector{RGBA{200, 200, 200, 255}});
    main_world.timer.task(0.5, [&line] {
        line.set_position(line.get_position() + glm::vec3{-10, 0, 0});
    });

    main_world.draws.set_function([&](const auto &camera) {
        main_shader.use();
        main_shader.set_uniform("modelViewMatrix", camera.get_view_matrix());
        main_shader.set_uniform("is_tex", GL_FALSE);
        vao.bind([&] {
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        });
    });

    auto &mesh = main_world.append_child<Mesh>();
    // mesh.vertices.xs = {1, 0, 0, 8};
    // mesh.vertices.ys = {1, 2, 3, -1};
    mesh.indices = {0, 1, 2, 3, 7, 1, 5, 0, 4, 2, 6, 7, 4, 5};
    mesh.vertices.coords = {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0}, {0, 0, 1}, {1, 0, 1}, {0, 1, 1}, {1, 1, 1}};
    // mesh.vertices.coords = {{0, 0, 0}, {1, 0, 0}, {0, 0, 1}, {1, 0, 1}, {0, 1, 0}, {1, 1, 0}, {0, 1, 1}, {1, 1, 1}};
    mesh.vertices.colors = std::vector(8, RGBA{0.46, 0.85, 0.84, 0.9});
    mesh.draw_mode = GL_LINE_STRIP;
    mesh.draw_mode = GL_TRIANGLE_STRIP;
    mesh.line_width = 10;
    mesh.scale = 200;
    mesh.position = {100, 100, 500};

    // レンダリングループ
    gui.mainloop();
}
