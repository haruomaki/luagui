#include <Window.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

DEFINE_RUNTIME_ERROR(FreeTypeException);

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

struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Offset to advance to next glyph
};

std::map<char, Character> Characters;

int main() {
    // FreeTypeを初期化
    FT_Library ft;
    if (FT_Init_FreeType(&ft) != 0) {
        throw FreeTypeException("ERROR::FREETYPE: Could not init FreeType Library");
    }

    // フォントを読み込む
    FT_Face face;
    if (FT_New_Face(ft, "assets/fonts/migmix-2p-regular.ttf", 0, &face) != 0) {
        throw FreeTypeException("ERROR::FREETYPE: Failed to load font");
    }

    // フォントサイズを指定
    FT_Set_Pixel_Sizes(face, 0, 48);

    // バイト列の制限（4の倍数byte）を解除する
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++) {
        // load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER) != 0) {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer);
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)};
        Characters.insert(std::pair<char, Character>(c, character));
    }

    // FreeTypeのリソースを解放
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    Window window(600, 500);

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

    setInterval(1000, [&] {
        debug(ins3.getAbsolutePosition());
        return true;
    });

    // レンダリングループ
    window.mainloop([&] {
    });
}
