#include <master.hpp>
#include <utility.hpp>

int main() {
    GUI gui;
    Window &window = gui.create_window(600, 500, "井戸型ポテンシャル");
    auto &viewport = window.make_child<MaximumViewport>();
    World &main_world = window.create_world();

    // バーテックスシェーダのコンパイル
    auto vsh_string = load_string("assets/shaders/shader.vsh");
    auto vsh_id = create_shader(GL_VERTEX_SHADER, vsh_string);

    // フラグメントシェーダのコンパイル
    auto fsh_string = load_string("assets/shaders/shader.fsh");
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

    auto &line = main_world.append_child<DynamicArray>(main_shader);
    line.vertices.xs = linspace(-200, 200, 10);
    line.vertices.colors = vector<RGBA>(10, {0.5, 0.7, 0.1, 1});
    main_world.timer.task(0.5, [&line] {
        line.set_position(line.get_position() + glm::vec3{-10, 0, 0});
    });

    gui.mainloop();
}
