#include <GUI.hpp>

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
        auto next_pos = getPosition() + glm::vec3(0, velocity_, 0);
        setPosition(next_pos);
        if (next_pos.y < -1 || 1 < next_pos.y) {
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

unsigned int VAO, VBO;

void RenderText(const Window &window, ProgramObject &s, std::string text, Point<float> pos, float scale, glm::vec3 color) {
    // activate corresponding render state
    s.use();
    glUniform3f(glGetUniformLocation(s.getPrgramId(), "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    float tail = 0;

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];

        float xpos = tail + ch.Bearing.x;
        float ypos = -(ch.Size.y - ch.Bearing.y);

        float w = ch.Size.x;
        float h = ch.Size.y;
        // update VBO for each character
        float vertices[6][4] = {
            {xpos, ypos + h, 0.0f, 0.0f},
            {xpos, ypos, 0.0f, 1.0f},
            {xpos + w, ypos, 1.0f, 1.0f},

            {xpos, ypos + h, 0.0f, 0.0f},
            {xpos + w, ypos, 1.0f, 1.0f},
            {xpos + w, ypos + h, 1.0f, 0.0f}};
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // モデルビュー行列
        const auto model_matrix = glm::translate(glm::mat4(1), {pos.x_, pos.y_, 1}) * glm::scale(glm::mat4(1), {scale, scale, scale});
        const auto model_view_matrix = window.getViewMatrix() * model_matrix;
        s.setUniform("modelViewMatrix", model_view_matrix);

        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        tail += (ch.Advance >> 6); // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

int main() {
    Window window(600, 500);

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

    // ブレンド（透明処理）の設定
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 projection = glm::ortho(0.0f, 600.0f, 0.0f, 500.0f);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    ProgramObject font_shader = {
        createShader(GL_VERTEX_SHADER, loadString("assets/shaders/font.vsh")),
        createShader(GL_FRAGMENT_SHADER, loadString("assets/shaders/font.fsh"))};

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
    ins3.setScale(3);

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
        RenderText(window, font_shader, "This is sample text", Point<float>{-0.5, 0}, 1, glm::vec3(0.5, 0.8f, 0.2f));
        // RenderText(font_shader, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

        shader.use();
        vao.bind([&] {
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        });
    });
}
