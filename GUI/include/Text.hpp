#pragma once

#include <Drawable.hpp>
#include <Shader.hpp>

struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Offset to advance to next glyph
};

class Font {
    ProgramObject shader_;
    std::map<char, Character> Characters;
    unsigned int VAO, VBO;

    friend class Text;

  public:
    Font();
};

class Text : public DrawableWorldObject {
    Font &font_;
    RGBA color_;

    void draw(const Camera &camera) const override;

  public:
    string text_;

    Text(World &world, Font &font, string text, RGBA color);
};
