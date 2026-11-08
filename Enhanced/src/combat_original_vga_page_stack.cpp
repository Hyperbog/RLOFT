#include "rloft/combat_original_vga_page_stack.hpp"

#include "rloft/combat_irq_display_runtime.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {
void select_291b0(
    CombatOriginalVgaPageStackState& state,
    std::uint8_t page,
    const CombatOriginalVgaPageStackServices& services) {
    // 291B0 calls 29FEC -> 2AB6C before setting EF4D.
    // The latter computes EFE9 and F001 from original F005,
    // masking page to one byte and wrapping all DWORD arithmetic.
    const auto live_stride=services.observed_stride_f005_before_291b0
        ? services.observed_stride_f005_before_291b0()
        : state.page_stride_f005;
    // Record the actual F005 observation, not a stale previous
    // stride from before 2F580's diagnostic callback.
    state.page_stride_f005=live_stride;
    const auto projection=CombatVgaPageSelectionModel::inspect(
        page,live_stride);
    state.selected_page_efe9=projection.original_efe9_page;
    state.physical_page_base_f001=
        projection.original_f001_vga_linear_base;
    state.active_page_ef4d=projection.original_ef4d_page;
    if(services.selected_page_291b0)
        services.selected_page_291b0(page);
}
void signal_2f580(
    const CombatOriginalVgaPageStackServices& services,
    std::uint32_t diagnostic_string_offset) {
    if(!services.diagnostic_2f580)
        throw std::invalid_argument(
            "original 2F580 VGA page stack diagnostic unavailable");
    services.diagnostic_2f580(diagnostic_string_offset);
}
} // namespace

void CombatOriginalVgaPageStackModel::push_2e590(
    CombatOriginalVgaPageStackState& state,
    std::uint32_t requested_page,
    const CombatOriginalVgaPageStackServices& services) {
    // 2E590 tests the BYTE before any stores. There is no
    // automatic clamp at ten; if 2F580 returns, execution
    // resumes and F0D0 is read AGAIN at 2E5A9.
    if(state.depth_f0d0==0x0AU)
        signal_2f580(services,0x1358U);
    const auto depth=state.depth_f0d0;
    if(static_cast<std::size_t>(depth)>=
       state.saved_pages_2b644.size())
        throw std::invalid_argument(
            "uncaptured original DGROUP 2B644 page stack slot");

    // 2E5B1 loads EF4D (NOT EFE9), 2E5B7 stores it at
    // 2B644+unsigned F0D0. 2E5C2 then INC AL wraps at FFh,
    // 2E5C5 writes F0D0 and calls 291B0(page low byte).
    state.saved_pages_2b644[depth]=state.active_page_ef4d;
    state.depth_f0d0=static_cast<std::uint8_t>(
        static_cast<std::uint32_t>(depth)+1U);
    select_291b0(state,static_cast<std::uint8_t>(requested_page),
                 services);
}

void CombatOriginalVgaPageStackModel::pop_2e5d4(
    CombatOriginalVgaPageStackState& state,
    const CombatOriginalVgaPageStackServices& services) {
    // At empty stack 2E5DD calls 2F580(136Ch); if that
    // diagnostic returns, the original 8-bit decrement STILL
    // takes place and reads from index FFh.
    if(state.depth_f0d0==0U)
        signal_2f580(services,0x136CU);
    const auto next_depth=static_cast<std::uint8_t>(
        static_cast<std::uint32_t>(state.depth_f0d0)-1U);
    if(static_cast<std::size_t>(next_depth)>=
       state.saved_pages_2b644.size())
        throw std::invalid_argument(
            "uncaptured original DGROUP 2B644 page restore slot");

    state.depth_f0d0=next_depth;
    // 2E5F7 MOVZX loads the saved BYTE, then 2E5FF invokes
    // 291B0 to update EFE9, F001 and finally EF4D.
    select_291b0(state,state.saved_pages_2b644[next_depth],services);
}

} // namespace rloft
