// FIXME: 正確な寸法に修正したのに合わせ、文字サイズ要再調整。

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

    World &world = window.create_world();
    World &ui_world = window.create_world();

    auto &camera = world.append_child<MobileOrthoCamera>();
    auto &ui_camera = ui_world.append_child<OrthoCamera>();
    // camera.setScale(0.01F);
    // camera.setScale(100);
    camera.set_active();
    ui_camera.set_active();

    auto &line = new_points(world);
    auto &line_mesh = line.mesh;
    line.scale = 0.03;
    int points_num = 100;

    // 左上に常在する点
    // これが間違ってworldの子になってもメモリエラー？回答：前と後ろでui_worldとworldのようにチグハグに指定するとエラー
    auto &top_left_point = ui_world.append_child<StickyPointTopLeft>();

    // 文字の表示
    Font migmix_font;
    auto &sample_text = top_left_point.append_child<Text>(migmix_font, "This is sample text 123456789", RGBA{0.5, 0.8, 0.2, 0.4});
    /* auto &credit_text =*/ui_world.append_child<Text>(migmix_font, "(C) LearnOpenGL.com", RGBA{0.3, 0.7, 0.9, 0.4});
    sample_text.position = {0.005, -0.02, 0};

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

    float t = 0;
    world.timer.task(0.5, [&] {
        t += 0.1;
        points_num = (points_num - 90) % 200 + 100; // 100〜300を繰り返す
    });

    auto proc = std::make_unique<std::function<void()>>([&] {
        sample_text.text_ = to_str(gui.tick);

        const auto xs = linspace(-9, 9, points_num);
        line_mesh.vertices.clear();
        line_mesh.vertices.xs = xs;
        line_mesh.vertices.ys = map(xs, [&](auto x) { return f(x + float(gui.tick) / 100) + t; });
        line_mesh.vertices.colors = vector<RGBA>(points_num, {0.5, 0.2, 0.7, 1.0});
    });
    world.updates.request_set(proc.get());

    gui.mainloop();
}
