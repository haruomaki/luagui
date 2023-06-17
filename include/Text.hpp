#pragma once

#include <Window.hpp>

struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Offset to advance to next glyph
};

void prepareText();
void RenderText(const Window &window, std::string text, Point<float> pos, float scale, glm::vec3 color);
