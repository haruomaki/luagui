#include "sound.hpp"
#include "GUI.hpp"

SoundSource::SoundSource(const Music &music)
    : OpenAL::Source(music) {
    gui().audio.sources.request_set(this);
}

SoundSource::~SoundSource() {
    gui().audio.sources.request_erase(this);
}
