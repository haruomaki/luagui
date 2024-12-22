#include <SumiGL/logger.hpp>
#include <lunchbox.hpp>

int main() {
    auto box = lunchbox::Lunchbox();
    box.list("assets");
    debug(box.get_text("assets/shaders/default.vsh"));
    box.dump_binary("assets/audio/テスト音声.wav");
}
