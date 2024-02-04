#include <Timer.hpp>
#include <master.hpp>
#include <utility.hpp>

inline float f(float x) {
    return 3 * std::sin(x);
}

int main() {
    // Timer timer;
    // auto id_hi = timer.task(1, [] { print("hi!"); });
    // int count = 0;
    // timer.task(1. / 4, [&] {
    //     print("あいう");
    //     if (count++ >= 5) {
    //         timer.erase(id_hi);
    //         if (count >= 10) {
    //             timer.stop();
    //         }
    //     }
    // });
    // timer.start();

    constexpr int width = 600, height = 500;
    GUI gui;
    Window &window = gui.create_window(width, height, "ウィンドウタイトル");
    // Window &another_window = gui.create_window(width, height, "２つめのウィンドウ");
    // auto &viewport = window.registerSizeCallback(MaximumViewport()); // これだと一度リサイズしないと画面が出ない
    auto &viewport = window.make_child<MaximumViewport>();

    World world, ui_world;

    ProgramObject main_shader = {
        create_shader(GL_VERTEX_SHADER, load_string("assets/shaders/shader.vsh")),
        create_shader(GL_FRAGMENT_SHADER, load_string("assets/shaders/shader.fsh"))};

    MobileOrthoCamera camera(window, world, viewport);
    OrthoCamera ui_camera(ui_world, viewport);
    // camera.setScale(0.01F);
    // camera.setScale(100);

    DynamicArray line(window, world, main_shader, {}, {});
    line.draw_mode = GL_POINTS;
    line.scale = 100;

    constexpr int points_num = 100;
    line.vertices.colors = vector<RGBA>(points_num, {0.5, 0.2, 0.7, 1.0});

    // 左上に常在する点
    StickyPointTopLeft top_left_point(window, world, viewport);

    // 文字の表示
    Font migmix_font;
    Text sample_text(ui_world, migmix_font, "This is sample text 123456789", {0.5, 0.8, 0.2, 0.4});
    Text credit_text(ui_world, migmix_font, "(C) LearnOpenGL.com", {0.3, 0.7, 0.9, 0.4});
    top_left_point.append(sample_text);
    sample_text.position = {20, -60, 0};

    // 三角形の表示
    Polygon my_triangle_polygon(main_shader, {{-0.8, -0.3, 0}, {-0.2, 0.7, 0}, {0.5, -0.5, 0}},
                                {
                                    {0.9, 0.3, 0, 1},
                                    {0.9, 0.2, 0.7, 0.3},
                                    {0.3, 0.7, 0.5, 0.5},
                                });
    Shape my_triangle(world, my_triangle_polygon);
    my_triangle.scale = 200;
    my_triangle.position = {-100, 0, 0};

    // レンダリングループ
    gui.mainloop([&] {
        sample_text.text_ = to_str(gui.tick);

        const auto xs = linspace(-9, 9, points_num);
        line.vertices.xs = xs;
        line.vertices.ys = map(xs, [&](auto x) { return f(x + float(gui.tick) / 100); });

        world.masterDraw(camera);
        glClear(GL_DEPTH_BUFFER_BIT);
        ui_world.masterDraw(ui_camera);
    });
}
