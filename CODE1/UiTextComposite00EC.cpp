#include "UiTextComposite00EC.h"

#include "RuntimeString6EE9.h"

#include <cstdint>

namespace ravenloft::game {
namespace {

char* allocateCopy(const char* text, const UiTextDrawOps00EC& ops) {
    // 0x0119..0x013E / 0x02C3..0x02E7:
    // strlen(text), INC EAX, malloc(length+1), strcpy(destination,text).
    // The allocation is intentionally not represented by std::vector: the
    // original heap call and exact temporary-buffer lifetime are observable.
    std::uint32_t bytes =
        static_cast<std::uint32_t>(RuntimeString6EE9::length(text));
    bytes += 1u; // x86 INC: 32-bit wrap, CF preservation is irrelevant here.
    const std::uintptr_t address = ops.call71C8(bytes);
    auto* destination = reinterpret_cast<char*>(address);
    RuntimeString6EE9::copy(destination, text);
    return destination;
}

void splitSpacesForMode0(char* buffer, std::int32_t& pieces) noexcept {
    // 0x0141..0x0161.  EAX walks the temporary string and ESI counts pieces.
    // The initial jump to 0x015E matters: an empty string produces zero
    // pieces, not one.
    char* cursor = buffer;
    pieces = 0;
    while (*cursor != '\0') {
        while (*cursor != ' ' && *cursor != '\0') {
            ++cursor;
        }
        if (*cursor == ' ') {
            *cursor = '\0';
            ++cursor;
        }
        ++pieces;
    }
}

void splitSpacesForModes45(char* buffer) noexcept {
    // 0x02EA..0x0307.  This variant has no piece counter; it simply replaces
    // every encountered space with NUL while retaining the original cursor
    // progression (including consecutive spaces).
    char* cursor = buffer;
    while (*cursor != '\0') {
        while (*cursor != ' ' && *cursor != '\0') {
            ++cursor;
        }
        if (*cursor == ' ') {
            *cursor = '\0';
            ++cursor;
        }
    }
}

char* afterCurrentPiece(char* current) noexcept {
    // 0x01A3..0x01AB / 0x0325..0x032F.
    while (*current != '\0') {
        ++current;
    }
    return current + 1;
}

std::int32_t halfSignedTowardZero(std::int32_t value) noexcept {
    // Watcom emitted SAR-sign correction before SAR 1:
    //   edx=value; sar edx,31; sub eax,edx; sar eax,1
    // This is signed division by two with truncation toward zero.  Use 64-bit
    // arithmetic to avoid introducing C++ signed-overflow UB for INT_MIN.
    return static_cast<std::int32_t>(static_cast<std::int64_t>(value) / 2);
}

} // namespace

void UiTextComposite00EC::draw(
    std::int32_t x,
    std::int32_t y,
    std::int32_t width,
    std::int32_t textWidth,
    const char* text,
    std::int32_t mode,
    std::int32_t surfaceOrResource,
    const UiTextDrawOps00EC& ops) {
    if (mode == 0) {
        // 0x00F5..0x0116: six pushed arguments, in original cdecl order.
        ops.call6EB0(0, x, y, 0x1A, 7, surfaceOrResource);

        char* const buffer = allocateCopy(text, ops);
        const std::uintptr_t bufferAddress =
            reinterpret_cast<std::uintptr_t>(buffer);

        std::int32_t pieces = 0;
        splitSpacesForMode0(buffer, pieces);

        // 0x0163..0x0180. LEA computes pieces*5; all address arithmetic in
        // EDI/EAX is 32-bit in the original.  Unsigned intermediates preserve
        // those modulo-2^32 additions/subtractions without C++ signed UB.
        std::uint32_t drawXBits = static_cast<std::uint32_t>(x);
        drawXBits += static_cast<std::uint32_t>(halfSignedTowardZero(width));
        drawXBits -= static_cast<std::uint32_t>(pieces) * 5u;
        char* piece = buffer;

        // 0x0183 is TEST ESI,ESI.  With valid strlen-sized input pieces is
        // nonnegative; the exact condition is nevertheless != 0.
        while (pieces != 0) {
            ops.call7D80(static_cast<std::int32_t>(drawXBits),
                         y, textWidth, piece, 0);
            piece = afterCurrentPiece(piece);
            drawXBits += 10u;
            --pieces;
        }

        // 0x01B4..0x01BC.
        ops.call7240(bufferAddress);
        return;
    }

    if (mode == 1) {
        // 0x01C7..0x0200.
        ops.call6C60(0, x, y, 0x1A, 7);
        const std::uint32_t drawX = static_cast<std::uint32_t>(x) + 8u;
        ops.call7D80(static_cast<std::int32_t>(drawX), y,
                     textWidth, text, 0);
        return;
    }

    if (mode == 2) {
        // 0x0205..0x0248.
        ops.call6C60(0, x, y, 0x1A, 7);
        std::uint32_t drawX = static_cast<std::uint32_t>(x);
        drawX += static_cast<std::uint32_t>(width);
        drawX -= 0x11u;
        ops.call7D80(static_cast<std::int32_t>(drawX), y,
                     textWidth, text, 0);
        return;
    }

    if (mode == 3) {
        // 0x0249..0x0296.
        ops.call6C60(0, x, y, 0x17, 7);
        std::uint32_t drawX = static_cast<std::uint32_t>(x);
        drawX += static_cast<std::uint32_t>(halfSignedTowardZero(width));
        drawX -= 5u;
        ops.call7D80(static_cast<std::int32_t>(drawX), y,
                     textWidth, text, 0);
        return;
    }

    // 0x0297..0x02A1: modes other than 4 and 5 fall through to epilogue.
    if (mode != 4 && mode != 5) {
        return;
    }

    // 0x02A7..0x02C0: mode 4 draws the frame; mode 5 suppresses it.
    if (mode != 5) {
        ops.call6C60(0, x, y, 0x1A, 7);
    }

    char* const buffer = allocateCopy(text, ops);
    const std::uintptr_t bufferAddress =
        reinterpret_cast<std::uintptr_t>(buffer);
    splitSpacesForModes45(buffer);

    // 0x0309..0x0322: first NUL-delimited piece at x+8.
    std::uint32_t firstX = static_cast<std::uint32_t>(x) + 8u;
    ops.call7D80(static_cast<std::int32_t>(firstX), y,
                 textWidth, buffer, 0);

    // 0x0325..0x0348: second piece begins one byte after the first NUL.
    // If no separator exists this is literally one-past the allocated string;
    // do not sanitize that original behavior in the faithful reconstruction.
    char* const second = afterCurrentPiece(buffer);
    std::uint32_t secondX = static_cast<std::uint32_t>(x);
    secondX += static_cast<std::uint32_t>(width);
    secondX -= 0x10u;
    ops.call7D80(static_cast<std::int32_t>(secondX), y,
                 textWidth, second, 0);

    // 0x034B..0x0351.
    ops.call7240(bufferAddress);
}

} // namespace ravenloft::game
