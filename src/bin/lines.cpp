#include <Window.hpp>

int main() {
    Window window(600, 500);

    ProgramObject main_shader = {
        createShader(GL_VERTEX_SHADER, loadString("assets/shaders/shader.vsh")),
        createShader(GL_FRAGMENT_SHADER, loadString("assets/shaders/shader.fsh"))};

    NormalCamera camera(window);
    camera.setPosition({0, 0, 1000});
    camera.setScale(1);

    DynamicArray line(main_shader, camera, {{0, 0, 0}, {200, 200, 200}});
    window.world_object_root_.append(&line);
    setInterval(0.1, [&line] {
        line.setPosition(line.getPosition() + glm::vec3{-10, 0, 0});
        return true;
    });

    // レンダリングループ
    window.mainloop([] {});
}
