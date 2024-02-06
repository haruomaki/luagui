#include <Timer.hpp>
#include <master.hpp>
#include <utility.hpp>

inline float f(float x) {
    return 3 * std::sin(x);
}

int main() {
    std::function<void()> waa = [] {};
    std::function<void()> pen = [] {};
    std::function<void()> mage = [] {};
    std::set<std::function<void()> *> sss = {&waa, &pen, &mage};
    // sss.erase(9);
    print(sss);
    sss.erase(&pen);
    print(sss);
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

    World &world = window.create_world();
    World &ui_world = window.create_world();

    ProgramObject main_shader = {
        create_shader(GL_VERTEX_SHADER, load_string("assets/shaders/shader.vsh")),
        create_shader(GL_FRAGMENT_SHADER, load_string("assets/shaders/shader.fsh"))};

    auto &camera = world.append_child<MobileOrthoCamera>(viewport);
    auto &ui_camera = ui_world.append_child<OrthoCamera>(viewport);
    // camera.setScale(0.01F);
    // camera.setScale(100);
    camera.set_active();
    ui_camera.set_active();

    auto &line = world.append_child<DynamicArray>(main_shader);
    line.draw_mode = GL_POINTS;
    line.scale = 100;

    constexpr int points_num = 100;
    line.vertices.colors = vector<RGBA>(points_num, {0.5, 0.2, 0.7, 1.0});

    // 左上に常在する点
    // TODO: これが間違ってworldの子になってもメモリエラー？回答：前と後ろでui_worldとworldのようにチグハグに指定するとエラー
    auto &top_left_point = ui_world.append_child<StickyPointTopLeft>(viewport);

    // 文字の表示
    Font migmix_font;
    auto &sample_text = top_left_point.append_child<Text>(migmix_font, "This is sample text 123456789", RGBA{0.5, 0.8, 0.2, 0.4});
    /* auto &credit_text =*/ui_world.append_child<Text>(migmix_font, "(C) LearnOpenGL.com", RGBA{0.3, 0.7, 0.9, 0.4});
    sample_text.position = {20, -60, 0};

    // 三角形の表示
    Polygon my_triangle_polygon(main_shader, {{-0.8, -0.3, 0}, {-0.2, 0.7, 0}, {0.5, -0.5, 0}},
                                {
                                    {0.9, 0.3, 0, 1},
                                    {0.9, 0.2, 0.7, 0.3},
                                    {0.3, 0.7, 0.5, 0.5},
                                });
    auto &my_triangle = world.append_child<Shape>(my_triangle_polygon);
    my_triangle.scale = 200;
    my_triangle.position = {-100, 0, 0};

    gui.timer.task(1, [] { print("タイマーからこんにちは😚"); });

    // レンダリングループ
    gui.mainloop([&] {
        sample_text.text_ = to_str(gui.tick);

        const auto xs = linspace(-9, 9, points_num);
        line.vertices.xs = xs;
        line.vertices.ys = map(xs, [&](auto x) { return f(x + float(gui.tick) / 100); });
    });
}
