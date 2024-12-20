#include "utility.hpp"
#include <master.hpp>

class GlassBall : public MeshComponent {
    Material &gen_material() {
        auto &window = this->get_world().window;
        GLuint tex = create_texture_from_png_file("assets/images/青いガラス玉.png");
        return MaterialBuilder().texture(tex).build(window);
    }

    StaticMesh &gen_mesh() {
        auto &window = this->get_world().window;
        auto &mesh = window.append_resource<StaticMesh>();
        mesh.use_index = false;
        mesh.vertices.coords = {{0, 0, 0}, {0.02, 0, 0}, {0.02, 0.02, 0}, {0, 0.02, 0}};
        mesh.vertices.uvs = {{0, 1}, {1, 1}, {1, 0}, {0, 0}};
        mesh.draw_mode = GL_TRIANGLE_FAN;
        mesh.sync_vram();
        return mesh;
    }

  public:
    GlassBall()
        : MeshComponent(gen_mesh(), &gen_material()) {}
};

class Hiyoko : virtual public WorldObject {
};

template <class T>
class SparkEffect : public Update {
  public:
    int level;
    int lifetime;
    float speed = 0.01;
    SparkEffect(int level, int lifetime)
        : level(level)
        , lifetime(lifetime) {
        print("こんすたらくた", this->level, this->lifetime);
        this->append_child<T>();
        this->position = {-0.1, 0, -0.1};
    }

    void update() override {
        if (--lifetime <= 0) {
            this->erase();
        }
        debug(lifetime);
        // speed -= 0.001;
        // this->position += glm::vec3{0, speed, 0};
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
    // auto &camera = world.append_child<MobileOrthoCamera>();
    auto &camera = world.append_child<MobileNormalCamera>();
    camera.rotate = ANGLE_Y(M_PIf);
    camera.set_active();

    // 三角形の表示
    auto &my_triangle_mesh = new_mesh(window, GL_TRIANGLE_FAN, {{-0.8, -0.3, 0}, {-0.2, 0.7, 0}, {0.5, -0.5, 0}},
                                      {
                                          {0.9, 0.3, 0, 1},
                                          {0.9, 0.2, 0.7, 0.3},
                                          {0.3, 0.7, 0.5, 0.5},
                                      });
    auto &my_triangle = world.append_child<MeshComponent>(my_triangle_mesh);
    my_triangle.scale = 0.1;
    my_triangle.position = {-0.1, 0, 0};

    // レンガの表示
    GLuint brick_texture = create_texture_from_png_file("assets/images/ピンクレンガ.png");
    auto &brick_material = MaterialBuilder().texture(brick_texture).build(window);
    auto &brick_mesh = create_brick_mesh(window);

    constexpr int num_x = 10, num_y = 30;
    auto bricks = std::vector(num_x, std::vector<MeshObject *>(num_y));
    for (int x = 0; x < num_x; x++) {
        for (int y = 0; y < num_y; y++) {
            bricks[x][y] = &world.append_child<MeshComponent>(brick_mesh, &brick_material);
            bricks[x][y]->position = {0.02 * x, 0.01 * y, 0};
        }
    }

    world.append_child<SparkEffect<GlassBall>>(3, 60);
    world.append_child<Hiyoko>();
    // auto h = Hiyoko();
    // Hiyoko g(h);
    // world.append_child<SparkEffect<WorldObject>>(3, 60);

    gui.mainloop();
}
