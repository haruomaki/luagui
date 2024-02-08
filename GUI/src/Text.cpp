#include "Text.hpp"
#include "Shader.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

DEFINE_RUNTIME_ERROR(FreeTypeException);

Font::Font()
    : shader_(ProgramObject{
          create_shader(GL_VERTEX_SHADER, load_string("assets/shaders/font.vsh")),
          create_shader(GL_FRAGMENT_SHADER, load_string("assets/shaders/font.fsh"))}) {
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

    this->vao_ = VertexArrayObject::gen();
    this->vbo_ = VertexBufferObject::gen(sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    this->vao_.bind([&] {
        this->vbo_.bind([&] {
            this->shader_.set_attribute("vertex", 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
        });
    });
}

Text::Text(Font &font, string text, RGBA color)
    : font_(font)
    , color_(color)
    , text_(std::move(text)) {}

void Text::draw(const Camera &camera) const {
    // activate corresponding render state
    font_.shader_.use();
    font_.shader_.set_uniform("textColor", color_);
    glActiveTexture(GL_TEXTURE0);
    this->font_.vao_.bind([&] {
        float tail = 0;

        // iterate through all characters
        std::string::const_iterator c;
        for (c = text_.begin(); c != text_.end(); c++) {
            Character ch = font_.Characters.at(*c);

            float xpos = tail + ch.Bearing.x;
            float ypos = -(ch.Size.y - ch.Bearing.y);

            float w = ch.Size.x;
            float h = ch.Size.y;
            // update VBO for each character
            float vertices[6][4] = {
                {xpos, ypos + h, 0.0F, 0.0F},
                {xpos, ypos, 0.0F, 1.0F},
                {xpos + w, ypos, 1.0F, 1.0F},

                {xpos, ypos + h, 0.0F, 0.0F},
                {xpos + w, ypos, 1.0F, 1.0F},
                {xpos + w, ypos + h, 1.0F, 0.0F}};
            // render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // update content of VBO memory
            this->font_.vbo_.bind([&] {
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            });

            // モデルビュー行列
            const auto model_matrix = this->get_absolute_transform();
            const auto model_view_matrix = camera.get_view_matrix() * model_matrix;
            font_.shader_.set_uniform("modelViewMatrix", model_view_matrix);

            // 射影変換行列
            const auto projection_matrix = camera.get_projection_matrix();
            font_.shader_.set_uniform("projectionMatrix", projection_matrix);

            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            tail += (ch.Advance >> 6); // bitshift by 6 to get value in pixels (2^6 = 64)
        }
    });
    glBindTexture(GL_TEXTURE_2D, 0);
}
