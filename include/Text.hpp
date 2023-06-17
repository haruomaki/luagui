#pragma once

#include <Window.hpp>

struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Offset to advance to next glyph
};

class Font {
    Window &window_;
    ProgramObject shader_;
    std::map<char, Character> Characters;
    unsigned int VAO, VBO;

    friend class Text;

  public:
    Font(Window &window);
};

class Text : public WorldObject, Draw {
    Font &font_;
    string text_;
    glm::vec3 color_;

  public:
    Text(Font &font, string text, glm::vec3 color);

    void draw() const override;
};
