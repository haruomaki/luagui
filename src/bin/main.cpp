#include <master.hpp>
#include <utility.hpp>

class MovingShape : public Shape, Update {
    static constexpr float speed = 1.5;
    float velocity_ = -speed;

    void update() override {
        auto next_pos = getPosition() + glm::vec3(0, velocity_, 0);
        setPosition(next_pos);
        if (next_pos.y < -300 || 300 < next_pos.y) {
            velocity_ *= -1.1f;
        }
    }

  public:
    MovingShape(Window &window, World &world, Polygon polygon)
        : Shape(world, polygon)
        , Update(window) {}
};

int main() {
    Window window(600, 500);
    MaximumViewport viewport(window);
    World main_world, ui_world;

    // バーテックスシェーダのコンパイル
    auto vsh_string = loadString("assets/shaders/shader.vsh");
    auto vsh_id = create_shader(GL_VERTEX_SHADER, vsh_string);

    // フラグメントシェーダのコンパイル
    auto fsh_string = loadString("assets/shaders/shader.fsh");
    auto fsh_id = create_shader(GL_FRAGMENT_SHADER, fsh_string);

    auto main_shader = ProgramObject{vsh_id, fsh_id};

    MobileNormalCamera camera(window, main_world, viewport);
    camera.position = {0, 0, 1000};
    camera.rotate = ANGLE_Y(M_PIf);
    camera.scale = 1;

    Font migmix_font;
    Text sample_text(main_world, migmix_font, "This is sample text 123456789", {0.5, 0.8, 0.2, 0.4});
    Text credit_text(main_world, migmix_font, "(C) LearnOpenGL.com", {0.3, 0.7, 0.9, 0.4});

    sample_text.setPosition({-200, 50, 200});
    credit_text.setPosition({200, 400, 1});

    GLuint tex_id = loadTexture("assets/images/cat.raw");

    Polygon gon(main_shader, {{0.9f, 0.9f, 0.4}, {0.5f, 0.f, 0}, {0.f, 0.f, 0.2}, {0.f, 0.5f, 0}},
                {
                    {0.9, 0.3, 0, 1},
                    {0.1, 0.2, 0.7, 0.3},
                    {0.9, 0.2, 0.7, 0.3},
                    {0.3, 0.7, 0.5f, 0.5},
                },
                tex_id);

    Polygon gon2(main_shader, {{-0.8, -0.3, 0}, {-0.2, 0.7, 0}, {0.5, -0.5, 0}},
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

    MovingShape ins3(window, main_world, gon3);
    Shape ins(main_world, gon), ins2(main_world, gon2), inspoly(main_world, poly);
    ins3.append(ins);
    ins3.scale = 1000;
    ins2.scale = 1500;
    inspoly.setPosition({0, 100, 100});

    vector<std::unique_ptr<Shape>> poly_instances;
    for (int i = -500; i <= 500; i += 100) {
        auto instance = std::make_unique<Shape>(main_world, poly);
        instance->setPosition({-200, -50, i});
        poly_instances.push_back(std::move(instance));
    }
    for (int i = -300; i <= 300; i += 100) {
        auto instance = std::make_unique<Shape>(main_world, poly);
        instance->setPosition({-0, 300, i});
        poly_instances.push_back(std::move(instance));
    }

    setInterval(1000, [&] {
        debug(ins3.getAbsoluteTransform());
        return true;
    });

    ProgramObject hello_shader = {
        create_shader(GL_VERTEX_SHADER, loadString("assets/shaders/hello.vsh")),
        create_shader(GL_FRAGMENT_SHADER, loadString("assets/shaders/hello.fsh"))};

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

    DynamicArray line(window, main_world, main_shader, {{0, 0, 0}, {200, 200, 200}});
    setInterval(0.5F, [&line] {
        line.setPosition(line.getPosition() + glm::vec3{-10, 0, 0});
        return true;
    });

    // レンダリングループ
    window.mainloop([&] {
        main_world.masterDraw(camera);

        main_shader.use();
        main_shader.setUniform("modelViewMatrix", camera.getViewMatrix());
        main_shader.set_Uniform("is_tex", GL_FALSE);
        vao.bind([&] {
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        });
    });
}
