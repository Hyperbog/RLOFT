#pragma once

#include <array>
#include <cstdint>
#include <span>

namespace ravenloft::game {

struct Code1VgaBlitterDescriptorC7A0 {
    std::uint16_t field0;
    std::uint16_t field2;
    std::uint16_t field4;
    std::uint8_t field6;
    std::uint8_t field7;
    std::uint8_t transparent8;
};

// CODE.1 0xC7A0..0xC8C0 and 0xC8C4..0xC9D1 are the two five-argument
// planar-VGA blitter variants selected by CODE.1 0x9BE0..0x9C43.
// CODE.1 0xC9D4..0xCAC9 and 0xCACC..0xCBD0 are the two seven-argument
// scaled planar-VGA variants selected by CODE.1 0x9C44..0x9C97.
//
// These four routines intentionally patch immediate operands inside their own
// instruction streams (for example C843/C849/C858/C871/C897/C89B and the
// corresponding locations in the other variants). They are therefore kept as
// an explicitly classified self-modifying VGA block rather than translated
// into ordinary re-entrant C++ with different state semantics.
//
// 0xC79F is one byte of alignment before the real ENTER instruction at 0xC7A0.
// 0xC8C1..0xC8C3, 0xC9D2..0xC9D3, 0xCACA..0xCACB, and 0xCBD1..0xCBD3 are
// alignment/no-op bytes.

class Code1VgaBlittersC7A0 {
public:
    // Direct reconstruction of CODE.1 0xCBD4..0xCBEE.
    // Copies exactly 256 bytes (64 dwords) into the translation table at 0x2F88.
    static void setTranslationTable(
        std::span<const std::uint8_t, 0x100> source,
        std::array<std::uint8_t, 0x100>& table2F88) noexcept;

    // CODE.1 0xCBEF is a one-byte RET stub.
    static void noOpCBEF() noexcept;
};

} // namespace ravenloft::game
