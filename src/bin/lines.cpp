#include <Window.hpp>

static float f(float x) {
    return 3 * sin(x);
}

int main() {
    Window window(600, 500);

    ProgramObject main_shader = {
        createShader(GL_VERTEX_SHADER, loadString("assets/shaders/shader.vsh")),
        createShader(GL_FRAGMENT_SHADER, loadString("assets/shaders/shader.fsh"))};

    OrthoCamera camera(window);
    window.world_object_root_.append(&camera);
    camera.setPosition({0, 0, 0});
    camera.setScale(0.01F);

    DynamicArray line(main_shader, camera, {});
    window.world_object_root_.append(&line);
    line.setScale(1);

    for (auto &&x : linspace(-9, 9, 100)) {
        InterleavedVertexInfo2 ver;
        ver.coord_ = {x, f(x), 0};
        ver.color_ = {0.5, 0.2, 0.7, 1.0};
        line.vertices_.push_back(ver);
    }

    debug(linspace(0, 10, 5, false));
    debug(linspace(10, 0, 5, false));

    debug(arange(0, 10.5, 2));
    debug(arange(0, 11.5, -2));
    debug(arange(10.5, 0, 2));
    debug(arange(10.5, 0, -2));

    // レンダリングループ
    window.mainloop([] {});
}
