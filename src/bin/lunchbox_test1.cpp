#include <al.h>
#include <alc.h>
#include <iostream>
#include <sndfile.h>
#include <vector>

#include <SumiGL/logger.hpp>
#include <lunchbox.hpp>

// メモリ上のデータを操作するための構造体
struct ReadState {
    const std::byte *data; // メモリ上のデータの開始位置
    sf_count_t size;       // データの総サイズ
    sf_count_t offset;     // 現在の読み取り位置
};

// データのサイズを取得
static sf_count_t get_filelen(void *user_data) {
    auto l = static_cast<ReadState *>(user_data)->size;
    std::cout << "Length: " << l << "\n";
    return l;
}

// データの読み取り
static sf_count_t read_file(void *ptr, sf_count_t count, void *user_data) {
    auto *state = static_cast<ReadState *>(user_data);
    sf_count_t remaining = state->size - state->offset;
    sf_count_t to_read = std::min(count, remaining);
    std::cout << count << "Read File: " << to_read << "\n";

    std::memcpy(ptr, state->data + state->offset, to_read);
    state->offset += to_read;

    std::cout << "end\n";
    return to_read;
}

// データのシーク
static sf_count_t seek_file(sf_count_t offset, int whence, void *user_data) {
    std::cout << "Seek: " << offset << "\n";
    auto *state = static_cast<ReadState *>(user_data);

    sf_count_t new_offset = state->offset;
    if (whence == SEEK_SET) {
        new_offset = offset;
    } else if (whence == SEEK_CUR) {
        new_offset += offset;
    } else if (whence == SEEK_END) {
        new_offset = state->size + offset;
    } else {
        return -1; // 無効なシーク
    }

    if (new_offset < 0 || new_offset > state->size) {
        std::cout << "🥺\n";
        return -1; // 範囲外
    }

    std::cout << "ok\n";
    return state->offset = new_offset;
}

static sf_count_t tell(void *user_data) {
    auto offset = static_cast<ReadState *>(user_data)->offset;
    std::cout << "Tell: " << offset << "\n";
    return offset;
}

static SF_VIRTUAL_IO virtual_io = {
    get_filelen,
    seek_file,
    read_file,
    nullptr, tell};

static void play_sound(std::vector<std::byte> &&wav_data) {
    // MemoryData構造体を初期化
    ReadState memory_data = {wav_data.data(), static_cast<sf_count_t>(wav_data.size()), 0};

    // SF_INFO構造体をゼロ初期化
    SF_INFO sfinfo = {0};

    // 仮想ファイルを開く
    SNDFILE *file = sf_open_virtual(&virtual_io, SFM_READ, &sfinfo, &memory_data);

    if (!file) {
        throw std::runtime_error("Failed to open virtual file: " + std::string(sf_strerror(nullptr)));
    }

    // 音声データの処理
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
    box.list("assets/audio");
    // debug(box.get_text("assets/shaders/default.vsh"));

    auto buffer = box.get_binary("assets/audio/テスト音声.wav");
    play_sound(std::move(buffer));
}
