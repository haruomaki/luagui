#pragma once

#include "Draw.hpp"
#include <Shader.hpp>

struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Offset to advance to next glyph
};

// 文字描画用のシェーダと48ptフォントテクスチャのセット
class Font {
    ProgramObject shader_;
    std::map<char, Character> Characters;
    VertexArrayObject vao_{};
    VertexBufferObject vbo_{};

    friend class Text;

  public:
    Font();
};

class Text : public Draw {
    Font &font_;
    RGBA color_;

    void draw(const Camera &camera) const override;

  public:
    string text_;

    Text(Font &font, string text, RGBA color);
};
