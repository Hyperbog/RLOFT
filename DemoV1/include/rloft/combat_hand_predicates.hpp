#pragma once

#include <cstddef>
#include <cstdint>

#include "rloft/exploration_state.hpp"
#include "rloft/object_catalog.hpp"

namespace rloft {

// Original CODE.2 23EAC / 23EE4 / 23E58 independent predicates used by
// the larger 24478 two-hand legality routine. A true predicate is NOT
// overall attack approval: 240A8 and the rest of 24478 remain pending.
class CombatHandPredicates final {
public:
    // 23EAC: compare signed class bytes actor +15h/+16h/+17h.
    [[nodiscard]] static bool has_class(
        const PartyImage& party,
        std::size_t actor,
        std::int32_t class_code);

    // 23EE4: six hardcoded object IDs.
    [[nodiscard]] static bool is_pair_exception(
        std::int32_t object_id) noexcept;

    // 23E58: pseudo-hand range 7Eh..86h OR category 9/10/11
    // OR exact object 30h. Caller must supply a real (>=0) ID.
    [[nodiscard]] static bool is_attack_hand_item(
        std::int32_t object_id,
        const ObjectCatalog& catalog);
};

} // namespace rloft
