#include "Text.hpp"
#include "Camera.hpp"
#include "World.hpp"
#include <Lunchbox/core/FreeType.hpp>
#include <SumiGL/Shader.hpp>
#include <SumiGL/constants.hpp>

Character CharactersCache::at(CharactersCache::Key key) {
    const auto [gid, pixel_width, pixel_height] = key;
    if (!cache_.contains(key)) {
        // フォントを読み込む
        FT_Face face = font_.ft_face();

        // フォントサイズを指定。ここで指定されるpixel_width/heightは、ディスプレイ解像度に合わせて修正済みの値。
        FT_Set_Pixel_Sizes(face, pixel_width, pixel_height);

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
            .texture_id = texture,
            .size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            .bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            .advance = static_cast<unsigned int>(face->glyph->advance.x)};
        cache_[key] = character;
    }

    return cache_[key];
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
    , text(std::move(text))
    , color(color) {}

struct Chip {
    unsigned int texture_id;
    float xpos, ypos, w, h;
};

struct Layout {
    std::vector<Chip> chips;
    float width, height;
};

static Layout layout(CharactersCache &characters, const harfbuzz::Font &font, const std::string &text, float pixel_width, float pixel_height) {
    std::vector<Chip> chips;
    float tail = 0;

    // HarfBuzzを利用してUTF8バイト列からグリフを取得。
    auto results = font.shape(text);

    // すべてのグリフについてイテレートする。
    for (auto shaping : results) {
        auto gid = shaping.glyph_id;
        Character ch = characters.at({gid, int(pixel_width), int(pixel_height)});

        // Characterのサイズやオフセット情報から描画位置・幅を計算する
        // 1px = 1/72インチ (≒ 0.3528mm) であると決め打ってスケーリングする。72DPIのディスプレイだと丁度紙面上のptどおりの大きさになる。
        float xpos = tail + float(ch.bearing.x);
        float ypos = -float(ch.size.y - ch.bearing.y);

        auto w = float(ch.size.x);
        auto h = float(ch.size.y);

        chips.emplace_back(
            ch.texture_id,
            xpos,
            ypos,
            w,
            h);

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        // debug(shaping.x_advance, shaping.y_advance);
        // debug(shaping.x_offset, shaping.y_offset);
        // TODO: shaping.x_advanceを活用したいがスケールが合っていない＆正しい値なのかイマイチよく分からない。
        tail += float(ch.advance >> 6); // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    return {.chips = chips, .width = tail, .height = pixel_height};
}

void Text::draw() const {
    // activate corresponding render state
    CameraInterface &camera = *world().rendering_camera();
    font_.shader_.use();
    font_.shader_.set_uniform("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    this->font_.vao_.bind([&] {
        const auto [dpi_scale_x, dpi_scale_y] = gui().monitor_content_scale();
        const auto extra_zoom = 1.5f; // ぼやける問題の応急処置として、大きめにラスタライズする。
        auto lout = layout(font_.characters_, font_.hb(), text, font_size * dpi_scale_x * extra_zoom, font_size * dpi_scale_y * extra_zoom);

        // すべてのグリフについてイテレートする。
        for (auto chip : lout.chips) {
            // Characterのサイズやオフセット情報から描画位置・幅を計算する
            // 1px = 1/72インチ (≒ 0.3528mm) であると決め打ってスケーリングする。72DPIのディスプレイだと丁度紙面上のptどおりの大きさになる。
            const float scale_x = px_meter / (dpi_scale_x * extra_zoom); // 大きめにラスタライズした分、テクスチャ自体が大きくなっているのでCSで割って補正。
            const float scale_y = px_meter / (dpi_scale_y * extra_zoom);
            float xpos = (chip.xpos - lout.width * (1 - anchor.x) / 2) * scale_x;
            float ypos = (chip.ypos - lout.height * (1 - anchor.y) / 2) * scale_y;
            float w = chip.w * scale_x;
            float h = chip.h * scale_y;
            // update VBO for each character
            float vertices[6][4] = {
                {xpos, ypos + h, 0.0F, 0.0F},
                {xpos, ypos, 0.0F, 1.0F},
                {xpos + w, ypos, 1.0F, 1.0F},

                {xpos, ypos + h, 0.0F, 0.0F},
                {xpos + w, ypos, 1.0F, 1.0F},
                {xpos + w, ypos + h, 1.0F, 0.0F}};
            // render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, chip.texture_id);
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
        }
    });
    glBindTexture(GL_TEXTURE_2D, 0);
}
