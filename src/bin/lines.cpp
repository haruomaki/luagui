// FIXME: 正確な寸法に修正したのに合わせ、文字サイズ要再調整。

#include <gui.hpp>

inline float f(float x) {
    return 3 * std::sin(x);
}

int main() {
    constexpr int width = 600, height = 500;
    GL::Context gui;
    Window window(gui, width, height, "ウィンドウタイトル");
    // Window &another_window = gui.create_window(width, height, "２つめのウィンドウ");
    // auto &viewport = window.registerSizeCallback(MaximumViewport()); // これだと一度リサイズしないと画面が出ない

    World &world = window.create_world();
    World &ui_world = window.create_world();

    auto &camera = mobile_ortho_camera(world);
    auto &ui_camera_obj = ui_world.append_child<WorldObject>();
    auto &ui_camera = ui_camera_obj.add_component<Camera>(Camera::Orthographic);
    // camera.setScale(0.01F);
    // camera.setScale(100);
    world.rendering_camera() = &camera;
    ui_world.rendering_camera() = &ui_camera;
    ui_camera.mode = Camera::TopLeft;

    auto &line = new_points(world);
    auto &line_mesh = line.mesh;
    line.owner().scale = 0.03;
    int points_num = 100;

    // 文字の表示
    auto &migmix_font = window.append_resource<Font>();
    auto &sample_text = ui_world.child_component<Text>(migmix_font, "This is sample text 123456789", RGBA{0.5, 0.8, 0.2, 0.4});
    auto &credit_text = ui_world.child_component<Text>(migmix_font, "(C) LearnOpenGL.com", RGBA{0.3, 0.7, 0.9, 0.4});
    sample_text.owner().position = {0.005, -0.02, 0};
    credit_text.owner().position = {0.1, -0.1, 0};

    // 三角形の表示
    auto &my_triangle_mesh = new_mesh(window, GL_TRIANGLE_FAN, {{-0.8, -0.3, 0}, {-0.2, 0.7, 0}, {0.5, -0.5, 0}},
                                      {
                                          {0.9, 0.3, 0, 1},
                                          {0.9, 0.2, 0.7, 0.3},
                                          {0.3, 0.7, 0.5, 0.5},
                                      });
    auto &my_triangle = world.child_component<MeshComponent>(my_triangle_mesh);
    my_triangle.owner().scale = 0.1;
    my_triangle.owner().position = {-0.1, 0, 0};

    float t = 0;
    world.timer.task(0.5, [&] {
        t += 0.1;
        points_num = (points_num - 90) % 200 + 100; // 100〜300を繰り返す
    });

    auto proc = std::make_unique<std::function<void()>>([&] {
        sample_text.text = to_str(gui.tick());

        const auto xs = linspace(-9, 9, points_num);
        line_mesh.vertices.clear();
        line_mesh.vertices.xs = xs;
        line_mesh.vertices.ys = map(xs, [&](auto x) { return f(x + float(gui.tick()) / 100) + t; });
        line_mesh.vertices.colors = vector<RGBA>(points_num, {0.5, 0.2, 0.7, 1.0});
    });
    world.updates.request_set(proc.get());

    gui.mainloop();
}
