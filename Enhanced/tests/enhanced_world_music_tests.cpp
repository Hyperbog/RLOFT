#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/enhanced_world_music.hpp"

#include <cassert>
#include <cstdint>

int main() {
    rloft::EnhancedWorldMusicState music;
    // Actual original Level22 +0x29C is zero: zero is a valid RES5 ID,
    // not a sentinel meaning turn music off.
    assert(music.should_start(22U,0U));
    for(int frame=0;frame<600;++frame) {
        assert(!music.should_start(22U,0U));
    }
    // A scripted cue can change while remaining on the same map.
    assert(music.should_start(22U,47U));
    assert(!music.should_start(22U,47U));
    // Reusing the same cue on a new authored level requires restarting
    // the track, including after a save/load transition.
    assert(music.should_start(23U,47U));
    assert(!music.should_start(23U,47U));
    // A cinematic takes exclusive control of XMIDI; resume afterward.
    music.invalidate();
    assert(music.should_start(23U,47U));
    assert(!music.should_start(23U,47U));
    music.invalidate();
    assert(music.should_start(22U,0U));
    return 0;
}
