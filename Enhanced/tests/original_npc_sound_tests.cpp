#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_npc_sound.hpp"
#include <cassert>
#include <stdexcept>
#include <vector>

int main() {
    std::vector<std::uint8_t> init(0x20U,0U);
    // Actual supplied INIT: creature 69 and death 166.
    init[0x14U]=69U;
    init[0x16U]=166U;
    rloft::NpcDamageResult event{};
    assert(!rloft::OriginalNpcSoundModel::res6_id(init,event));
    event.sound_kind=rloft::NpcSoundKind::DamageReaction;
    event.sound_id=0U;
    assert(rloft::OriginalNpcSoundModel::res6_id(init,event)==69U);
    event.sound_id=96U;
    assert(rloft::OriginalNpcSoundModel::res6_id(init,event)==165U);
    event.sound_kind=rloft::NpcSoundKind::Death;
    event.sound_id=0U;
    assert(rloft::OriginalNpcSoundModel::res6_id(init,event)==166U);
    event.sound_id=90U;
    assert(rloft::OriginalNpcSoundModel::res6_id(init,event)==256U);
    bool invalid=false;
    try { (void)rloft::OriginalNpcSoundModel::res6_id({},event); }
    catch(const std::runtime_error&){invalid=true;}
    assert(invalid);
    event.sound_kind=rloft::NpcSoundKind::None;
    assert(!rloft::OriginalNpcSoundModel::res6_id({},event));
    // Source INIT is authoritative: not a hardcoded 69/166 in
    // production. Validate changing both backing bank words.
    init[0x14U]=31U;init[0x16U]=99U;
    event.sound_kind=rloft::NpcSoundKind::DamageReaction;
    event.sound_id=6U;
    assert(rloft::OriginalNpcSoundModel::res6_id(init,event)==37U);
    event.sound_kind=rloft::NpcSoundKind::Death;
    event.sound_id=0U;
    assert(rloft::OriginalNpcSoundModel::res6_id(init,event)==99U);
}
