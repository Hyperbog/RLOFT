#include "rloft/combat_level_up_hp.hpp"
#include "rloft/original_random.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kDataBase=0x75F24U;
constexpr std::size_t kConTable=kDataBase+0x67FAU;
constexpr std::size_t kSidesTable=kDataBase+0x6821U;
constexpr std::uint64_t kOriginalTablesFnv64=0xBBACFA0D87AE4EF1ULL;

std::uint16_t word(const PartyImage& p,std::size_t offset) noexcept {
    return static_cast<std::uint16_t>(
        static_cast<std::uint16_t>(p.bytes[offset]) |
        (static_cast<std::uint16_t>(p.bytes[offset+1U])<<8U));
}

std::int8_t signed_byte(std::uint8_t v) noexcept {
    return static_cast<std::int8_t>(
        v<=127U ? static_cast<std::int32_t>(v) :
                  static_cast<std::int32_t>(v)-256);
}
} // namespace

std::optional<CombatLevelUpHpTables> CombatLevelUpHpTables::from_code2(
    const std::vector<std::uint8_t>& code2) noexcept {
    if (code2.size()<kSidesTable+kClassCount ||
        code2.size()<kConTable+2U*kConstitutionValues)
        return std::nullopt;

    // FNV guards the exact 38+6 original data bytes; the offline
    // executable verifier independently checks both SHA256 digests.
    std::uint64_t hash=0xCBF29CE484222325ULL;
    const auto mix=[&hash](std::uint8_t v) {
        hash=(hash^v)*0x100000001B3ULL;
    };
    for(std::size_t i=0;i<2U*kConstitutionValues;++i)
        mix(code2[kConTable+i]);
    for(std::size_t i=0;i<kClassCount;++i)
        mix(code2[kSidesTable+i]);
    if(hash!=kOriginalTablesFnv64) return std::nullopt;

    CombatLevelUpHpTables out{};
    for(std::size_t group=0;group<2U;++group) {
        for(std::size_t con=0;con<kConstitutionValues;++con)
            out.constitution_bonus[group][con]=signed_byte(
                code2[kConTable+group*kConstitutionValues+con]);
    }
    for(std::size_t i=0;i<kClassCount;++i) {
        const auto sides=code2[kSidesTable+i];
        if(sides==0U) return std::nullopt;
        out.die_sides[i]=sides;
    }
    return out;
}

std::optional<CombatLevelUpHpProjection> CombatLevelUpHpModel::inspect(
    const PartyImage& party,
    std::size_t actor,
    std::size_t lane,
    std::int32_t level,
    std::uint32_t rng_state,
    const CombatLevelUpHpTables& tables) noexcept {
    if(actor>=PartyImage::kActorCount || lane>=3U)
        return std::nullopt;
    const auto base=actor*PartyImage::kActorSize;
    const auto cls=party.bytes[base+0x15U+lane];
    const auto con=party.bytes[base+0x37U];
    // The original does unchecked pointer arithmetic. Values outside
    // its six-entry CS jump table / two 19-byte Constitution rows
    // cannot be modeled as safe, invented array defaults.
    if(cls>=CombatLevelUpHpTables::kClassCount ||
       con>=CombatLevelUpHpTables::kConstitutionValues ||
       tables.die_sides[cls]==0U)
        return std::nullopt;

    // DC8C..DCA9: 1C328(1, class_die_sides, 0, 0)
    // consumes exactly one original 30CA2 draw.
    OriginalRandom rng(rng_state);
    const auto rolled=rng.dice(1U,tables.die_sides[cls]);
    // DCB3..DCD4: signed class / 3 selects the 19-byte
    // Constitution adjustment bank; the CON value is MOVZX.
    const auto mod=static_cast<std::int32_t>(
        tables.constitution_bonus[cls/3U][con]);

    std::int32_t hp=0;
    // CS:DC64 six-way original class-specific dispatch.
    // Classes 0..2,3 switch at level10; classes 4,5 at level11.
    if(cls<=2U) {
        hp=level<10 ? static_cast<std::int32_t>(rolled)+mod : mod+3;
    } else if(cls==3U) {
        hp=level<10 ? static_cast<std::int32_t>(rolled)+mod : mod+2;
    } else if(cls==4U) {
        hp=level<11 ? static_cast<std::int32_t>(rolled)+mod : mod+1;
    } else {
        hp=level<11 ? static_cast<std::int32_t>(rolled)+mod : mod+2;
    }
    // DD3F..DD49: clamp BEFORE the class-track split; after
    // a three-way split, a zero HP increase is possible.
    if(hp<1) hp=1;

    // DD51..DD84: the source checks the second and third
    // SIGNED class code bytes for -1, not a normalized
    // count based on valid class combinations.
    if(party.bytes[base+0x16U]!=0xFFU) {
        hp/=party.bytes[base+0x17U]!=0xFFU ? 3 : 2;
    }

    CombatLevelUpHpProjection out{};
    out.class_code=cls;
    out.constitution=con;
    out.die_sides=tables.die_sides[cls];
    out.original_die_roll=rolled;
    out.constitution_modifier=static_cast<std::int16_t>(mod);
    out.original_level_argument=level;
    out.final_hp_increase=static_cast<std::int16_t>(hp);
    out.current_hp_before=word(party,base+0x1CU);
    out.max_hp_before=word(party,base+0x1EU);
    // DD8E/DD95: two WORD ADD instructions; preserve 16-bit wrap.
    out.current_hp_after=static_cast<std::uint16_t>(
        static_cast<std::uint32_t>(out.current_hp_before)+
        static_cast<std::uint32_t>(hp));
    out.max_hp_after=static_cast<std::uint16_t>(
        static_cast<std::uint32_t>(out.max_hp_before)+
        static_cast<std::uint32_t>(hp));
    out.rng_state_after=rng.state();
    return out;
}

} // namespace rloft
