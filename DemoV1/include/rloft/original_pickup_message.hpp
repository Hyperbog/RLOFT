#pragma once

#include <cstdint>
#include <optional>

namespace rloft {

// CODE.2 21767..2187E: independent assignments to DGROUP BF44
// AFTER 21704 has placed the family-C object in the live hand.
// No matching branch means BF44 retains its prior value.
class OriginalPickupMessageModel final {
public:
    [[nodiscard]] static std::optional<std::uint32_t> assignment_21704(
        std::uint16_t picked_object,
        bool party_has_tome_35,
        bool persistent_event_51_set) noexcept {
        // 21767..21786: parchments 61h..64h with Tome 35h.
        if(picked_object>=0x61U && picked_object<=0x64U &&
           party_has_tome_35)
            return 0x0FU;
        // Sequential independent comparisons, not a generic
        // item-category hint; original BF44 is overwritten only
        // when a matching branch actually executes.
        if(picked_object==0x49U) return 0x25U;
        if(picked_object==0x9EU) return 0x36U;
        if(picked_object==0x5FU) return 0x37U;
        if(picked_object==0x8BU) return 0x38U;

        // F2B4 checks only item 10h or 5Fh, but the subsequent
        // 217F9 branch excludes 5Fh. Thus ONLY 10h uses 39h.
        if(picked_object==0x10U) return 0x39U;
        if(picked_object>=0xA7U && picked_object<=0xC9U)
            return 0x3AU;
        if(picked_object==0x77U) return 0x3CU;
        if(picked_object==0x75U) return 0x29U;
        // 2185A..2187E reads event bit 51h via 1C1DC.
        if(picked_object==0x74U && persistent_event_51_set)
            return 0x2FU;
        return std::nullopt;
    }
};

} // namespace rloft
