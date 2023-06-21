#include <Window.hpp>

// float f(float x) {
//     return
// }

int main() {
    Window window(600, 500);

    ProgramObject main_shader = {
        createShader(GL_VERTEX_SHADER, loadString("assets/shaders/shader.vsh")),
        createShader(GL_FRAGMENT_SHADER, loadString("assets/shaders/shader.fsh"))};

    NormalCamera camera(window);
    camera.setPosition({0, 0, 1000});
    camera.setScale(1);

    DynamicArray line(main_shader, camera, {});
    window.world_object_root_.append(&line);

    for (float x = -300; x <= 300; x += 10.0F) {
        InterleavedVertexInfo2 ver;
        ver.coord_ = {x, x * x / 300, 0};
        ver.color_ = {0.5, 0.2, 0.7, 1.0};
        line.vertices_.push_back(ver);
    }

    // レンダリングループ
    window.mainloop([] {});
}
