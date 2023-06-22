#pragma once

#include <Draw.hpp>
#include <Shader.hpp>

struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Offset to advance to next glyph
};

class Font {
    ProgramObject shader_;
    const Camera &camera_;
    std::map<char, Character> Characters;
    unsigned int VAO, VBO;

    friend class Text;

  public:
    Font(const Camera &camera);
};

class Text : public WorldObject, Draw {
    Font &font_;
    string text_;
    RGBA color_;

  public:
    Text(World &world, Font &font, string text, RGBA color);

    void draw(const Camera &camera) const override;
};
