#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_irq_display.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <vector>

int main() {
    using rloft::CombatIrqDisplayAction;
    using rloft::CombatIrqDisplayModel;
    using rloft::CombatIrqDisplaySnapshot;
    using A=CombatIrqDisplayAction;
    const auto inspect=[](std::uint8_t pending,
                          std::uint32_t deadline,
                          std::uint32_t after_draw_tick) {
        return CombatIrqDisplayModel::inspect(
            CombatIrqDisplaySnapshot{pending,deadline,after_draw_tick});
    };
    // Newly queued message is drawn BEFORE the IRQ check. The
    // original presentation calls occur even when tick == deadline.
    auto plan=inspect(1U,190U,190U);
    assert(plan.drew_new_message && !plan.erased_expired_message);
    assert(plan.resulting_pending_cf59==0U);
    assert(plan.resulting_deadline_cf54==190U);
    assert((plan.ordered_actions==std::vector<A>{
        A::Call291B0WithLowByteC050,
        A::Call2939CWith2222C,
        A::Call3060CWith14580,
        A::Call1A8C8WithDwordC050,
        A::ClearPendingCF59,
        A::Read30700IrqCounter,
        A::CompareUnsignedAgainstCF54
    }));

    // If rendering itself takes long enough for tick > deadline,
    // original 1C2C4 draws THEN erases in the SAME invocation.
    plan=inspect(1U,190U,191U);
    assert(plan.drew_new_message && plan.erased_expired_message);
    assert(plan.ordered_actions.size()==11U);
    assert(plan.ordered_actions[4]==A::ClearPendingCF59);
    assert(plan.ordered_actions[5]==A::Read30700IrqCounter);
    assert(plan.ordered_actions[6]==A::CompareUnsignedAgainstCF54);
    assert(plan.ordered_actions[7]==A::Call291B0WithLowByteC050);
    assert(plan.ordered_actions[8]==A::Call2939CWith2222C);
    assert(plan.ordered_actions[9]==A::StoreDeadlineCF54FFFFFFFF);
    assert(plan.ordered_actions[10]==A::Call1A8C8WithDwordC050);
    assert(plan.resulting_deadline_cf54==0xFFFFFFFFU);

    // A timer can expire with no currently pending presentation.
    // No 3060C text draw is performed by the clearing path.
    plan=inspect(0U,190U,191U);
    assert(!plan.drew_new_message && plan.erased_expired_message);
    assert((plan.ordered_actions==std::vector<A>{
        A::Read30700IrqCounter,
        A::CompareUnsignedAgainstCF54,
        A::Call291B0WithLowByteC050,
        A::Call2939CWith2222C,
        A::StoreDeadlineCF54FFFFFFFF,
        A::Call1A8C8WithDwordC050
    }));
    assert(plan.resulting_pending_cf59==0U);
    plan=inspect(0U,190U,190U);
    assert(!plan.drew_new_message && !plan.erased_expired_message);
    assert(plan.ordered_actions.size()==2U);

    // The original code checks NONZERO pending, not exactly one.
    plan=inspect(0x80U,30U,29U);
    assert(plan.drew_new_message);
    assert(plan.resulting_pending_cf59==0U);

    // FFFFFFFF is the original cleared-deadline sentinel: no
    // 32-bit unsigned IRQ tick is ever greater than it.
    plan=inspect(1U,0xFFFFFFFFU,0xFFFFFFFFU);
    assert(plan.drew_new_message && !plan.erased_expired_message);
    assert(plan.resulting_deadline_cf54==0xFFFFFFFFU);
    plan=inspect(0U,0xFFFFFFFFU,0U);
    assert(plan.ordered_actions.size()==2U);

    // Original unsigned compare is deliberately NOT robust against
    // wrap: a fresh rolled-over deadline 0x4A can appear expired
    // at tick FFFFFFF1. Retain original quirks.
    plan=inspect(0U,0x4AU,0xFFFFFFF1U);
    assert(plan.erased_expired_message);
    plan=inspect(0U,0x4AU,0x4AU);
    assert(!plan.erased_expired_message);
}
