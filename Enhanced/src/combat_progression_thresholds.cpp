#include "rloft/combat_progression_thresholds.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kDataObjectStart=0x75F24U;
constexpr std::size_t kThresholdStart=kDataObjectStart+0x7D9CU;
constexpr std::size_t kCoefficientStart=kDataObjectStart+0x6827U;
constexpr std::size_t kThresholdSize=6U*20U*2U;
constexpr std::size_t kCoefficientSize=6U*2U;
constexpr std::uint64_t kOriginalTableFingerprint=0xA1AB0932C322B502ULL;

std::uint16_t read16(const PartyImage& p,std::size_t at) noexcept {
    return static_cast<std::uint16_t>(
        static_cast<std::uint16_t>(p.bytes[at]) |
        (static_cast<std::uint16_t>(p.bytes[at+1U])<<8U));
}

std::int16_t signed16(std::uint16_t value) noexcept {
    return static_cast<std::int16_t>(
        value<=0x7FFFU ? static_cast<std::int32_t>(value) :
        static_cast<std::int32_t>(value)-0x10000);
}

std::size_t track_count(const PartyImage& p,std::size_t base) noexcept {
    return 1U+
        static_cast<std::size_t>(p.bytes[base+0x16U]!=0xFFU)+
        static_cast<std::size_t>(p.bytes[base+0x17U]!=0xFFU);
}

} // namespace

std::optional<CombatProgressionTables> CombatProgressionTables::from_code2(
    const std::vector<std::uint8_t>& code2) noexcept {
    if (code2.size()<kThresholdStart+kThresholdSize ||
        code2.size()<kCoefficientStart+kCoefficientSize) {
        return std::nullopt;
    }
    // FNV64 is a version/fingerprint guard here, NOT a cryptographic
    // substitute for the canonical full CODE.2 SHA-256 audit.
    std::uint64_t hash=0xCBF29CE484222325ULL;
    const auto mix=[&hash](std::uint8_t byte) {
        hash=(hash^byte)*0x100000001B3ULL;
    };
    for (std::size_t i=0;i<kThresholdSize;++i)
        mix(code2[kThresholdStart+i]);
    for (std::size_t i=0;i<kCoefficientSize;++i)
        mix(code2[kCoefficientStart+i]);
    if (hash!=kOriginalTableFingerprint) return std::nullopt;

    CombatProgressionTables out{};
    for (std::size_t cls=0;cls<kClassCount;++cls) {
        for (std::size_t i=0;i<kThresholdCount;++i) {
            const auto at=kThresholdStart+cls*0x28U+i*2U;
            out.thresholds_in_thousands[cls][i]=
                static_cast<std::uint16_t>(code2[at] |
                    (static_cast<std::uint16_t>(code2[at+1U])<<8U));
        }
        out.attack_numerator[cls]=code2[kCoefficientStart+cls*2U];
        out.attack_denominator[cls]=code2[kCoefficientStart+cls*2U+1U];
        if (out.attack_denominator[cls]==0U) return std::nullopt;
    }
    return out;
}

std::optional<CombatXpLevelProjection> CombatXpLevelModel::inspect(
    const PartyImage& original_party,
    const CombatXpDistributionPlan& award,
    const CombatProgressionTables& tables) noexcept {
    CombatXpLevelProjection out{};
    std::size_t observed_eligible=0U;
    for (std::size_t a=0;a<PartyImage::kActorCount;++a) {
        const auto base=a*PartyImage::kActorSize;
        const bool eligible=(read16(original_party,base+0x10U)&0xF240U)==0U;
        if (eligible!=award.actors[a].eligible) return std::nullopt;
        if (!eligible) continue;
        ++observed_eligible;
        const auto count=track_count(original_party,base);
        if (count!=award.actors[a].class_track_count) return std::nullopt;
        for (std::size_t lane=0;lane<count;++lane) {
            const auto cls=original_party.bytes[base+0x15U+lane];
            // Original LEA into 7D9C+40*MOVSX(class) has no bounds
            // check; only six class records are proved class tables.
            if (cls>=CombatProgressionTables::kClassCount) {
                return std::nullopt;
            }
            const auto progress=award.actors[a].projected_track_progress[lane];
            const auto scaled=progress/1000U; // original unsigned DIV
            std::uint8_t level=0U;
            for (const auto threshold:tables.thresholds_in_thousands[cls]) {
                if (static_cast<std::uint32_t>(threshold)>scaled) break;
                ++level;
            }
            auto& entry=out.actors[a][lane];
            entry.visited=true;
            entry.class_code=cls;
            entry.projected_experience=progress;
            entry.original_level=signed16(
                read16(original_party,base+0x20U+lane*2U));
            entry.table_level=level;
            entry.requires_original_level_change_services=
                static_cast<std::int32_t>(entry.original_level)!=
                static_cast<std::int32_t>(level);
            out.has_unexecuted_level_change_services |=
                entry.requires_original_level_change_services;
        }
    }
    if (observed_eligible!=award.eligible_party_members)
        return std::nullopt;
    return out;
}

std::optional<CombatAttackTargetProjection>
CombatAttackTargetModel::inspect(
    const PartyImage& observed_party,
    std::size_t actor_index,
    const CombatProgressionTables& tables) noexcept {
    if (actor_index>=PartyImage::kActorCount) return std::nullopt;
    const auto base=actor_index*PartyImage::kActorSize;
    const auto count=track_count(observed_party,base);
    // DBD4 holds the first strict maximum signed level, so ties
    // preserve the first lane. If no level is positive, EDI is
    // uninitialized in original x86; decline to fabricate its value.
    std::int32_t best=0;
    std::size_t winner=0U;
    bool found=false;
    for (std::size_t i=0;i<count;++i) {
        const auto level=static_cast<std::int32_t>(
            signed16(read16(observed_party,base+0x20U+i*2U)));
        if (level>best) {
            best=level;
            winner=i;
            found=true;
        }
    }
    if (!found) return std::nullopt;
    const auto cls=observed_party.bytes[base+0x15U+winner];
    if (cls>=CombatProgressionTables::kClassCount ||
        tables.attack_denominator[cls]==0U) return std::nullopt;
    const auto quotient=best/tables.attack_denominator[cls];
    const auto adjusted=20-
        quotient*static_cast<std::int32_t>(tables.attack_numerator[cls]);
    CombatAttackTargetProjection out{};
    out.winning_class_track=winner;
    out.winning_class_level=static_cast<std::int16_t>(best);
    out.class_code=cls;
    // DFE3 MOV BYTE truncates the original signed result to 8 bits.
    out.stored_target_byte=static_cast<std::uint8_t>(adjusted);
    return out;
}

} // namespace rloft
