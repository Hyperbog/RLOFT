#include "rloft/combat_hand_predicates.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {

bool CombatHandPredicates::has_class(
    const PartyImage& party,
    std::size_t actor,
    std::int32_t class_code) {
    if (actor >= PartyImage::kActorCount) {
        throw std::out_of_range("CODE.2 23EAC actor");
    }
    const auto base = actor * PartyImage::kActorSize;
    // CODE.2 23EB4,23EC1,23ECE MOVSX three separate signed bytes.
    for (std::size_t i = 0U; i < 3U; ++i) {
        const auto signed_class = static_cast<std::int32_t>(
            static_cast<std::int8_t>(party.bytes[base + 0x15U + i]));
        if (signed_class == class_code) return true;
    }
    return false;
}

bool CombatHandPredicates::is_pair_exception(
    std::int32_t object_id) noexcept {
    // CODE.2 23EE4..23F32: ordered equality tests, NOT a category.
    switch (object_id) {
        case 0x2B:
        case 0x59:
        case 0x68:
        case 0x9F:
        case 0x102:
        case 0x103:
            return true;
        default:
            return false;
    }
}

bool CombatHandPredicates::is_attack_hand_item(
    std::int32_t object_id,
    const ObjectCatalog& catalog) {
    // The original helper assumes a valid nonnegative catalog ID
    // outside the pseudo-range; do not probe OBJECTS[-1].
    if (object_id < 0) {
        throw std::out_of_range(
            "CODE.2 23E58 requires a nonnegative object ID");
    }
    if (object_id >= 0x7E && object_id <= 0x86) return true;
    // Original 23E80 reads item_category from OBJECTS record +2Fh.
    const auto category = catalog.at(
        static_cast<std::size_t>(object_id)).item_category;
    return category == 9U ||
           category == 10U ||
           category == 11U ||
           object_id == 0x30;
}

} // namespace rloft
