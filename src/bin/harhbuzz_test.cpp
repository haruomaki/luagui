#include <Lunchbox/Storage.hpp>
#include <Lunchbox/core/HarfBuzz.hpp>
#include <SumiGL/logger.hpp>

int main() {
    lunchbox::Storage storage;

    debug(storage.list("/"));

    auto font = storage.get_font("fonts/UDEVGothic.ttf");
    // auto font = storage.get_font("fonts/main.ttf");
    // hb_face_get_upem(font.get());
    // auto res = font.shape("鯖と鯖󠄀->");
    harfbuzz::Buffer buffer;
    buffer.add_utf8("abcこんにちは");
    buffer.dump("guess前");
    hb_buffer_guess_segment_properties(buffer.get());
    buffer.dump("guess後");
    auto res = font.shape(buffer);

    font.dump();

    for (auto r : res) {
        // debug(int(r.glyph_id));
        // debug(r.x_offset, r.y_offset, r.x_advance, r.y_advance);
        std::cout << int(r.glyph_id) << " ";
    }
}
