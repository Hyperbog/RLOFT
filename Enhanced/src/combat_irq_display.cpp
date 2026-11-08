#include "rloft/combat_irq_display.hpp"

#include <cstdint>

namespace rloft {

CombatIrqDisplayPlan CombatIrqDisplayModel::inspect(
    const CombatIrqDisplaySnapshot& observed) {
    using Action=CombatIrqDisplayAction;
    CombatIrqDisplayPlan out{};
    out.resulting_pending_cf59=observed.pending_cf59;
    out.resulting_deadline_cf54=observed.deadline_cf54;

    // 1C258..1C2C1 is called first, without checking CF54.
    if(observed.pending_cf59!=0U) {
        out.drew_new_message=true;
        out.ordered_actions.push_back(Action::Call291B0WithLowByteC050);
        // 2939C(0, 0x106, 0x20, DWORD[2222C])
        out.ordered_actions.push_back(Action::Call2939CWith2222C);
        // 3060C(0x107, 0x22, DGROUP:14580, 1)
        out.ordered_actions.push_back(Action::Call3060CWith14580);
        // 1A8C8(DWORD[C050],0x106,0x20,0x0C,0x100)
        out.ordered_actions.push_back(Action::Call1A8C8WithDwordC050);
        // Original writes CF59=0 only AFTER 1A8C8 returns.
        out.ordered_actions.push_back(Action::ClearPendingCF59);
        out.resulting_pending_cf59=0U;
    }

    // 1C2C4's 1C258 call returns BEFORE 30700 is invoked.
    out.ordered_actions.push_back(Action::Read30700IrqCounter);
    out.ordered_actions.push_back(Action::CompareUnsignedAgainstCF54);

    // 1C2D7 JBE skips expiration if tick <= deadline,
    // including when deadline==FFFFFFFF. This is NOT a
    // wrap-aware subtraction of elapsed clock ticks.
    if(observed.irq_tick_at_expiry_check>observed.deadline_cf54) {
        out.erased_expired_message=true;
        out.ordered_actions.push_back(Action::Call291B0WithLowByteC050);
        out.ordered_actions.push_back(Action::Call2939CWith2222C);
        // Store sentinel BEFORE final compositing call 1A8C8.
        out.ordered_actions.push_back(Action::StoreDeadlineCF54FFFFFFFF);
        out.resulting_deadline_cf54=0xFFFFFFFFU;
        out.ordered_actions.push_back(Action::Call1A8C8WithDwordC050);
    }
    return out;
}

} // namespace rloft
