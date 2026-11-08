#pragma once
// Borrowed host services for original CODE.2 guest callbacks.
// No Machine, heap, input queue, world, or saved-level owner is recreated.
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft::live_level_files { struct Overlay; }
namespace rloft::guest_callbacks {

struct GuestVector final {
    std::uint32_t offset{};
    std::uint16_t selector{};
};
using VectorTable=std::array<GuestVector,256u>;
void get_vector(const VectorTable& table,std::uint8_t vector,
                std::uint32_t* offset,std::uint16_t* selector) noexcept;
void set_vector(VectorTable& table,std::uint8_t vector,
                std::uint32_t offset,std::uint16_t selector) noexcept;

// Guest address validation never forms an out-of-range pointer.
[[nodiscard]] bool guest_range(const std::vector<std::uint8_t>& guest,
                               std::uint32_t offset,std::uint32_t count) noexcept;

enum class OverlayIoStatus:std::uint8_t {
    Ok, GuestOutOfBounds, InvalidHandle, ShortIo
};
struct OverlayIoResult {
    OverlayIoStatus status=OverlayIoStatus::Ok;
    std::uint32_t bytes=0u;
};
// Copy through the *same* mutable Overlay and *same* retained guest buffer.
// The host callback retains error-to-source-STOP mapping, disk fallback,
// session counters, and original call order.
[[nodiscard]] OverlayIoResult write_level_from_guest(
    live_level_files::Overlay& files,
    const std::vector<std::uint8_t>& guest,
    std::uint32_t src,std::uint32_t size,std::uint32_t handle) noexcept;
[[nodiscard]] OverlayIoResult read_level_into_guest(
    live_level_files::Overlay& files,
    std::vector<std::uint8_t>& guest,
    std::uint32_t dst,std::uint32_t size,std::uint32_t handle) noexcept;
} // namespace rloft::guest_callbacks
