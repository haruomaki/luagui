#include <Timer.hpp>
#include <master.hpp>
#include <utility.hpp>

inline float f(float x) {
    return 3 * std::sin(x);
}

int main() {
    constexpr int width = 600, height = 500;
    GUI gui;
    Window &window = gui.create_window(width, height, "ウィンドウタイトル");
    // Window &another_window = gui.create_window(width, height, "２つめのウィンドウ");
    // auto &viewport = window.registerSizeCallback(MaximumViewport()); // これだと一度リサイズしないと画面が出ない
    auto &viewport = window.make_child<MaximumViewport>();

    World &world = window.create_world();
    World &ui_world = window.create_world();

    ProgramObject main_shader = {
        create_shader(GL_VERTEX_SHADER, load_string("assets/shaders/default.vsh")),
        create_shader(GL_FRAGMENT_SHADER, load_string("assets/shaders/default.fsh"))};

    auto &camera = world.append_child<MobileOrthoCamera>(viewport);
    auto &ui_camera = ui_world.append_child<OrthoCamera>(viewport);
    // camera.setScale(0.01F);
    // camera.setScale(100);
    camera.set_active();
    ui_camera.set_active();

    auto &line = world.append_child<DynamicArray>();
    line.draw_mode = GL_POINTS;
    line.scale = 0.03;
    int points_num = 100;

    // 左上に常在する点
    // TODO: これが間違ってworldの子になってもメモリエラー？回答：前と後ろでui_worldとworldのようにチグハグに指定するとエラー
    auto &top_left_point = ui_world.append_child<StickyPointTopLeft>(viewport);

    // 文字の表示
    Font migmix_font;
    auto &sample_text = top_left_point.append_child<Text>(migmix_font, "This is sample text 123456789", RGBA{0.5, 0.8, 0.2, 0.4});
    /* auto &credit_text =*/ui_world.append_child<Text>(migmix_font, "(C) LearnOpenGL.com", RGBA{0.3, 0.7, 0.9, 0.4});
    sample_text.position = {0.005, -0.02, 0};

    // 三角形の表示
    Polygon my_triangle_polygon(main_shader, {{-0.8, -0.3, 0}, {-0.2, 0.7, 0}, {0.5, -0.5, 0}},
                                {
                                    {0.9, 0.3, 0, 1},
                                    {0.9, 0.2, 0.7, 0.3},
                                    {0.3, 0.7, 0.5, 0.5},
                                });
    auto &my_triangle = world.append_child<Shape>(my_triangle_polygon);
    my_triangle.scale = 0.1;
    my_triangle.position = {-0.1, 0, 0};

    float t = 0;
    world.timer.task(0.5, [&] {
        t += 0.1;
        points_num = (points_num - 90) % 200 + 100; // 100〜300を繰り返す
    });

    world.updates.set_function([&] {
        sample_text.text_ = to_str(gui.tick);

        const auto xs = linspace(-9, 9, points_num);
        line.vertices.clear();
        line.vertices.xs = xs;
        line.vertices.ys = map(xs, [&](auto x) { return f(x + float(gui.tick) / 100) + t; });
        line.vertices.colors = vector<RGBA>(points_num, {0.5, 0.2, 0.7, 1.0});
    });

    gui.mainloop();
}
