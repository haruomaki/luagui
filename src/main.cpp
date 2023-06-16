#include <Window.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

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
    static constexpr float speed = 0.005f;
    float velocity_ = -speed;

    void update() override {
        auto next_pos = getPosition() + Point<float>{0, velocity_};
        setPosition(next_pos);
        if (next_pos.y_ < -1 || 1 < next_pos.y_) {
            velocity_ *= -1.1f;
        }
    }
};

int main() {
    Window window(600, 500);

    Leaf leaf;

    GLuint tex_id = loadTexture("image/cat.raw");

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

    setInterval(1000, [&] {
        debug(ins3.getAbsolutePosition());
        return true;
    });

    // レンダリングループ
    window.mainloop([&] {
    });
}
