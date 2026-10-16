#include "rloft/combat_strength_attack.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {

CombatStrengthAttackModel CombatStrengthAttackModel::from_code2(
    const std::vector<std::uint8_t>& code2) {
    // CODE.2 LE object2 physical start 0x75F24; original C7E7
    // MOVSX EAX, BYTE PTR [EDX + DGROUP 6832].
    constexpr std::size_t start=0x75F24U+0x6832U;
    CombatStrengthAttackModel result{};
    if(code2.size()<start+result.table_.size())
        throw std::runtime_error(
            "original CODE.2 attack Strength adjustment table missing");
    for(std::size_t i=0;i<result.table_.size();++i)
        result.table_[i]=static_cast<std::int8_t>(code2[start+i]);
    return result;
}

int CombatStrengthAttackModel::score(
    std::uint8_t strength,std::uint8_t exceptional) const {
    // C7DA..C80A: only Strength exactly 18 AND a nonzero
    // exceptional score uses the C7EF three-range override.
    if(strength==18U && exceptional!=0U) {
        if(exceptional<=50U)return 1;
        if(exceptional<=99U)return 2;
        return 3;
    }
    if(strength>=table_.size())
        throw std::out_of_range(
            "C7C4 Strength outside original 26-byte table");
    return static_cast<int>(table_[strength]);
}

int CombatStrengthAttackModel::inspect(
    const PartyImage& party,std::size_t actor) const {
    if(actor>=PartyImage::kActorCount)
        throw std::out_of_range("C7C4 party actor");
    const auto at=actor*PartyImage::kActorSize;
    return score(party.bytes[at+0x33U],party.bytes[at+0x3FU]);
}

} // namespace rloft
