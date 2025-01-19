#include "Text.hpp"
#include "Camera.hpp"
#include "World.hpp"
#include <Lunchbox/core/FreeType.hpp>
#include <SumiGL/Shader.hpp>

DEFINE_RUNTIME_ERROR(FreeTypeException);

// NOLINTNEXTLINE(readability-identifier-naming)
constexpr float pt_meter = 0.3528f / 1000.f; // 1ptは0.3528mm

Character CharactersCache::at(harfbuzz::GlyphID gid) {
    if (!cache_.contains(gid)) {
        // フォントを読み込む
        FT_Face face = font_.ft_face();

        // フォントサイズを指定（48で固定） TODO: ディスプレイ解像度に合わせてテクスチャの大きさを変更
        FT_Set_Pixel_Sizes(face, 0, 48);

        // バイト列の制限（4の倍数byte）を解除する
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

        // load character glyph
        if (FT_Load_Glyph(face, gid, FT_LOAD_RENDER) != 0) {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << '\n';
            warn("フォントにグリフが含まれていません（コードポイント: ", int(gid), "）");
            return {};
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            int(face->glyph->bitmap.width),
            int(face->glyph->bitmap.rows),
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
        cache_[gid] = character;
    }

    return cache_[gid];
}

Font::Font(GL::ProgramObject &&shader, harfbuzz::Font &&hb_font)
    : shader_(std::move(shader))
    , hb_font_(std::move(hb_font))
    , characters_(hb_font_) {
    this->vbo_ = GL::VertexBuffer(sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    this->vao_.bind([&] {
        this->vbo_.bind([&] {
            this->shader_.set_attribute_float("vertex", 4, false, 4 * sizeof(float), nullptr);
        });
    });
}

Text::Text(Font &font, string text, RGBA color)
    : UpdateComponent([this](auto &) { this->draw(); }, "Draw")
    , font_(font)
    , color_(color)
    , text(std::move(text)) {}

void Text::draw() const {
    // activate corresponding render state
    CameraInterface &camera = *world().rendering_camera();
    font_.shader_.use();
    font_.shader_.set_uniform("textColor", color_);
    glActiveTexture(GL_TEXTURE0);
    this->font_.vao_.bind([&] {
        float tail = 0;

        // HarfBuzzを利用してUTF8バイト列からグリフを取得。
        auto results = font_.hb().shape(text);

        // すべてのグリフについてイテレートする。
        for (auto shaping : results) {
            char32_t gid = shaping.glyph_id;
            Character ch = font_.characters_.at(gid);

            // Characterのサイズやオフセット情報から描画位置・幅を計算する
            // 標準48ptのフォント。1pt=1px=0.3528mmだと決め打ってスケーリングする TODO: HiDPI（1pt≠1px）時の対応
            float xpos = tail + float(ch.bearing.x) * pt_meter;
            float ypos = -float(ch.size.y - ch.bearing.y) * pt_meter;

            float w = float(ch.size.x) * pt_meter;
            float h = float(ch.size.y) * pt_meter;
            // update VBO for each character
            float vertices[6][4] = {
                {xpos, ypos + h, 0.0F, 0.0F},
                {xpos, ypos, 0.0F, 1.0F},
                {xpos + w, ypos, 1.0F, 1.0F},

                {xpos, ypos + h, 0.0F, 0.0F},
                {xpos + w, ypos, 1.0F, 1.0F},
                {xpos + w, ypos + h, 1.0F, 0.0F}};
            // render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.texture_id);
            // update content of VBO memory
            this->font_.vbo_.bind([&] {
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            });

            // モデルビュー行列
            const auto model_matrix = owner().get_absolute_transform();
            const auto model_view_matrix = camera.get_view_matrix() * model_matrix;
            font_.shader_.set_uniform("modelViewMatrix", model_view_matrix);

            // 射影変換行列
            const auto projection_matrix = camera.get_projection_matrix();
            font_.shader_.set_uniform("projectionMatrix", projection_matrix);

            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            debug(shaping.x_advance, shaping.y_advance);
            debug(shaping.x_offset, shaping.y_offset);
            tail += float(ch.advance >> 6) * pt_meter; // bitshift by 6 to get value in pixels (2^6 = 64)
        }
    });
    glBindTexture(GL_TEXTURE_2D, 0);
}
