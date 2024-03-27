#include "master.hpp"
#include "utility.hpp"

int main() {
    GUI gui;
    Window &window = gui.create_window(1280, 720, "blank");
    auto &viewport = window.append_resource<MaximumViewport>();
    World &main_world = window.create_world();

    glfwSetInputMode(window.get_glfw(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    auto &camera = main_world.append_child<MobileNormalCamera>(viewport);
    camera.position = {0, 0, 10};
    camera.rotate = ANGLE_Y(M_PIf);
    camera.scale = 1;
    camera.set_active();

    auto &mesh = main_world.append_child<MeshOld>();
    // mesh.vertices.xs = {1, 0, 0, 8};
    // mesh.vertices.ys = {1, 2, 3, -1};
    mesh.indices = {0, 1, 2, 3, 7, 1, 5, 0, 4, 2, 6, 7, 4, 5};
    mesh.vertices.coords = {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0}, {0, 0, 1}, {1, 0, 1}, {0, 1, 1}, {1, 1, 1}};
    // mesh.vertices.coords = {{0, 0, 0}, {1, 0, 0}, {0, 0, 1}, {1, 0, 1}, {0, 1, 0}, {1, 1, 0}, {0, 1, 1}, {1, 1, 1}};
    mesh.vertices.colors = std::vector(8, RGBA{0.46, 0.85, 0.84, 0.9});
    std::vector<RGBA> colors_tmp = {};
    for (int i = 0; i < 8; i++) {
        auto r = float(pow(sin(0.4 * i + 1), 2));
        auto g = float(pow(sin(0.4 * i + 2), 2));
        auto b = float(pow(sin(0.4 * i + 3), 2));
        colors_tmp.emplace_back(r, g, b, 1);
    }
    mesh.vertices.colors = colors_tmp;
    mesh.draw_mode = GL_LINE_STRIP;
    // mesh.draw_mode = GL_TRIANGLE_STRIP;
    mesh.line_width = 5;
    mesh.scale = 2;
    mesh.position = {1, 1, 5};

    main_world.append_child<GridGround>();

    // レンダリングループ
    gui.mainloop();
}
