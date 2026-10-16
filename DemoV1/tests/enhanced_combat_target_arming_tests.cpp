#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/enhanced_combat_target_arming.hpp"
#include <cassert>
#include <optional>
#include <stdexcept>

int main() {
    using rloft::EnhancedCombatTargetArming;
    using rloft::EnhancedCombatSceneStamp;
    EnhancedCombatTargetArming state{};
    const EnhancedCombatSceneStamp seen{42U,22U,
        0x00118000U,0x00078000U,0xD5U};
    assert(!state.take(seen).consumed);
    state.arm(2U,1U,seen);
    assert(state.pending());
    const auto attack=state.take(seen);
    assert(attack.consumed && attack.valid);
    assert(attack.actor==2U && attack.hand==1U);
    assert(!state.pending());
    assert(!state.take(seen).consumed); // no double strike

    // Every source-view dimension independently makes the armed
    // action invalid. The target click is STILL consumed: it may
    // never fall through into the original 21704 floor pickup.
    for(unsigned n=0U;n<5U;++n) {
        auto changed=seen;
        if(n==0U)++changed.world_revision;
        if(n==1U)++changed.level_id;
        if(n==2U)++changed.coordinate_a;
        if(n==3U)++changed.coordinate_b;
        if(n==4U)++changed.facing;
        state.arm(0U,0U,seen);
        const auto stale=state.take(changed);
        assert(stale.consumed && !stale.valid);
        assert(!state.pending());
        assert(!state.take(changed).consumed);
    }
    state.arm(3U,0U,seen);
    const auto no_frame=state.take(std::nullopt);
    assert(no_frame.consumed && !no_frame.valid);
    // Entering a menu/panel or changing level abandons the selection.
    state.arm(0U,0U,seen);
    state.cancel();
    assert(!state.pending());
    assert(!state.take(seen).consumed);
    bool rejected=false;
    try{state.arm(4U,0U,seen);}
    catch(const std::out_of_range&){rejected=true;}
    assert(rejected && !state.pending());
    rejected=false;
    try{state.arm(0U,2U,seen);}
    catch(const std::out_of_range&){rejected=true;}
    assert(rejected && !state.pending());
}
