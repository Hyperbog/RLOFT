#include "UiTextComposite00EC.h"

#include "RuntimeString6EE9.h"

#include <cstddef>
#include <vector>

namespace ravenloft::game {
namespace {

std::vector<char> copyText(const char* text) {
    const auto length = RuntimeString6EE9::length(text);
    std::vector<char> buffer(length + 1u, '\0');
    RuntimeString6EE9::copy(buffer.data(), text);
    return buffer;
}

std::int32_t splitAtSpaces(char* text) noexcept {
    std::int32_t pieces = 0;
    char* cursor = text;
    for (;;) {
        while (*cursor != ' ' && *cursor != '\0') {
            ++cursor;
        }
        if (*cursor == ' ') {
            *cursor = '\0';
            ++cursor;
        }
        ++pieces;
        if (*cursor == '\0') {
            return pieces;
        }
    }
}

char* nextPiece(char* current) noexcept {
    while (*current != '\0') {
        ++current;
    }
    return current + 1;
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
        // 0x00FF..0x0116.
        ops.call6EB0(0, x, y, 0x1A, 7, surfaceOrResource);

        auto buffer = copyText(text);
        std::int32_t pieces = splitAtSpaces(buffer.data());

        // 0x0163..0x0180: signed division by two truncates toward zero.
        std::int32_t drawX = x + width / 2 - pieces * 5;
        char* piece = buffer.data();

        // 0x0183..0x01B2.
        while (pieces != 0) {
            ops.call7D80(drawX, y, textWidth, piece, 0);
            piece = nextPiece(piece);
            drawX += 10;
            --pieces;
        }
        return;
    }

    if (mode == 1) {
        // 0x01CD..0x0200.
        ops.call6C60(0, x, y, 0x1A, 7);
        ops.call7D80(x + 8, y, textWidth, text, 0);
        return;
    }

    if (mode == 2) {
        // 0x020B..0x0248.
        ops.call6C60(0, x, y, 0x1A, 7);
        ops.call7D80(x + width - 0x11, y, textWidth, text, 0);
        return;
    }

    if (mode == 3) {
        // 0x024F..0x0296.
        ops.call6C60(0, x, y, 0x17, 7);
        ops.call7D80(x + width / 2 - 5, y, textWidth, text, 0);
        return;
    }

    if (mode != 4 && mode != 5) {
        return;
    }

    // 0x02A7..0x02C0: mode 4 draws the frame; mode 5 does not.
    if (mode != 5) {
        ops.call6C60(0, x, y, 0x1A, 7);
    }

    auto buffer = copyText(text);
    splitAtSpaces(buffer.data());

    // 0x0309..0x0322.
    char* first = buffer.data();
    ops.call7D80(x + 8, y, textWidth, first, 0);

    // 0x0325..0x0348.
    char* second = nextPiece(first);
    ops.call7D80(x + width - 0x10, y, textWidth, second, 0);
}

} // namespace ravenloft::game
