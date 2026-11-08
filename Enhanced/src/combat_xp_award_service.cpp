#include "rloft/combat_xp_award_service.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>

namespace rloft {
namespace {
void write_dword(PartyImage& party,std::size_t at,
                 std::uint32_t value) noexcept {
    party.bytes[at]=static_cast<std::uint8_t>(value);
    party.bytes[at+1U]=static_cast<std::uint8_t>(value>>8U);
    party.bytes[at+2U]=static_cast<std::uint8_t>(value>>16U);
    party.bytes[at+3U]=static_cast<std::uint8_t>(value>>24U);
}
} // namespace

std::optional<CombatXpAwardResult> CombatXpAwardService::apply(
    PartyImage& party,
    std::uint16_t original_npc_award,
    const CombatProgressionTables& source_tables) noexcept {
    // DDA4 first determines which original actor records participate,
    // rounds the WORD award to actors, and divides again by the count
    // of that actor's original class tracks.
    const auto shares=CombatXpDistributionModel::inspect(
        party,original_npc_award);
    if(!shares)return std::nullopt;
    // Validate all eligible class bytes against the EXACT shipped
    // 6x20 class thresholds BEFORE changing even the first actor.
    // A level change requires original 50DC follow-on services, not
    // just a naive update to the actor's level byte/HP.
    const auto projection=CombatXpLevelModel::inspect(
        party,*shares,source_tables);
    if(!projection)return std::nullopt;

    CombatXpAwardResult result{};
    result.distribution=*shares;
    result.level_projection=*projection;
    result.level_change_services_pending=
        projection->has_unexecuted_level_change_services;

    // Original DEBC..DEC2 performs unsigned DWORD additions to the
    // XP lanes at +26/+2A/+2E. This projection already preserves x86
    // 32-bit overflow. Only actual DDA4-eligible class lanes change.
    for(std::size_t actor=0;actor<PartyImage::kActorCount;++actor) {
        const auto& award=shares->actors[actor];
        if(!award.eligible)continue;
        for(std::size_t lane=0;lane<award.class_track_count;++lane) {
            const auto base=actor*PartyImage::kActorSize;
            write_dword(party,base+0x26U+lane*4U,
                award.projected_track_progress[lane]);
            ++result.updated_tracks;
        }
    }
    // DO NOT fabricate original level-up side effects here, even if a
    // class crosses a threshold. The accumulated XP is persisted in
    // the original class fields, so future implementation can resolve
    // the outstanding original level services from saved records.
    return result;
}

} // namespace rloft
