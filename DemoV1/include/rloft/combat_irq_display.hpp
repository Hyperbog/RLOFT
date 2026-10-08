#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// CODE.2 logical 1C258..1C326: an ordered event trace, not
// a simulated VGA renderer. Host execution must re-read original
// C050 and 2222C globals at each deferred call site.
enum class CombatIrqDisplayAction : std::uint8_t {
    Call291B0WithLowByteC050,
    Call2939CWith2222C,
    Call3060CWith14580,
    Call1A8C8WithDwordC050,
    ClearPendingCF59,
    Read30700IrqCounter,
    CompareUnsignedAgainstCF54,
    StoreDeadlineCF54FFFFFFFF,
};

struct CombatIrqDisplaySnapshot final {
    std::uint8_t pending_cf59{};
    std::uint32_t deadline_cf54{0xFFFFFFFFU};
    // Taken specifically at CODE.2 1C2CC, AFTER any 1C258
    // display calls, not sampled before deferred rendering.
    std::uint32_t irq_tick_at_expiry_check{};
};

struct CombatIrqDisplayPlan final {
    std::vector<CombatIrqDisplayAction> ordered_actions;
    bool drew_new_message{};
    bool erased_expired_message{};
    std::uint8_t resulting_pending_cf59{};
    std::uint32_t resulting_deadline_cf54{0xFFFFFFFFU};
};

class CombatIrqDisplayModel final {
public:
    // Original 1C2C4 calls 1C258, even when CF59 is zero, then
    // reads IRQ 30700 and expires only on unsigned tick > deadline.
    // This records each external renderer call and write in order.
    // No event is executed and no framebuffer bytes are changed.
    [[nodiscard]] static CombatIrqDisplayPlan inspect(
        const CombatIrqDisplaySnapshot& observed);
};

} // namespace rloft
