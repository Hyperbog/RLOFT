#include "rloft/combat_attack_timing.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {

std::uint32_t read_u32(const std::uint8_t* bytes) noexcept {
    return static_cast<std::uint32_t>(bytes[0]) |
           (static_cast<std::uint32_t>(bytes[1])<<8U) |
           (static_cast<std::uint32_t>(bytes[2])<<16U) |
           (static_cast<std::uint32_t>(bytes[3])<<24U);
}

} // namespace

CombatAttackTimingPlan CombatAttackTimingModel::inspect(
    const PartyImage& party,
    const ObjectCatalog& objects,
    std::size_t actor,
    std::size_t hand,
    std::int32_t dispatched_item_id,
    std::uint32_t gameplay_tick_a7e4_before_hit_write,
    std::uint32_t gameplay_tick_a7e4_after_presentation,
    bool hit_succeeded,
    std::int32_t computed_damage) {
    if (actor>=PartyImage::kActorCount || hand>=2U) {
        throw std::out_of_range("CODE.2 E5F0 actor/hand");
    }
    const auto* rec =
        party.bytes.data()+actor*PartyImage::kActorSize;
    // E5F0:1E5F7 uses the third dispatched call argument, not
    // a second potentially stale read of the live party hand.
    const auto item=dispatched_item_id;
    if (item<-1) {
        throw std::out_of_range("CODE.2 E5F0 invalid hand item");
    }

    // E5F0:1E9D0..1EA0D: unarmed (-1) and the temporary spell
    // hands (7Eh..86h) use exactly 0x12 (18) maintenance ticks.
    // Other items read the full WORD at OBJECTS record +35h.
    // The original SHL EAX,10 / SHR AX,12 extracts bits 2..5,
    // then LEA*8 + ADD forms 9 * ((word>>2)&15).
    std::uint32_t interval=0x12U;
    if (item>=0 && !(item>=0x7E && item<=0x86)) {
        const auto word=objects.at(
            static_cast<std::size_t>(item)).raw_word_35;
        interval=9U*((static_cast<std::uint32_t>(word)>>2U)&0x0FU);
    }

    // E5F0:1EA15..1EA2C: DWORD actor +132h mask
    // 0x01000010 halves the duration (integer truncate toward 0).
    // All currently derived durations are nonnegative.
    const auto flags=read_u32(rec+0x132U);
    if ((flags&0x01000010U)!=0U) interval/=2U;

    CombatAttackTimingPlan result{};
    result.animation_interval=interval;

    // E5F0:EA2E..EA5C: only a C80C hit reads 29CC4(A7E4)
    // and writes selected actor +19E hand-ready and +1AA damage.
    if (hit_succeeded) {
        result.hand_ready_deadline=
            gameplay_tick_a7e4_before_hit_write+(interval/2U);
        result.stored_damage=computed_damage;
    }

    // E5F0:EA63 invokes CODE.2 logical 24B48 (objdump 34B48).
    // 24B48 presents the action graphics first, THEN, if the
    // interval is nonzero, reads the current A7E4 and writes
    // actor+10B action-state = current tick + FULL interval.
    // This happens even after a miss or no target. The second
    // clock observation may differ from the earlier hit-only one.
    if (interval!=0U) {
        result.action_state_deadline=
            gameplay_tick_a7e4_after_presentation+interval;
    }
    return result;
}

} // namespace rloft
