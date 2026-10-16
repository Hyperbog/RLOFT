#include "rloft/original_npc_sound.hpp"
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>

namespace rloft {

std::optional<std::uint16_t> OriginalNpcSoundModel::res6_id(
    const std::vector<std::uint8_t>& init,
    const NpcDamageResult& result) {
    if(result.sound_kind==NpcSoundKind::None)
        return std::nullopt;
    // Object1 16E3A writes INIT+14 into 258C0, and 16E67 writes
    // INIT+16 into 258BC. This is NOT the adjacent +16/+18 pair.
    const std::size_t at=
        result.sound_kind==NpcSoundKind::DamageReaction?0x14U:0x16U;
    if(init.size()<at+2U)
        throw std::runtime_error("original INIT NPC sound bank truncated");
    const auto base=static_cast<std::uint32_t>(init[at]) |
        (static_cast<std::uint32_t>(init[at+1U])<<8U);
    const auto res6_id=base+result.sound_id;
    if(res6_id>std::numeric_limits<std::uint16_t>::max())
        throw std::out_of_range("original NPC sound RES6 id overflow");
    // Death ID zero STILL queues the bank base. Do not suppress it.
    return static_cast<std::uint16_t>(res6_id);
}
} // namespace rloft
