#include <Window.hpp>

class QButton : KeyCallback {
    void keyCallback(int key, int action) {
        cout << "QButtonのコールバックだよ" << endl;
        if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
            cout << "さようなら" << endl;
            exit(0);
        }
    }
};

QButton *qbutton;
bool is_there_qbutton = false;

class Leaf : KeyCallback {
    void keyCallback(int key, int action) {
        if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
            cout << "UP!" << endl;
            for (int i = 0; i < 20; i++) {
                // gon.update();
            }
            if (!is_there_qbutton) {
                qbutton = new QButton;
                is_there_qbutton = true;
                cout << "出現！" << endl;
            } else {
                delete qbutton;
                is_there_qbutton = false;
                cout << "消えます" << endl;
            }
        }
    }
};

class MovingPolygonInstance : public PolygonInstance, Update {
    using PolygonInstance::PolygonInstance;
    static constexpr float speed = 1.5f;
    float velocity_ = -speed;

    void update() override {
        auto next_pos = getPosition() + glm::vec3(0, velocity_, 0);
        setPosition(next_pos);
        if (next_pos.y < -300 || 300 < next_pos.y) {
            velocity_ *= -1.1f;
        }
    }
};

int main() {
    Window window(600, 500);

    Font migmix_font(window);
    Text sample_text(migmix_font, "This is sample text 123456789", {0.5, 0.8, 0.2});
    Text credit_text(migmix_font, "(C) LearnOpenGL.com", {0.3, 0.7, 0.9});

    credit_text.setPosition({200, 500, 0});

    Leaf leaf;

    GLuint tex_id = loadTexture("assets/images/cat.raw");

    Polygon gon(window, {{0.9f, 0.9f}, {0.5f, 0.f}, {0.f, 0.f}, {0.f, 0.5f}},
                {
                    {0.9, 0.3, 0, 1},
                    {0.1, 0.2, 0.7, 0.3},
                    {0.9, 0.2, 0.7, 0.3},
                    {0.3, 0.7, 0.5f, 0.5},
                },
                tex_id);

    Polygon gon2(window, {{-0.8, -0.3}, {-0.2, 0.7}, {0.5, -0.5}},
                 {
                     {0.9, 0.3, 0, 1},
                     {0.9, 0.2, 0.7, 0.3},
                     {0.3, 0.7, 0.5f, 0.5},
                 });

    Polygon gon3(window, {{0.4, 0.3}, {0.5, 0.3}, {0.5, 0.4}, {0.45, 0.35}, {0.4, 0.4}},
                 {
                     {0.9, 0.3, 0, 1},
                     {0.9, 0.2, 0.7, 0.3},
                     {0.9, 0.2, 0.7, 0.3},
                     {0.9, 0.2, 0.7, 0.3},
                     {0.3, 0.7, 0.5f, 0.5},
                 });

    Polygon poly(window, {{-0.9, -0.9}, {-0.9, 0}, {0, 0}, {0, -0.9}}, {{0.3, 0.7, 0.1, 1}});

    PolygonInstance ins(gon), ins2(gon2), inspoly(poly);
    MovingPolygonInstance ins3(gon3);
    window.world_object_root_.append(&ins3);
    ins3.append(&ins);
    ins3.setScale(900);
    ins2.setScale(1500);
    inspoly.setScale(300);

    setInterval(1000, [&] {
        debug(ins3.getAbsoluteTransform());
        return true;
    });

    ProgramObject hello_shader = {
        createShader(GL_VERTEX_SHADER, loadString("assets/shaders/hello.vsh")),
        createShader(GL_FRAGMENT_SHADER, loadString("assets/shaders/hello.fsh"))};

    const vector<glm::vec3> vertices = {
        glm::vec3(0, -0.9, 0),
        glm::vec3(0.5, 0, 0),
        glm::vec3(0, 0.5, 0),
        glm::vec3(-0.5, 0, 0),
    };

    const auto &shader = hello_shader;
    auto vbo = VertexBufferObject::gen(sizeof(glm::vec3) * 4, vertices.data(), GL_STATIC_DRAW);

    // VAOを作成。頂点の座標と色、uvを関連付ける
    auto vao = VertexArrayObject::gen();
    vao.bind([&] {
        vbo.bind([&] {
            shader.setAttribute("position", 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr); // 位置
        });
        getErrors();
    });

    // レンダリングループ
    window.mainloop([&] {
        shader.use();
        vao.bind([&] {
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        });
    });
}
