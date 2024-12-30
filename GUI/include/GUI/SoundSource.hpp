#pragma once

#include "Component.hpp"
#include "Resource.hpp"
#include <Lunchbox/OpenAL.hpp>
#include <Lunchbox/core/sound.hpp>

class Music : public Resource, public OpenAL::Buffer {
  public:
    Music(const lunchbox::core::SoundData &sound)
        : OpenAL::Buffer(sound.wave, sound.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, sound.samplerate) {}
};

class SoundSource : public Component, public OpenAL::Source {
  public:
    SoundSource(const Music &music)
        : OpenAL::Source(music) {}
};
