#include <gui.hpp>

int main() {
    GL::Context gui;
    Window window(gui, 1280, 720, "blank");
    World &main_world = window.create_world();

    glfwSetInputMode(window.gwin, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    auto &camera = mobile_normal_camera(main_world);
    auto &cobj = *camera.owner().get_parent();
    cobj.position = {0, 0, 10};
    cobj.rotate = ANGLE_Y(M_PIf);
    cobj.scale = 10;
    main_world.active_camera() = &camera;

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
    cube.owner().scale = 2;
    cube.owner().position = {1, 1, 5};

    main_world.append_child<GridGround>();

    // レンダリングループ
    gui.mainloop();
    // TODO: Windowの開放タイミングのバグ調査
    print("blankおわり");
}
