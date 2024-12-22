#include <al.h>
#include <alc.h>
#include <iostream>
#include <sndfile.h>
#include <vector>

#include <SumiGL/logger.hpp>
#include <lunchbox.hpp>

int main() {
    auto box = lunchbox::Lunchbox();
    box.list("assets");
    debug(box.get_text("assets/shaders/default.vsh"));
    box.dump_binary("assets/audio/テスト音声.wav");

    // ファイル読み込み
    const char *file_path = "assets/audio/テスト音声.wav";
    SF_INFO sfinfo;
    SNDFILE *file = sf_open(file_path, SFM_READ, &sfinfo);
    if (!file) {
        std::cerr << "ファイルを開けませんでした" << std::endl;
        return -1;
    }

    std::vector<short> buffer(sfinfo.frames * sfinfo.channels);
    sf_readf_short(file, buffer.data(), sfinfo.frames);
    sf_close(file);

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
