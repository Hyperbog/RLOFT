#include "rloft/combat_irq_display_runtime.hpp"

#include <cstdint>
#include <stdexcept>

namespace rloft {

CombatVgaPageSelectionProjection CombatVgaPageSelectionModel::inspect(
    std::uint8_t selected_page,
    std::uint32_t observed_f005_page_stride) noexcept {
    CombatVgaPageSelectionProjection out{};
    // 291BE..291C2 stores the page byte to EF4D after the
    // 2AB6C address-calculation service has returned.
    out.original_ef4d_page=selected_page;
    // 2AB6E..2AB73 masks EAX to 0xFF before writing EFE9.
    out.original_efe9_page=selected_page;
    // 2AB7D..2AB8D unsigned MUL/add/SHL4 writes F001.
    const auto paragraph=static_cast<std::uint32_t>(
        static_cast<std::uint32_t>(selected_page)*
        observed_f005_page_stride+0xA000U);
    out.original_f001_vga_linear_base=paragraph<<4U;
    return out;
}

void CombatIrqDisplayRuntimeModel::update(
    CombatIrqDisplayRuntime& state,
    const CombatIrqDisplayServices& service) {
    // 1C2C4 ALWAYS performs one 30700 read, but only invokes
    // drawing collaborators on branches that actually require them.
    if (!service.observed_irq_tick_30700)
        throw std::invalid_argument("missing original IRQ 30700 service");

    if (state.pending_cf59!=0U) {
        if (!service.select_vga_page_291b0 ||
            !service.draw_background_2939c || !service.draw_text_3060c ||
            !service.composite_1a8c8)
            throw std::invalid_argument(
                "missing original 1C258 drawing service");
        // Original 3060C consumes a NUL-terminated buffer at 14580.
        // A missing NUL is an incomplete host snapshot, not a valid
        // emulation of the original unbounded source memory reads.
        bool terminated=false;
        for (const auto byte:state.queued_message_14580) {
            if (byte==0U) {terminated=true;break;}
        }
        if (!terminated) throw std::invalid_argument(
            "missing terminator in captured original 14580 buffer");

        // 1C258 reads C050's low byte for 291B0; 2939C reads
        // the live 2222C DWORD at the moment of its own call.
        service.select_vga_page_291b0(
            static_cast<std::uint8_t>(state.active_render_mode_c050));
        service.draw_background_2939c(
            0U,0x106U,0x20U,state.background_resource_2222c);
        service.draw_text_3060c(
            0x107U,0x22U,state.queued_message_14580,1U);
        service.composite_1a8c8(
            state.active_render_mode_c050,0x106U,0x20U,0x0CU,0x100U);
        // CF59 clears only AFTER the compositor has returned.
        state.pending_cf59=0U;
    }

    // 1C2C4's IRQ observation occurs after 1C258 returns; CF54
    // is NOT a deadline cached before the callbacks.
    const auto now=service.observed_irq_tick_30700();
    if (now>state.deadline_cf54) {
        if (!service.select_vga_page_291b0 ||
            !service.draw_background_2939c || !service.composite_1a8c8)
            throw std::invalid_argument(
                "missing original 1C2C4 erase service");
        service.select_vga_page_291b0(
            static_cast<std::uint8_t>(state.active_render_mode_c050));
        service.draw_background_2939c(
            0U,0x106U,0x20U,state.background_resource_2222c);
        // 1C314 stores the sentinel BEFORE the second compositor.
        state.deadline_cf54=0xFFFFFFFFU;
        service.composite_1a8c8(
            state.active_render_mode_c050,0x106U,0x20U,0x0CU,0x100U);
    }
}

} // namespace rloft
