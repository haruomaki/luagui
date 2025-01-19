#include <Lunchbox/Storage.hpp>
#include <Lunchbox/core/HarfBuzz.hpp>
#include <SumiGL/logger.hpp>

int main() {
    lunchbox::Storage storage;

    debug(storage.list("/"));

    auto face = storage.get_font("fonts/UDEVGothic.ttf");
    // auto face = storage.get_font("fonts/HackGen.ttf");
    harfbuzz::Font font(face);
    harfbuzz::Buffer buffer;
    buffer.add_utf8("鯖と鯖󠄀->");
    hb_shape(font.get(), buffer.get(), nullptr, 0);

    // シェイピング結果を取得
    unsigned int glyph_count;
    hb_glyph_info_t *glyph_info = hb_buffer_get_glyph_infos(buffer.get(), &glyph_count);
    hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buffer.get(), &glyph_count);

    // std::vector<Glyph> glyphs;

    for (unsigned int i = 0; i < glyph_count; i++) {
        unsigned int glyph_id = glyph_info[i].codepoint;
        float x_advance = glyph_pos[i].x_advance / 64.0f; // 26.6固定小数点
        float y_advance = glyph_pos[i].y_advance / 64.0f;
        float x_offset = glyph_pos[i].x_offset / 64.0f;
        float y_offset = glyph_pos[i].y_offset / 64.0f;
        debug(glyph_id);
    }
}
