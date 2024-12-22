#include <al.h>
#include <alc.h>
#include <iostream>
#include <sndfile.h>
#include <vector>

#include <SumiGL/logger.hpp>
#include <lunchbox.hpp>

// メモリ上のデータを読み込むためのカスタム関数
static sf_count_t get_filelen(void *user_data) {
    std::vector<char> *data = static_cast<std::vector<char> *>(user_data);
    return data->size();
}

static sf_count_t read_file(void *ptr, sf_count_t count, void *user_data) {
    std::vector<char> *data = static_cast<std::vector<char> *>(user_data);
    sf_count_t to_read = std::min(count, static_cast<sf_count_t>(data->size()));
    std::memcpy(ptr, data->data(), to_read);
    data->erase(data->begin(), data->begin() + to_read);
    return to_read;
}

static sf_count_t seek_file(sf_count_t offset, int whence, void *user_data) {
    std::vector<char> *data = static_cast<std::vector<char> *>(user_data);
    if (whence == SEEK_SET) {
        if (offset >= 0 && offset < data->size()) {
            data->erase(data->begin(), data->begin() + offset);
            return offset;
        }
    }
    return -1;
}

SF_VIRTUAL_IO virtual_io = {
    get_filelen,
    seek_file,
    read_file,
    nullptr, nullptr};

void play_sound(std::vector<char> wav_data) {
    SF_INFO sfinfo;
    SNDFILE *file = sf_open_virtual(&virtual_io, SFM_READ, &sfinfo, &wav_data);
    if (!file) {
        std::cerr << "ファイルを開けませんでした" << std::endl;
        std::abort();
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

int main() {
    auto box = lunchbox::Lunchbox();
    box.list("assets");
    debug(box.get_text("assets/shaders/default.vsh"));

    box.get_binary_stream("assets/audio/テスト音声.wav");
}
