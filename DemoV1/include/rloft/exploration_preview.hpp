#pragma once

#include <cstdint>
#include <optional>
#include <vector>

namespace rloft {

// Native-only nonpersistent preview mode. Do not serialize these synthetic
// characters to CHARS, and do not attribute them to original CODE.1 output.
struct ExplorationPreviewParty final {
    std::vector<std::uint8_t> chars;
    bool uses_existing_chars{};
    bool uses_original_save_party{};
};

class ExplorationPreview final {
public:
    // Existing exact 0x920-byte CHARS is never mutated. If absent/malformed,
    // supply a minimal isolated pair of actors for environmental exploration.
    [[nodiscard]] static ExplorationPreviewParty party(
        const std::optional<std::vector<std::uint8_t>>& existing,
        const std::optional<std::vector<std::uint8_t>>& original_save =
            std::nullopt);

    [[nodiscard]] static std::vector<std::uint8_t>
    synthetic_party();
};

} // namespace rloft
