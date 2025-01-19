#pragma once

#include "Resource.hpp"
#include "Update.hpp"
#include "graphical_base.hpp"
#include <Lunchbox/core/HarfBuzz.hpp>
#include <SumiGL/Shader.hpp>
#include <map>

struct Character {
    unsigned int texture_id; // ID handle of the glyph texture
    glm::ivec2 size;         // Size of glyph
    glm::ivec2 bearing;      // Offset from baseline to left/top of glyph
    unsigned int advance;    // Offset to advance to next glyph
};

// 文字描画用のシェーダと48ptフォントテクスチャのセット
class Font : public Resource {
    GL::ProgramObject shader_;
    std::map<unsigned long, Character> characters_;
    GL::VertexArray vao_;
    GL::VertexBuffer vbo_;

    friend class Text;

  public:
    Font(GL::ProgramObject &&shader, const harfbuzz::Font &hb_font);
};

class Text : public UpdateComponent {
    Font &font_;
    RGBA color_;

    void draw() const;

  public:
    std::string text;

    Text(Font &font, std::string text, RGBA color);
};
