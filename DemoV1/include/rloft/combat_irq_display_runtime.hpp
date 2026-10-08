#pragma once

#include <cstdint>
#include <functional>
#include <vector>

namespace rloft {

// Values are original CODE.2 mutable globals, not host-frame estimates.
struct CombatIrqDisplayRuntime final {
    std::uint8_t pending_cf59{};
    std::uint32_t deadline_cf54{0xFFFFFFFFU};
    std::uint32_t active_render_mode_c050{};
    std::uint32_t background_resource_2222c{};
    std::vector<std::uint8_t> queued_message_14580;
};

// Original rendering services are injected, never substituted with
// an assumed framebuffer rectangle or a fabricated host timer.
// Collaborators may change the observed globals during rendering.
struct CombatIrqDisplayServices final {
    std::function<void(std::uint8_t)> select_vga_page_291b0;
    std::function<void(std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t)>
        draw_background_2939c;
    std::function<void(std::uint32_t,std::uint32_t,
                       const std::vector<std::uint8_t>&,std::uint32_t)>
        draw_text_3060c;
    std::function<void(std::uint32_t,std::uint32_t,
                       std::uint32_t,std::uint32_t,std::uint32_t)>
        composite_1a8c8;
    std::function<std::uint32_t()> observed_irq_tick_30700;
};

// 291B0 -> 29FEC -> 2AB6C. F005 is the OBSERVED page-stride DWORD,
// not a hardcoded modern framebuffer/page constant. All arithmetic
// uses original 32-bit modulo behavior.
struct CombatVgaPageSelectionProjection final {
    std::uint8_t original_ef4d_page{};
    std::uint32_t original_efe9_page{};
    std::uint32_t original_f001_vga_linear_base{};
};

class CombatVgaPageSelectionModel final {
public:
    [[nodiscard]] static CombatVgaPageSelectionProjection inspect(
        std::uint8_t selected_page,
        std::uint32_t observed_f005_page_stride) noexcept;
};

// Unlike the earlier read-only event plan this layer runs the
// original collaborators in source order, using live mutable
// globals. It does not itself implement the four renderer services.
class CombatIrqDisplayRuntimeModel final {
public:
    static void update(
        CombatIrqDisplayRuntime& state,
        const CombatIrqDisplayServices& original_services);
};

} // namespace rloft
