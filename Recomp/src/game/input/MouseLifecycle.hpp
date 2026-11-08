#pragma once
// Borrowed CODE.2 1A064/33A7C INT33 function 0Ch callback ABI.
// Source guest Session owns the successful installation count.
#include <cstddef>
#include <cstdint>
namespace rloft::input::mouse_lifecycle {
[[nodiscard]] bool callback_frame_1a064(
    const std::uint8_t* guest, std::size_t size,
    std::uint32_t interrupt, std::uint32_t registers,
    std::uint32_t output, std::uint32_t segments) noexcept;
[[nodiscard]] bool may_install(bool source_mouse_started,
                               unsigned successful_installs) noexcept;
[[nodiscard]] bool accepted_reply(bool source_mouse_started,
    unsigned successful_installs, std::uint32_t return_eax) noexcept;
[[nodiscard]] bool result_pointer(std::uint32_t output,
                                  std::size_t guest_size) noexcept;
} // namespace rloft::input::mouse_lifecycle
