#pragma once

#include <cstdint>
#include <optional>
#include <vector>

namespace rloft {

// Caller-owned, MUTABLE original DOS state. CODE.2 object-1 bytes
// are indexed at their LOGICAL offsets, excluding file base 3AF24h.
// This is not an arbitrary high-level sprite-color property.
struct CombatOriginalSpriteMachineState final {
    std::vector<std::uint8_t> writable_code_object1;
    // The two transparent branches explicitly write DWORD[F009]
    // AFTER the first sequencer OUT and BEFORE source copying.
    std::optional<std::uint32_t> transparent_row_count_f009;
};

// CODE.2:2A590 and 2A595 self-modify two separate instruction
// operands. They execute BEFORE 2AB38 and any sprite reads.
// This method checks instruction opcode prefixes at both sites
// before changing either 4-byte operand. Repatching is allowed.
class CombatOriginalSpriteCodePatchModel final {
public:
    [[nodiscard]] static bool patch_2a584(
        CombatOriginalSpriteMachineState& machine,
        std::uint32_t original_shader_table_pointer) noexcept;
};

} // namespace rloft
