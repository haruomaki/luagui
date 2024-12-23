#include <al.h>
#include <alc.h>
#include <iostream>
#include <vector>

#include <Lunchbox/OpenAL.hpp>
#include <Lunchbox/Storage.hpp>
#include <Lunchbox/sound.hpp>
#include <SumiGL/logger.hpp>

static void play_sound(std::vector<std::byte> &&wav_data) {
    auto [buffer, sfinfo] = lunchbox::load_sound(std::move(wav_data));

    // OpenALバッファとソース作成
    lunchbox::OpenALBuffer al_buffer(buffer, sfinfo.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, sfinfo.samplerate);
    lunchbox::OpenALSource source(al_buffer);

    // 再生
    source.play();
    std::cout << "再生中..." << std::endl;

    // 再生が終了するまで待機
    while (source.is_playing());
}

int main() {
    lunchbox::OpenALContext openal_ctx;
    lunchbox::Storage storage;
    storage.list("assets/audio");

    auto buffer = storage.get_binary("assets/audio/テスト音声.wav");
    play_sound(std::move(buffer));
}
