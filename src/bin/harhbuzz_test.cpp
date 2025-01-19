#include <Lunchbox/Storage.hpp>
#include <Lunchbox/core/HarfBuzz.hpp>
#include <SumiGL/logger.hpp>

int main() {
    lunchbox::Storage storage;

    debug(storage.list("/"));

    auto font = storage.get_font("fonts/UDEVGothic.ttf");
    // auto face = storage.get_font("fonts/HackGen.ttf");
    auto res = font.shape("鯖と鯖󠄀->");

    for (auto r : res) {
        debug(int(r.glyph_id));
    }
}
