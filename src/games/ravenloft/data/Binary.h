#pragma once
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>

namespace ravenloft::data {
std::uint16_t u16(std::span<const std::uint8_t> b, std::size_t o);
std::int16_t i16(std::span<const std::uint8_t> b, std::size_t o);
std::uint32_t u32(std::span<const std::uint8_t> b, std::size_t o);
std::int32_t i32(std::span<const std::uint8_t> b, std::size_t o);
std::string printablePrefix(std::span<const std::uint8_t> b);
void requireRange(std::span<const std::uint8_t> b, std::size_t o, std::size_t n, const char* what);
}
