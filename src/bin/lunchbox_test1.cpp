#include <Lunchbox/OpenAL.hpp>
#include <Lunchbox/Storage.hpp>
#include <SumiGL/misc.hpp>

int main() {
    setUTF8();

    lunchbox::OpenALContext openal_ctx;
    lunchbox::Storage storage;

    auto [buffer, sfinfo] = storage.get_sound("assets/audio/テスト音声.wav");

    // OpenALバッファとソース作成
    lunchbox::OpenALBuffer al_buffer(buffer, sfinfo.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, sfinfo.samplerate);
    lunchbox::OpenALSource source(al_buffer);

    // 再生
    source.play();
    std::cout << "再生中...\n";

    // 再生が終了するまで待機
    while (source.is_playing());
}
