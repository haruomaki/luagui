#include <master.hpp>

static float f(float x) {
    return 3 * sin(x);
}

int main() {
    Window window(600, 500);
    MaximumViewport viewport;

    World world;

    ProgramObject main_shader = {
        createShader(GL_VERTEX_SHADER, loadString("assets/shaders/shader.vsh")),
        createShader(GL_FRAGMENT_SHADER, loadString("assets/shaders/shader.fsh"))};

    OrthoCamera camera(window);
    camera.setScale(0.01F);

    DynamicArray line(world, main_shader, camera, {}, {}, GL_LINE_LOOP);

    for (auto &&x : linspace(-9, 9, 100)) {
        InterleavedVertexInfo2 ver;
        ver.coord_ = {x, f(x), 0};
        ver.color_ = {0.5, 0.2, 0.7, 1.0};
        line.vertices_.push_back(ver);
    }

    setInterval(2.F, [] {
        array<GLint, 4> vp;
        glGetIntegerv(GL_VIEWPORT, vp.data());
        debug(vp);
        return true;
    });

    // レンダリングループ
    window.mainloop([&] {
        for (auto &&ver : line.vertices_) {
            const float x = ver.coord_.x;
            const float y = f(x + float(window.tick_) / 100);
            ver.coord_ = glm::vec3{x, y, 0};
        }

        world.masterDraw(camera);
    });
}
