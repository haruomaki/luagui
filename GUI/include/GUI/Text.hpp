#pragma once

#include "Resource.hpp"
#include "Update.hpp"
#include "graphical_base.hpp"
#include <SumiGL/Shader.hpp>
#include <map>

struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Offset to advance to next glyph
};

// 文字描画用のシェーダと48ptフォントテクスチャのセット
class Font : public Resource {
    GL::ProgramObject shader_;
    std::map<char, Character> Characters;
    GL::VertexArray vao_;
    GL::VertexBuffer vbo_;

    friend class Text;

  public:
    Font(const std::string &font_path = "assets/fonts/main.ttf");
};

class Text : public UpdateComponent {
    Font &font_;
    RGBA color_;

    void draw() const;

  public:
    std::string text_;

    Text(Font &font, std::string text, RGBA color);
};
