#include "utility.hpp"
#include <master.hpp>

class GlassBall : public MeshObject, Update {
    static Material &gen_material(Window &window) {
        GLuint tex = create_texture_from_png_file("assets/images/青いガラス玉.png");
        return MaterialBuilder().texture(tex).build(window);
    }

    static StaticMesh &gen_mesh(Window &window) {
        auto &mesh = window.append_resource<StaticMesh>();
        mesh.vertices.coords = {{0, 0, 0}, {0.02, 0, 0}, {0.02, 0.02, 0}, {0, 0.02, 0}};
        mesh.vertices.uvs = {{0, 1}, {1, 1}, {1, 0}, {0, 0}};
        mesh.draw_mode = GL_TRIANGLE_FAN;
        mesh.sync_vram();
        return mesh;
    }

  public:
    GlassBall(Window &window)
        : MeshObject(gen_mesh(window), &gen_material(window)) {}

    double t = 0;
    const double cycle = 30;

    void update() override {
        position = glm::vec3{0.1 * cos(t / cycle), 0.1 * sin(t / cycle), 0.1};
        t++;
    }
};

static StaticMesh &create_brick_mesh(Window &window) {
    auto &brick_mesh = window.append_resource<StaticMesh>();
    brick_mesh.vertices.coords = {{0, 0, 0}, {0.02, 0, 0}, {0.02, 0.01, 0}, {0, 0.01, 0}};
    brick_mesh.vertices.uvs = {{0, 1}, {1, 1}, {1, 0}, {0, 0}};
    brick_mesh.draw_mode = GL_TRIANGLE_FAN;
    brick_mesh.sync_vram();
    return brick_mesh;
}

int main() {
    constexpr int width = 600, height = 500;
    GUI gui;
    Window &window = gui.create_window(width, height, "ウィンドウタイトル");
    World &world = window.create_world();
    auto &camera = world.append_child<MobileOrthoCamera>();
    // auto &camera = world.append_child<MobileNormalCamera>();
    // camera.rotate = ANGLE_Y(M_PIf);
    camera.set_active();

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

    // レンガの表示
    GLuint brick_texture = create_texture_from_png_file("assets/images/ピンクレンガ.png");
    auto &brick_material = MaterialBuilder().texture(brick_texture).build(window);
    auto &brick_mesh = create_brick_mesh(window);

    constexpr int num_x = 10, num_y = 100;
    auto bricks = std::vector(num_x, std::vector<MeshObject *>(num_y));
    for (int x = 0; x < num_x; x++) {
        for (int y = 0; y < num_y; y++) {
            bricks[x][y] = &world.append_child<MeshObject>(brick_mesh, &brick_material);
            bricks[x][y]->position = {0.02 * x, 0.01 * y, 0};
        }
    }

    auto &ball = world.append_child<GlassBall>(window);

    gui.mainloop();
}
