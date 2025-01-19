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
    auto res = font.shape(buffer);

    for (auto r : res) {
        debug(int(r.glyph_id));
    }
}
