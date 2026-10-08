#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

#include "rloft/exploration_state.hpp"

namespace rloft {

// DDA4:DF54 uses the originally serialized CODE.2 DWORD[67E4]
// format pointer, which points to "%s gains a level." at DGROUP:548.
// 3210F delegates to 385B0 then writes a terminating NUL at
// destination[emitted_count]. This is the bounded ONE-format path,
// not a replacement for the full variadic 385B0 formatter.
struct CombatLevelMessageProjection final {
    std::size_t formatted_byte_count{};
    std::vector<std::uint8_t> formatted_c_string; // including final NUL
    std::vector<std::uint8_t> projected_destination; // remaining bytes kept
};

class CombatLevelMessageModel final {
public:
    // Requires the matching original CODE.2 format pointer and bytes,
    // an in-record NUL-terminated actor name, and a captured writable
    // DGROUP:759C buffer span large enough for the result. If any
    // source detail is unsupported, the result is nullopt; never
    // truncate a genuine original formatter output silently.
    [[nodiscard]] static std::optional<CombatLevelMessageProjection> inspect(
        const PartyImage& observed_party,
        std::size_t actor_index,
        const std::vector<std::uint8_t>& original_code2,
        const std::vector<std::uint8_t>& destination_759c);
};

} // namespace rloft
