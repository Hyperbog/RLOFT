#include "VgaBlittersC7A0.h"

#include <cstring>

namespace ravenloft::game {

void VgaBlittersC7A0::setTranslationTable(
    std::span<const std::uint8_t, 0x100> source,
    std::array<std::uint8_t, 0x100>& table2F88) noexcept {
    std::memcpy(
        table2F88.data(),
        source.data(),
        table2F88.size());
}

void VgaBlittersC7A0::noOpCBEF() noexcept {
}

} // namespace ravenloft::game
