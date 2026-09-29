#include "Code1Binary.h"

#include <stdexcept>
#include <string>

namespace ravenloft::game {

void Code1Binary::requireRange(std::span<const std::uint8_t> bytes,
                               std::size_t offset,
                               std::size_t count,
                               const char* what) {
    if (offset > bytes.size() || count > bytes.size() - offset) {
        throw std::runtime_error(std::string("Out of range: ") + what);
    }
}

std::uint16_t Code1Binary::u16(std::span<const std::uint8_t> bytes,
                               std::size_t offset) {
    requireRange(bytes, offset, 2, "u16");
    return static_cast<std::uint16_t>(
        bytes[offset] |
        (static_cast<std::uint16_t>(bytes[offset + 1]) << 8));
}

std::uint32_t Code1Binary::u32(std::span<const std::uint8_t> bytes,
                               std::size_t offset) {
    requireRange(bytes, offset, 4, "u32");
    return static_cast<std::uint32_t>(bytes[offset]) |
           (static_cast<std::uint32_t>(bytes[offset + 1]) << 8) |
           (static_cast<std::uint32_t>(bytes[offset + 2]) << 16) |
           (static_cast<std::uint32_t>(bytes[offset + 3]) << 24);
}

} // namespace ravenloft::game
