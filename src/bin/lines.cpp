#include <master.hpp>
#include <utility.hpp>

static float f(float x) {
    return 3 * sin(x);
}

int main() {
    constexpr int width = 600, height = 500;
    Window window(width, height);
    MaximumViewport viewport(window);

    World world, ui_world;

    ProgramObject main_shader = {
        createShader(GL_VERTEX_SHADER, loadString("assets/shaders/shader.vsh")),
        createShader(GL_FRAGMENT_SHADER, loadString("assets/shaders/shader.fsh"))};

    MobileOrthoCamera camera(window, world, viewport);
    OrthoCamera ui_camera(ui_world, viewport);
    // camera.setScale(0.01F);
    // camera.setScale(100);

    DynamicArray line(window, world, main_shader, {}, {});
    line.draw_mode = GL_POINTS;
    line.scale = 100;

    constexpr int points_num = 100;
    line.vertices.colors = vector<RGBA>(points_num, {0.5, 0.2, 0.7, 1.0});
    setInterval(1. / 60, [&] {
        vector<glm::vec3> coords;
        for (const float x : linspace(-9, 9, points_num)) {
            const float y = f(x + float(window.tick_) / 100);
            coords.emplace_back(x, y, 0);
        }
        line.vertices.coords = coords;
        return true;
    });

    // 左上に常在する点
    StickyPointTopLeft top_left_point(window, world, viewport);

    // 文字の表示
    Font migmix_font;
    Text sample_text(ui_world, migmix_font, "This is sample text 123456789", {0.5, 0.8, 0.2, 0.4});
    Text credit_text(ui_world, migmix_font, "(C) LearnOpenGL.com", {0.3, 0.7, 0.9, 0.4});
    top_left_point.append(sample_text);
    sample_text.position = {20, -60, 0};

    // 三角形の表示
    Polygon my_triangle_polygon(main_shader, {{-0.8, -0.3, 0}, {-0.2, 0.7, 0}, {0.5, -0.5, 0}},
                                {
                                    {0.9, 0.3, 0, 1},
                                    {0.9, 0.2, 0.7, 0.3},
                                    {0.3, 0.7, 0.5, 0.5},
                                });
    Shape my_triangle(world, my_triangle_polygon);
    my_triangle.scale = 200;
    my_triangle.position = {-100, 0, 0};

    // レンダリングループ
    window.mainloop([&] {
        sample_text.text_ = toStr(window.tick_);

        world.masterDraw(camera);
        glClear(GL_DEPTH_BUFFER_BIT);
        ui_world.masterDraw(ui_camera);
    });
}
