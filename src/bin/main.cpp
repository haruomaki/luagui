#include <SumiGL/Texture.hpp>
#include <gui.hpp>
#include <lunchbox.hpp>

int main() {
    lunchbox::Storage storage;
    GUI gui;
    Window window(gui, 600, 500, "めいん");
    World &main_world = gui.create_world();

    auto &camera = mobile_normal_camera(main_world, window);
    auto &cobj = *camera.owner().get_parent();
    cobj.position = {0, 0, 1000};
    cobj.rotate = ANGLE_Y(M_PIf);
    cobj.scale = 2000; // 速度調整

    auto text_shader = GL::ProgramObject{
        GL::create_shader(GL_VERTEX_SHADER, storage.get_text("assets/shaders/font.vsh")),
        GL::create_shader(GL_FRAGMENT_SHADER, storage.get_text("assets/shaders/font.fsh"))};

    auto font = storage.get_font("assets/fonts/main.ttf");
    auto &migmix_font = gui.resources.append<Font>(std::move(text_shader), font).get();
    auto &sample_text = main_world.child_component<Text>(migmix_font, "This is sample text 123456789", RGBA{0.5, 0.8, 0.2, 0.4});
    auto &credit_text = main_world.child_component<Text>(migmix_font, "(C) LearnOpenGL.com", RGBA{0.3, 0.7, 0.9, 0.4});

    sample_text.owner().set_position({-200, 50, 200});
    sample_text.owner().set_scale_one(2000);
    credit_text.owner().set_position({200, 400, 1});
    credit_text.owner().set_scale_one(2000);

    // GLuint cat_texture = load_texture("assets/images/cat.raw");
    auto img = storage.get_image("assets/images/ピンクレンガ.png");
    auto cat_texture = GL::create_texture(img.width, img.height, img.channels, img.pixels.get());
    Material &cat_material = MaterialBuilder().texture(cat_texture.get()).build(gui);

    auto &gon = new_mesh(gui, GL_TRIANGLE_FAN, {{0.9f, 0.9f, 0.4}, {0.5f, 0.f, 0}, {0.f, 0.f, 0.2}, {0.f, 0.5f, 0}},
                         {
                             {0.9, 0.3, 0, 1},
                             {0.1, 0.2, 0.7, 0.3},
                             {0.9, 0.2, 0.7, 0.3},
                             {0.3, 0.7, 0.5f, 0.5},
                         },
                         {{1, 0}, {0, 0}, {0, 1}, {1, 1}});

    auto &gon2 = new_mesh(gui, GL_TRIANGLE_FAN, {{-0.8, -0.3, -0.1}, {-0.2, 0.7, 0}, {0.5, -0.5, 0}},
                          {
                              {0.9, 0.3, 0, 1},
                              {0.9, 0.2, 0.7, 0.3},
                              {0.3, 0.7, 0.5f, 0.5},
                          });

    auto &gon3 = new_mesh(gui, GL_TRIANGLE_FAN, {{0.4, 0.3, 0}, {0.5, 0.3, 0}, {0.5, 0.4, 0}, {0.45, 0.35, 0}, {0.4, 0.4, 0}},
                          {
                              {0.9, 0.3, 0, 1},
                              {0.9, 0.2, 0.7, 0.3},
                              {0.9, 0.2, 0.7, 0.3},
                              {0.9, 0.2, 0.7, 0.3},
                              {0.3, 0.7, 0.5f, 0.5},
                          });

    auto &poly = new_mesh(gui, GL_TRIANGLE_FAN, {{-50, -50, 0}, {-50, 50, 0}, {50, 50, 0}, {50, -50, 0}}, {{0.3, 0.7, 0.1, 0.5}});

    auto &ins3 = main_world.child_component<MeshComponent>(gon3);
    /* auto &ins = */ ins3.owner().child_component<MeshComponent>(gon, &cat_material);
    auto &ins2 = main_world.child_component<MeshComponent>(gon2);
    auto &inspoly = main_world.child_component<MeshComponent>(poly);
    ins3.owner().scale = 1000;
    ins2.owner().scale = 1500;
    inspoly.owner().set_position({0, 100, 100});

    for (int i = -500; i <= 500; i += 100) {
        auto &instance = main_world.child_component<MeshComponent>(poly);
        instance.owner().set_position({-200, -50, i});
    }
    for (int i = -300; i <= 300; i += 100) {
        auto &instance = main_world.child_component<MeshComponent>(poly);
        instance.owner().set_position({-0, 300, i});
    }

    // main_world.timer.task(1, [&] {
    //     debug(ins3.get_absolute_transform());
    // });

    const vector<glm::vec3> vertices = {
        glm::vec3(0, -900, 500),
        glm::vec3(500, 0, 0),
        glm::vec3(0, 500, 0),
        glm::vec3(-500, 0, 0),
    };
    const vector<RGBA> colors = {
        {0.1, 0.4, 0.7, 0.5},
        {0.1, 0.4, 0.7, 1},
        {0.1, 0.4, 0.7, 0.5},
        {0.1, 0.4, 0.7, 1},
    };

    auto &cube = new_mesh(main_world);
    auto &mesh = cube.mesh;
    // mesh.vertices.xs = {1, 0, 0, 8};
    // mesh.vertices.ys = {1, 2, 3, -1};
    mesh.indices = {0, 1, 2, 3, 7, 1, 5, 0, 4, 2, 6, 7, 4, 5};
    mesh.vertices.coords = {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0}, {0, 0, 1}, {1, 0, 1}, {0, 1, 1}, {1, 1, 1}};
    // mesh.vertices.coords = {{0, 0, 0}, {1, 0, 0}, {0, 0, 1}, {1, 0, 1}, {0, 1, 0}, {1, 1, 0}, {0, 1, 1}, {1, 1, 1}};
    mesh.vertices.colors = std::vector(8, RGBA{0.46, 0.85, 0.84, 0.9});
    mesh.draw_mode = GL_LINE_STRIP;
    mesh.draw_mode = GL_TRIANGLE_STRIP;
    cube.material.line_width = 10;
    cube.owner().scale = 200;
    cube.owner().position = {100, 100, 500};

    auto &grid = new_line(main_world);
    // grid.indices = {0, 1, 2, 3, 4, 5};
    for (int i = -10; i <= 10; i++) {
        constexpr RGBA grid_color = {0.1, 0.1, 0.1, 1};
        grid.mesh.vertices.push_back(InterleavedVertexInfo{{i, 0, -10}, grid_color});
        grid.mesh.vertices.push_back(InterleavedVertexInfo{{i, 0, 10}, grid_color});
        grid.mesh.vertices.push_back(InterleavedVertexInfo{{-10, 0, i}, grid_color});
        grid.mesh.vertices.push_back(InterleavedVertexInfo{{10, 0, i}, grid_color});
    }
    grid.mesh.draw_mode = GL_LINES;
    grid.material.line_width = 6;
    grid.owner().scale = 100;

    // レンダリングループ
    gui.mainloop();
}
