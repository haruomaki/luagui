#include <gui.hpp>

int main() {
    GUI gui;
    Window window(gui, 600, 500, "blank");
    World &main_world = gui.create_world();

    window.set_input_mode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    auto &camera = mobile_normal_camera(main_world.root, window);
    auto &cobj = *camera.owner().get_parent();
    cobj.position = {0, 0, 10};
    cobj.rotate = ANGLE_Y(M_PIf);
    cobj.scale = 10;

    auto &cube = new_mesh(main_world.root);
    auto &mesh = cube.mesh;
    // mesh.vertices.xs = {1, 0, 0, 8};
    // mesh.vertices.ys = {1, 2, 3, -1};
    mesh.indices = {0, 1, 2, 3, 7, 1, 5, 0, 4, 2, 6, 7, 4, 5};
    mesh.vertices.coords = {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0}, {0, 0, 1}, {1, 0, 1}, {0, 1, 1}, {1, 1, 1}};
    // mesh.vertices.coords = {{0, 0, 0}, {1, 0, 0}, {0, 0, 1}, {1, 0, 1}, {0, 1, 0}, {1, 1, 0}, {0, 1, 1}, {1, 1, 1}};
    mesh.vertices.colors = std::vector(8, RGBA{.r = 0.46, .g = 0.85, .b = 0.84, .a = 0.9});
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

    main_world.root.append_child<GridGround>();

    cube.owner().add_component<Collider>();
    cube.owner().add_component<Rigidbody>();

    // レンダリングループ
    gui.mainloop();
    // TODO: Windowの開放タイミングのバグ調査
    print("blankおわり");
}
