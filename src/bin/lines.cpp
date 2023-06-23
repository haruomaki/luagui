#include <master.hpp>
#include <utility.hpp>

static float f(float x) {
    return 3 * sin(x);
}

// template <class Parent, class T, function<T()> getter>
// struct Property {
//     T value_;
//     operator T() const { return getter(); }
// };

// class MyClass {
//   public:
//     Property<MyClass, int, []() {}> value;
// };

namespace mc {
template <class T>
struct Property {
    T v_;
    operator T() { return v_; }
    void operator=(const T &v) { v_ = v; }
    T *operator->() { return &v_; }
    T &operator*() { return v_; }
    friend std::ostream &operator<<(std::ostream &os, const Property &it) { return os << it.v_; }
};
} // namespace mc

struct Person {
    mc::Property<const char *> firstName;
    mc::Property<std::string> lastName;
    mc::Property<int> age;
};

int main() {
    Person p = {"tom", "yum", 16};
    std::cout << p.firstName << " ";
    std::cout << p.lastName->c_str();
    printf("(%d)\n", *p.age);

    p.firstName = "O", p.lastName = "B", p.age = 1;
    printf("%s%s(%d)\n", *p.firstName, p.lastName->data(), *p.age);

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

    DynamicArray line(window, world, main_shader, {}, {}, GL_LINE_LOOP);
    line.setScale(100);
    line.draw(camera);

    for (auto &&x : linspace(-9, 9, 100)) {
        InterleavedVertexInfo ver;
        ver.coord_ = {x, f(x), 0};
        ver.color_ = {0.5, 0.2, 0.7, 1.0};
        line.vertices_.push_back(ver);
    }

    // 左上に常在する点
    StickyPointTopLeft top_left_point(window, world, viewport);

    // 文字の表示
    Font migmix_font;
    Text sample_text(ui_world, migmix_font, "This is sample text 123456789", {0.5, 0.8, 0.2, 0.4});
    Text credit_text(ui_world, migmix_font, "(C) LearnOpenGL.com", {0.3, 0.7, 0.9, 0.4});
    top_left_point.append(&sample_text);
    sample_text.setPosition({20, -60, 0});

    // 三角形の表示
    Polygon my_triangle_polygon(main_shader, {{-0.8, -0.3, 0}, {-0.2, 0.7, 0}, {0.5, -0.5, 0}},
                                {
                                    {0.9, 0.3, 0, 1},
                                    {0.9, 0.2, 0.7, 0.3},
                                    {0.3, 0.7, 0.5, 0.5},
                                });
    Shape my_triangle(world, my_triangle_polygon);
    my_triangle.setScale(200);
    my_triangle.setPosition({-100, 0, 0});

    // レンダリングループ
    window.mainloop([&] {
        for (auto &&ver : line.vertices_) {
            const float x = ver.coord_.x;
            const float y = f(x + float(window.tick_) / 100);
            ver.coord_ = {x, y, 0};
        }

        sample_text.text_ = toStr(window.tick_);

        world.masterDraw(camera);
        glClear(GL_DEPTH_BUFFER_BIT);
        ui_world.masterDraw(camera);
    });
}
