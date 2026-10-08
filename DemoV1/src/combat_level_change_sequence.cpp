#include "rloft/combat_level_change_sequence.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>

namespace rloft {
namespace {
std::int16_t level_word(const PartyImage& p,std::size_t at) noexcept {
    const auto bits=static_cast<std::uint16_t>(
        static_cast<std::uint16_t>(p.bytes[at]) |
        (static_cast<std::uint16_t>(p.bytes[at+1U])<<8U));
    return static_cast<std::int16_t>(
        bits<=0x7FFFU ? static_cast<std::int32_t>(bits) :
        static_cast<std::int32_t>(bits)-0x10000);
}
}

std::optional<CombatLevelChangeSequence>
CombatLevelChangeSequenceModel::inspect(
    const PartyImage& observed_party,
    std::size_t actor,
    std::size_t changed_track,
    std::int16_t old_level,
    std::int16_t new_level) noexcept {
    if(actor>=PartyImage::kActorCount || old_level==new_level)
        return std::nullopt;
    const auto base=actor*PartyImage::kActorSize;
    // DDA4:DE0F..DE21 skips an ineligible actor before this
    // entire level-change branch can be reached. 50DC itself
    // returns -1 for that state, but the surrounding DDA4
    // sequence must NOT emit presentation/IRQ/HP events.
    const auto status=static_cast<std::uint16_t>(
        static_cast<std::uint16_t>(observed_party.bytes[base+0x10U]) |
        (static_cast<std::uint16_t>(observed_party.bytes[base+0x11U])<<8U));
    if ((status&0xF240U)!=0U)
        return std::nullopt;
    const auto tracks=1U+
        static_cast<unsigned>(observed_party.bytes[base+0x16U]!=0xFFU)+
        static_cast<unsigned>(observed_party.bytes[base+0x17U]!=0xFFU);
    if(changed_track>=tracks ||
       level_word(observed_party,base+0x20U+changed_track*2U)!=new_level)
        return std::nullopt;

    const auto selected=CombatLevelChangeGateModel::inspect(
        observed_party,actor,0);
    if(!selected) return std::nullopt;

    CombatLevelChangeSequence out{};
    out.actor=actor;
    out.changed_track=changed_track;
    out.old_level=old_level;
    out.new_level=new_level;
    out.selected_50dc_track=*selected;
    // DF1F: 50DC(0,actor), DF27 checks result signed >=0.
    out.events[out.count++]=CombatLevelChangeService::Select50DC;
    if(*selected>=0) {
        // DF2B/DF36: check actor FIRST class code, regardless of
        // which of the up to three class tracks was selected.
        out.events[out.count++]=
            observed_party.bytes[base+0x15U]==1U
                ? CombatLevelChangeService::Class3EA4
                : CombatLevelChangeService::Class3DE4;
    }
    // DF54..DF86: these calls still occur when 50DC returns -1,
    // and must not be skipped or reordered. In particular DC7C
    // consumes RNG and modifies character HP/state.
    out.events[out.count++]=CombatLevelChangeService::Format3210F;
    out.events[out.count++]=CombatLevelChangeService::Queue1C21C;
    out.events[out.count++]=CombatLevelChangeService::HpIncreaseDC7C;
    return out;
}

} // namespace rloft
