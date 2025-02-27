#pragma once

#include "Component.hpp"
#include "Resource.hpp"
#include <Lunchbox/OpenAL.hpp>
#include <Lunchbox/core/sound.hpp>
#include <SumiGL/buffered_container.hpp>
#include <map>

class Music : public Resource, public OpenAL::Buffer {
  public:
    Music(const lunchbox::core::SoundData &sound)
        : OpenAL::Buffer(sound.wave, sound.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, sound.samplerate) {}
};

class AudioManager;

// AudioManagerと密結合する。
// 音声の追加や削除、音量調整のタイミングをフックして全体音量等を即座に調整するため。
class SoundSource : public Component, OpenAL::Source {
    friend class AudioManager; // OpenAL::Sourceのメソッドに触れるように

    const std::string group_name_;

  public:
    SoundSource(std::string &&group_name, const Music &music);
    ~SoundSource() override;

    std::string_view group_name() { return group_name_; }
    void play() { OpenAL::Source::play(); }
    bool is_playing() { return OpenAL::Source::is_playing(); }
};

class AudioManager {
  public:
    BufferedSet<SoundSource *> sources;        // 管理対象のSource一覧 INFO: Bufferedである必要は無いけど念のため。
    std::map<std::string, float> group_volume; // グループ内の最大音量

    AudioManager() = default;

    void adjust_overall_gain() {
        // print("あじゃすとです！");
        std::map<std::string, float> total_gain;

        // グループごとに、各Sourceの現在の音量を取得
        sources.flush();
        for (SoundSource *source : sources) {
            ALfloat gain = source->get_gain();
            total_gain[source->group_name_] += gain;
        }

        // 全体の音量合計がmaster_volumeになるように正規化
        // float scale = group_volume / total_gain;
        std::map<std::string, float> scale;
        for (const auto &[k, v] : total_gain) {
            if (!group_volume.contains(k)) group_volume[k] = 1.0f;
            // debug(k, group_volume[k]);
            scale[k] = group_volume[k] / v;
        }
        for (SoundSource *source : sources) {
            ALfloat gain;
            alGetSourcef(source->get(), AL_GAIN, &gain);
            source->set_gain(gain * scale[source->group_name_]);
        }
    }
};
