#pragma once

#include "Component.hpp"
#include "Resource.hpp"
#include <Lunchbox/OpenAL.hpp>
#include <Lunchbox/core/sound.hpp>
#include <SumiGL/buffered_container.hpp>

class Music : public Resource, public OpenAL::Buffer {
  public:
    Music(const lunchbox::core::SoundData &sound)
        : OpenAL::Buffer(sound.wave, sound.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, sound.samplerate) {}
};

class SoundSource : public Component, public OpenAL::Source {
  public:
    SoundSource(const Music &music);
    ~SoundSource() override;
};

class AudioManager {
  public:
    BufferedSet<SoundSource *> sources; // 管理対象のSource一覧
    float master_volume = 1.0f;         // 全体の最大音量

    AudioManager() = default;

    void adjust_overall_gain() {
        float total_gain = 0.0f;

        // 各Sourceの現在の音量を取得
        sources.flush();
        for (SoundSource *source : sources) {
            ALfloat gain = source->get_gain();
            total_gain += gain;
        }

        // 全体の音量合計がmaster_volumeになるように正規化
        float scale = master_volume / total_gain;
        for (SoundSource *source : sources) {
            ALfloat gain;
            alGetSourcef(source->get(), AL_GAIN, &gain);
            source->set_gain(gain * scale);
        }
    }
};
