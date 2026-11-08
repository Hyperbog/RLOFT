#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "rloft/object_catalog.hpp"

namespace rloft {

// Original CODE.2 22510..225DF: construct the message passed to
// 1C21C for a picked family-C object. This is SOURCE DATA and original
// field ordering, not a replacement for 1C258/3060C VGA rendering.
class OriginalPickupRibbonTextModel final {
public:
    // The original CODE.2 DGROUP D67C stores a pointer into its data
    // object; shipped baseline points at 11B4 ("You pick up ").
    // Do not bake the English string into native game code.
    [[nodiscard]] static std::optional<std::vector<std::uint8_t>>
    source_prefix_from_code2(
        const std::vector<std::uint8_t>& code2);

    // Exact 22510..225DA no-suffix path. ID name comes from original
    // OBJECTS record +0 (0x41-byte record). If known and +37 != 0,
    // 2257C appends 11C4 and the result of 2E6F0: that path is not
    // yet reconstructed and must NOT be guessed.
    [[nodiscard]] static std::optional<std::vector<std::uint8_t>>
    compose_unenchanted(
        const std::vector<std::uint8_t>& original_prefix,
        const ObjectDefinition& object,
        bool original_known_bit);
};

} // namespace rloft
