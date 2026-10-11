#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace rloft_native {

// The original RES records contain color INDEXES. Their COLORS bank is
// selected by the CODE.1/CODE.2 CALL SITE and current world/scene state,
// not by an archive-wide default and not necessarily by the record itself.
enum class NativePaletteEvidence {
    ExactCallSite,
    LevelContext,
    ContextualPreview,
    Unresolved
};

struct NativePaletteDecision {
    std::optional<unsigned> bank;
    NativePaletteEvidence evidence{NativePaletteEvidence::Unresolved};
    std::vector<unsigned> alternatives;
    std::string reason;
};

// Without a known level, world resources use an explicitly non-authoritative
// bank 4 canonical preview; runtime world assets may legitimately change
// their displayed colors when the game changes levels.
[[nodiscard]] NativePaletteDecision resolve_native_palette(
    std::string_view archive, std::size_t record,
    std::optional<std::size_t> world_level = std::nullopt);

// CODE.2 DGROUP 81AC, first byte of each level display tuple; 0-based level.
[[nodiscard]] unsigned native_world_palette_bank(std::size_t level);

} // namespace rloft_native
