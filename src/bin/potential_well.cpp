#include <master.hpp>
#include <utility.hpp>

int main() {
    GUI window(600, 500);
    MaximumViewport viewport(window);
    World main_world, ui_world;

    // バーテックスシェーダのコンパイル
    auto vsh_string = loadString("assets/shaders/shader.vsh");
    auto vsh_id = createShader(GL_VERTEX_SHADER, vsh_string);

    // フラグメントシェーダのコンパイル
    auto fsh_string = loadString("assets/shaders/shader.fsh");
    auto fsh_id = createShader(GL_FRAGMENT_SHADER, fsh_string);

    auto main_shader = ProgramObject{vsh_id, fsh_id};

    MobileNormalCamera camera(window, main_world, viewport);
    camera.position = {0, 0, 1000};
    camera.rotate = ANGLE_Y(M_PIf);
    camera.scale = 1;

    Font migmix_font;
    Text sample_text(main_world, migmix_font, "This is sample text 123456789", {0.5, 0.8, 0.2, 0.4});
    Text credit_text(main_world, migmix_font, "(C) LearnOpenGL.com", {0.3, 0.7, 0.9, 0.4});

    sample_text.setPosition({-200, 50, 200});
    credit_text.setPosition({200, 400, 1});

    ProgramObject hello_shader = {
        createShader(GL_VERTEX_SHADER, loadString("assets/shaders/hello.vsh")),
        createShader(GL_FRAGMENT_SHADER, loadString("assets/shaders/hello.fsh"))};

    DynamicArray line(window, main_world, main_shader);
    line.vertices.xs = linspace(-200, 200, 10);
    line.vertices.colors = vector<RGBA>(10, {0.5, 0.7, 0.1, 1});
    setInterval(0.5F, [&line] {
        line.setPosition(line.getPosition() + glm::vec3{-10, 0, 0});
        return true;
    });

    // レンダリングループ
    window.mainloop([&] {
        main_world.masterDraw(camera);
    });
}
