#include <al.h>
#include <alc.h>
#include <iostream>
#include <vector>

#include <Lunchbox/Storage.hpp>
#include <Lunchbox/sound.hpp>
#include <SumiGL/logger.hpp>

static void play_sound(std::vector<std::byte> &&wav_data) {
    auto [buffer, sfinfo] = lunchbox::load_sound(std::move(wav_data));

    // OpenAL初期化
    ALCdevice *device = alcOpenDevice(nullptr);
    ALCcontext *context = alcCreateContext(device, nullptr);
    alcMakeContextCurrent(context);

    // OpenALバッファとソース作成
    ALuint al_buffer;
    alGenBuffers(1, &al_buffer);
    alBufferData(al_buffer, (sfinfo.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16),
                 buffer.data(), int(buffer.size() * sizeof(short)), sfinfo.samplerate);

    ALuint source;
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, int(al_buffer));

    // 再生
    alSourcePlay(source);
    std::cout << "再生中..." << std::endl;

    // 再生が終了するまで待機
    ALint state;
    do {
        alGetSourcei(source, AL_SOURCE_STATE, &state);
    } while (state == AL_PLAYING);

    // 後始末
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &al_buffer);
    alcDestroyContext(context);
    alcCloseDevice(device);
}

int main() {
    lunchbox::Storage storage;
    storage.list("assets/audio");

    auto buffer = storage.get_binary("assets/audio/テスト音声.wav");
    play_sound(std::move(buffer));
}
