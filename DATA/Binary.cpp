#include "games/ravenloft/data/Binary.h"
#include <stdexcept>

namespace ravenloft::data {
void requireRange(std::span<const std::uint8_t> b, std::size_t o, std::size_t n, const char* what) {
    if (o > b.size() || n > b.size() - o) throw std::runtime_error(std::string("Out of range: ") + what);
}
std::uint16_t u16(std::span<const std::uint8_t> b, std::size_t o) {
    requireRange(b,o,2,"u16"); return static_cast<std::uint16_t>(b[o] | (std::uint16_t(b[o+1])<<8));
}
std::int16_t i16(std::span<const std::uint8_t> b, std::size_t o) { return static_cast<std::int16_t>(u16(b,o)); }
std::uint32_t u32(std::span<const std::uint8_t> b, std::size_t o) {
    requireRange(b,o,4,"u32"); return std::uint32_t(b[o]) | (std::uint32_t(b[o+1])<<8) | (std::uint32_t(b[o+2])<<16) | (std::uint32_t(b[o+3])<<24);
}
std::int32_t i32(std::span<const std::uint8_t> b, std::size_t o) { return static_cast<std::int32_t>(u32(b,o)); }
std::string printablePrefix(std::span<const std::uint8_t> b) {
    std::string s;
    for (auto c: b) { if (c < 0x20 || c > 0x7e) break; s.push_back(static_cast<char>(c)); }
    return s;
}
}
