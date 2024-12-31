#include "sound.hpp"
#include "GUI.hpp"

SoundSource::SoundSource(const Music &music)
    : OpenAL::Source(music) {
    gui().audio.sources.request_set(this);
    gui().audio.adjust_overall_gain(); // ここで音量調整することで突発ノイズを防げる。
}

SoundSource::~SoundSource() {
    gui().audio.sources.request_erase(this);
    gui().audio.adjust_overall_gain(); // ここでも調整しないと、音源が消えていくたびに総音量が目減りしてしまう。
    // OPTIMIZE: しかし、些細な効果音1個ごとに毎回音量を調整するのは流石にパフォーマンスが心配。ボトルネックにならなければokだが。
}
