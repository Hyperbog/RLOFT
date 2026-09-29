#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

namespace ravenloft::game {

class Code1Binary {
public:
    static std::uint16_t u16(std::span<const std::uint8_t> bytes,
                             std::size_t offset);
    static std::uint32_t u32(std::span<const std::uint8_t> bytes,
                             std::size_t offset);
    static void requireRange(std::span<const std::uint8_t> bytes,
                             std::size_t offset,
                             std::size_t count,
                             const char* what);
};

} // namespace ravenloft::game
