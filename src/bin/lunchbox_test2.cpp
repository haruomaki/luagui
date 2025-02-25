#include <Lunchbox/OpenAL.hpp>
#include <Lunchbox/Storage.hpp>
#include <Lunchbox/core/third-party/tiny_gltf.h>
#include <SumiGL/misc.hpp>

int main() {
    setUTF8();
    std::cout << "こんにちは\n";

    OpenAL::Context openal_ctx;
    lunchbox::Storage storage;
    std::cout << storage.get_text("shaders/hello.vsh");

    const auto sound = storage.get_sound("audio/テスト音声.wav");

    // // OpenALバッファとソース作成
    // OpenAL::Buffer al_buffer(sound.wave, sound.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, sound.samplerate);
    // OpenAL::Source source(al_buffer);

    // // 再生
    // source.play();
    // source.set_gain(1); // 音量調節も可能
    // std::cout << "再生中...\n";

    // // 再生が終了するまで待機
    // while (source.is_playing());
}
