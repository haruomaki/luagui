#include <master.hpp>
#include <utility.hpp>

int main() {
    GUI gui;
    Window &window = gui.create_window(1280, 720, "blank");
    auto &viewport = window.make_child<MaximumViewport>();
    World &main_world = window.create_world();

    auto &camera = main_world.append_child<MobileNormalCamera>(viewport);
    camera.position = {0, 0, 10};
    camera.rotate = ANGLE_Y(M_PIf);
    camera.scale = 1;
    camera.set_active();

    auto &mesh = main_world.append_child<Mesh>();
    // mesh.vertices.xs = {1, 0, 0, 8};
    // mesh.vertices.ys = {1, 2, 3, -1};
    mesh.indices = {0, 1, 2, 3, 7, 1, 5, 0, 4, 2, 6, 7, 4, 5};
    mesh.vertices.coords = {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0}, {0, 0, 1}, {1, 0, 1}, {0, 1, 1}, {1, 1, 1}};
    // mesh.vertices.coords = {{0, 0, 0}, {1, 0, 0}, {0, 0, 1}, {1, 0, 1}, {0, 1, 0}, {1, 1, 0}, {0, 1, 1}, {1, 1, 1}};
    mesh.vertices.colors = std::vector(8, RGBA{0.46, 0.85, 0.84, 0.9});
    mesh.draw_mode = GL_LINE_STRIP;
    mesh.draw_mode = GL_TRIANGLE_STRIP;
    mesh.line_width = 10;
    mesh.scale = 2;
    mesh.position = {1, 1, 5};

    auto &grid = main_world.append_child<DynamicArray>();
    // grid.indices = {0, 1, 2, 3, 4, 5};
    for (int i = -10; i <= 10; i++) {
        constexpr RGBA grid_color = {0.1, 0.1, 0.1, 1};
        grid.vertices.push_back(InterleavedVertexInfo{{i, 0, -10}, grid_color});
        grid.vertices.push_back(InterleavedVertexInfo{{i, 0, 10}, grid_color});
        grid.vertices.push_back(InterleavedVertexInfo{{-10, 0, i}, grid_color});
        grid.vertices.push_back(InterleavedVertexInfo{{10, 0, i}, grid_color});
    }
    grid.draw_mode = GL_LINES;
    grid.line_width = 6;
    grid.scale = 1;

    // レンダリングループ
    gui.mainloop();
}
