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
[[nodiscard]] bool source_title_presented(
    unsigned original_page_presentations) noexcept;
} // namespace rloft::ui::title_interaction
