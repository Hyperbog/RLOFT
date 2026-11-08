#pragma once
// Original 19464 mouse-event ABI; source 19D18/19464 own menu choices.
#include <cstddef>
#include <cstdint>
namespace rloft::ui::title_interaction {
struct MouseEvent {
    std::uint32_t event=1u;
    std::uint32_t buttons=0u;
    std::uint32_t dos_x_doubled=0u;
    std::uint32_t dos_y=0u;
    std::uint32_t reserved=0u;
};
[[nodiscard]] bool read_source_mouse_event(
    const std::uint8_t* guest, std::size_t size,
    MouseEvent& event) noexcept;
// One callback reason per observed physical mouse state transition.
// Uses DOS INT33 fn0C bits: motion=1, L down/up=2/4, R down/up=8/16.
class CallbackDispatch final {
    std::uint32_t previous_buttons_=0u;
    std::uint32_t previous_dos_x_=0x140u, previous_dos_y_=0xC8u;
public:
    [[nodiscard]] std::uint32_t event_mask(const MouseEvent& state) noexcept;
};
[[nodiscard]] bool source_title_presented(
    unsigned original_page_presentations) noexcept;
} // namespace rloft::ui::title_interaction
