#pragma once
// Host IRQ9 port and INT33 classification only. Original 308F8/30B40,
// F590 key translation and mouse state remain on the same CODE.2 guest.
#include <cstddef>
#include <cstdint>
namespace rloft::input {
class Irq9Ports final {
    std::uint8_t scan_=0, port61_=0;
    unsigned eoi_count_=0;
public:
    void begin_scan(std::uint8_t scan) noexcept;
    [[nodiscard]] bool read8(std::uint16_t port,
                             std::uint8_t& value) const noexcept;
    [[nodiscard]] bool write8(std::uint16_t port,
                              std::uint8_t value) noexcept;
    [[nodiscard]] unsigned eoi_count() const noexcept;
};
enum class MouseCommand : std::uint8_t {
    RangeX, RangeY, SetPosition, ReadPosition, Unsupported
};
[[nodiscard]] MouseCommand classify_20440_mouse(
    std::uint32_t ax, std::uint32_t bx,
    std::uint32_t cx, std::uint32_t dx) noexcept;
[[nodiscard]] bool is_host_set1_key(std::uint32_t key) noexcept;
[[nodiscard]] bool decode_host_set1_key(std::uint32_t key,
    std::size_t guest_size, std::uint8_t& scan) noexcept;
} // namespace rloft::input
