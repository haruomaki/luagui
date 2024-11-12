#include <GUI/master.hpp>

#include "GUI/utility.hpp"

int main() {
    GUI gui;
    Window &window = gui.create_window(1280, 720, "blank");
    World &main_world = window.create_world();

    glfwSetInputMode(window.glfw(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    auto &camera = main_world.append_child<MobileNormalCamera>();
    camera.position = {0, 0, 10};
    camera.rotate = ANGLE_Y(M_PIf);
    camera.scale = 10;
    camera.set_active();

    auto &cube = new_mesh(main_world);
    auto &mesh = cube.mesh;
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
    // cube.draw_mode = GL_LINE_STRIP;
    mesh.draw_mode = GL_TRIANGLE_STRIP;
    cube.material.line_width = 5;
    cube.scale = 2;
    cube.position = {1, 1, 5};

    main_world.append_child<GridGround>();

    // レンダリングループ
    gui.mainloop();
}
