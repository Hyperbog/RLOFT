#pragma once

#include <cstdint>
#include <optional>

namespace rloft {

// Windows virtual-key values are intentionally represented as constants
// here so the mapping and its tests remain portable C++17 (no windows.h).
// Returns the existing AutomapModalController action IDs rather than
// mutating the party or inventing a second map navigation model.
class EnhancedAutomapKeyboard final {
public:
    [[nodiscard]] static std::optional<int> action_for_key(
        std::uint32_t virtual_key, bool page) noexcept {
        // Original automap actions 0..3: row-,row+,column-,column+.
        // Existing actions 12..15 use the corresponding page steps.
        int base=-1;
        switch (virtual_key) {
            case 0x26U: base=0; break; // VK_UP
            case 0x28U: base=1; break; // VK_DOWN
            case 0x25U: base=2; break; // VK_LEFT
            case 0x27U: base=3; break; // VK_RIGHT
            default: return std::nullopt;
        }
        return page ? base+12 : base;
    }
};

} // namespace rloft
