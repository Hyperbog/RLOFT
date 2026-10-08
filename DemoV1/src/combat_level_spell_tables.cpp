#include "rloft/combat_level_spell_tables.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kCode2Data=0x75F24U;
constexpr std::size_t kClass3de4At=kCode2Data+0x62B0U;
constexpr std::size_t kClass3ea4At=kCode2Data+0x6350U;
constexpr std::size_t kCountsAt=kCode2Data+0x6420U;
constexpr std::uint64_t kOriginalTablesFnv64=0x8C3BCDCF54B8DC23ULL;
constexpr std::size_t kFirstSpellAt=0xBAU;
constexpr std::size_t kRowStride=10U;

std::int32_t read_s32(const std::vector<std::uint8_t>& data,
                      std::size_t at) noexcept {
    const auto bits=static_cast<std::uint32_t>(data[at]) |
        (static_cast<std::uint32_t>(data[at+1U])<<8U) |
        (static_cast<std::uint32_t>(data[at+2U])<<16U) |
        (static_cast<std::uint32_t>(data[at+3U])<<24U);
    // All original group counts are small and positive.
    return static_cast<std::int32_t>(bits);
}

} // namespace

std::optional<CombatLevelSpellTables> CombatLevelSpellTables::from_code2(
    const std::vector<std::uint8_t>& code2) noexcept {
    constexpr auto tableBytes=kLevels*kGroups;
    if (code2.size()<kCountsAt+kGroups*4U ||
        code2.size()<kClass3ea4At+tableBytes ||
        code2.size()<kClass3de4At+tableBytes) return std::nullopt;
    // FNV64 over three non-contiguous original spans. Independent
    // SHA256 byte checks are owned by verify_combat_entry_bytes.py.
    std::uint64_t hash=0xCBF29CE484222325ULL;
    const auto mix=[&hash](std::uint8_t byte) {
        hash=(hash^byte)*0x100000001B3ULL;
    };
    for(std::size_t i=0U;i<tableBytes;++i) mix(code2[kClass3de4At+i]);
    for(std::size_t i=0U;i<tableBytes;++i) mix(code2[kClass3ea4At+i]);
    for(std::size_t i=0U;i<kGroups*4U;++i) mix(code2[kCountsAt+i]);
    if(hash!=kOriginalTablesFnv64) return std::nullopt;

    CombatLevelSpellTables out{};
    for(std::size_t level=0U;level<kLevels;++level) {
        for(std::size_t group=0U;group<kGroups;++group) {
            out.class_3de4_level_groups[level][group]=
                code2[kClass3de4At+level*kGroups+group];
            out.class_3ea4_level_groups[level][group]=
                code2[kClass3ea4At+level*kGroups+group];
        }
    }
    for(std::size_t group=0U;group<kGroups;++group) {
        const auto count=read_s32(code2,kCountsAt+group*4U);
        if(count<0 || count>static_cast<std::int32_t>(kRowStride))
            return std::nullopt;
        out.group_entry_counts[group]=count;
    }
    return out;
}

std::optional<CombatLevelSpellProjection> CombatLevelSpellModel::inspect(
    const PartyImage& observed_party,
    std::size_t actor,
    std::int32_t new_level,
    CombatLevelSpellRoutine routine,
    const CombatLevelSpellTables& tables) noexcept {
    if(actor>=PartyImage::kActorCount ||
       new_level<1 ||
       new_level>static_cast<std::int32_t>(CombatLevelSpellTables::kLevels))
        return std::nullopt;
    const auto tableLevel=static_cast<std::size_t>(new_level-1);
    for(const auto count:tables.group_entry_counts) {
        if(count<0 || count>static_cast<std::int32_t>(kRowStride))
            return std::nullopt;
    }
    CombatLevelSpellProjection out{};
    out.projected_party=observed_party;
    out.actor=actor;
    out.original_new_level=new_level;
    out.routine=routine;
    const auto actorBase=actor*PartyImage::kActorSize;

    // 3DE4:3E08..3E13 always sets actor BYTE +C2 to 11h.
    // 3EA4:3EB7..3ED2 sets +C1 to 11h, and also +C2 for
    // new levels >=4 (signed comparison after subtracting 1).
    if(routine==CombatLevelSpellRoutine::Original3DE4) {
        out.projected_party.bytes[actorBase+0xC2U]=0x11U;
    } else {
        out.projected_party.bytes[actorBase+0xC1U]=0x11U;
        if(new_level>=4)
            out.projected_party.bytes[actorBase+0xC2U]=0x11U;
    }
    const auto& levels=routine==CombatLevelSpellRoutine::Original3DE4
        ? tables.class_3de4_level_groups
        : tables.class_3ea4_level_groups;

    // 3DE4:3E1C..3E9A / 3EA4:3EFA..3F6B:
    // group 0..7, each actor row 10 bytes, count read as signed
    // DWORD from 6420+4*group. Only its first 'count' bytes
    // participate. Low nibble 0Fh is a unique sentinel.
    for(std::size_t group=0U;group<CombatLevelSpellTables::kGroups;++group) {
        const auto requiresClear=levels[tableLevel][group]!=0U;
        const auto count=static_cast<std::size_t>(tables.group_entry_counts[group]);
        for(std::size_t slot=0U;slot<count;++slot) {
            const auto at=actorBase+kFirstSpellAt+group*kRowStride+slot;
            auto& byte=out.projected_party.bytes[at];
            if(requiresClear) {
                if((byte&0x0FU)==0x0FU) {
                    byte=0x00U;
                    ++out.spell_bytes_written;
                }
            } else if((byte&0x0FU)!=0x0FU) {
                byte=0xFFU;
                ++out.spell_bytes_written;
            }
        }
    }
    return out;
}
} // namespace rloft
