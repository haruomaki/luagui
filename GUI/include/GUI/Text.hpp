#pragma once

#include "Resource.hpp"
#include "Update.hpp"
#include "graphical_base.hpp"
#include <Lunchbox/core/HarfBuzz.hpp>
#include <SumiGL/Shader.hpp>
#include <map>

// namespace typesetting {

// struct Decoration {
//     std::string text;
//     const harfbuzz::Font *font;
// };

// } // namespace typesetting

struct Character {
    unsigned int texture_id; // ID handle of the glyph texture
    glm::ivec2 size;         // Size of glyph
    glm::ivec2 bearing;      // Offset from baseline to left/top of glyph
    unsigned int advance;    // Offset to advance to next glyph
};

// 各文字のテクスチャを遅延生成＆取得できるクラス。
class CharactersCache {
  public:
    using Key = std::tuple<harfbuzz::GlyphID, unsigned int, unsigned int>;

  private:
    const harfbuzz::Font &font_;
    std::map<Key, Character> cache_;

  public:
    CharactersCache(const harfbuzz::Font &font)
        : font_(font) {}
    Character at(Key key);
};

// 文字描画用のシェーダと48ptフォントテクスチャのセット
class Font : public Resource {
    GL::ProgramObject shader_;
    harfbuzz::Font hb_font_;
    CharactersCache characters_;
    GL::VertexArray vao_;
    GL::VertexBuffer vbo_;

    friend class Text;

  public:
    Font(GL::ProgramObject &&shader, harfbuzz::Font &&hb_font);
    [[nodiscard]] const harfbuzz::Font &hb() const { return hb_font_; }
};

class Text : public UpdateComponent {
    Font &font_;
    RGBA color_;

    void draw() const;

  public:
    std::string text;
    float font_size = 48;
    glm::vec2 anchor = {0, 0};

    Text(Font &font, std::string text, RGBA color);
};
