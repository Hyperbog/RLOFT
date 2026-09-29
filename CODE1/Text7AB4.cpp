#include "Text7AB4.h"

namespace ravenloft::game {
namespace {

std::int32_t signedGlyph(
    char value) noexcept {
    return static_cast<std::int8_t>(
        static_cast<std::uint8_t>(
            value));
}

std::uint8_t glyphWidth(
    const FontView7AB4& font,
    std::int32_t glyph) {
    return font.widthAt(glyph);
}

void drawGlyph(
    std::int32_t glyph,
    const FontView7AB4& font,
    TextState7AB4& state,
    const TextOps7AB4& ops) {
    if (state.mode2908 != 0) {
        ops.call6824(
            glyph,
            state.coord8990,
            state.coord898C,
            font.glyphResource,
            state.mode2908);
    } else {
        ops.call67C8(
            glyph,
            state.coord8990,
            state.coord898C,
            font.glyphResource);
    }
}

} // namespace

std::int32_t Text7AB4::width(
    const char* text,
    std::int32_t font,
    TextState7AB4& state,
    const TextOps7AB4& ops) {
    state.font8988 =
        ops.call954CFont(font);

    std::int32_t total = 0;

    if (text != nullptr) {
        while (*text != '\0') {
            total +=
                glyphWidth(
                    state.font8988,
                    signedGlyph(*text));
            ++text;
        }
    }

    return total;
}

std::int32_t Text7AB4::widthN(
    const char* text,
    std::int32_t font,
    std::int32_t count,
    TextState7AB4& state,
    const TextOps7AB4& ops) {
    state.font8988 =
        ops.call954CFont(font);

    std::int32_t total = 0;

    if (text != nullptr) {
        while (*text != '\0' &&
               count != 0) {
            total +=
                glyphWidth(
                    state.font8988,
                    signedGlyph(*text));
            ++text;
            --count;
        }
    }

    return total;
}

std::int32_t Text7AB4::glyphWidth(
    std::int32_t glyph,
    std::int32_t font,
    TextState7AB4& state,
    const TextOps7AB4& ops) {
    state.font8988 =
        ops.call954CFont(font);

    return ravenloft::game::glyphWidth(
        state.font8988,
        static_cast<std::int8_t>(glyph));
}

std::int32_t Text7AB4::resourceWidth(
    std::int32_t font,
    TextState7AB4& state,
    const TextOps7AB4& ops) {
    state.font8988 =
        ops.call954CFont(font);

    return state.font8988.glyphResourceWidth;
}

void Text7AB4::drawChar(
    std::int32_t glyph,
    std::int32_t font,
    TextState7AB4& state,
    const TextOps7AB4& ops) {
    state.font8988 =
        ops.call954CFont(font);

    const std::int32_t index =
        static_cast<std::int8_t>(
            glyph);

    drawGlyph(
        index,
        state.font8988,
        state,
        ops);

    state.coord898C +=
        glyphWidth(
            state.font8988,
            index);
}

void Text7AB4::drawString(
    const char* text,
    std::int32_t font,
    TextState7AB4& state,
    const TextOps7AB4& ops) {
    state.font8988 =
        ops.call954CFont(font);

    if (text == nullptr) {
        return;
    }

    while (*text != '\0') {
        const std::int32_t index =
            signedGlyph(*text);

        drawGlyph(
            index,
            state.font8988,
            state,
            ops);

        state.coord898C +=
            glyphWidth(
                state.font8988,
                index);

        ++text;
    }
}

void Text7AB4::drawStringN(
    const char* text,
    std::int32_t font,
    std::int32_t count,
    TextState7AB4& state,
    const TextOps7AB4& ops) {
    state.font8988 =
        ops.call954CFont(font);

    if (text == nullptr) {
        return;
    }

    while (*text != '\0' &&
           count != 0) {
        const std::int32_t index =
            signedGlyph(*text);

        drawGlyph(
            index,
            state.font8988,
            state,
            ops);

        state.coord898C +=
            glyphWidth(
                state.font8988,
                index);

        ++text;
        --count;
    }
}

void Text7AB4::setPosition(
    std::int32_t first,
    std::int32_t second,
    TextState7AB4& state) noexcept {
    state.coord8990 = first;
    state.coord898C = second;
}

void Text7AB4::drawCentered(
    std::int32_t first,
    std::int32_t second,
    std::int32_t extent,
    const char* text,
    std::int32_t font,
    TextState7AB4& state,
    const TextOps7AB4& ops) {
    if (text == nullptr) {
        return;
    }

    state.font8988 =
        ops.call954CFont(font);

    state.coord8990 = first;

    const std::int32_t textWidth =
        width(
            text,
            font,
            state,
            ops);

    state.coord898C =
        second +
        ((extent - textWidth) >> 1);

    drawString(
        text,
        font,
        state,
        ops);
}

std::int32_t Text7AB4::coord8990(
    const TextState7AB4& state) noexcept {
    return state.coord8990;
}

std::int32_t Text7AB4::coord898C(
    const TextState7AB4& state) noexcept {
    return state.coord898C;
}

void Text7AB4::setMode(
    std::int32_t mode,
    TextState7AB4& state) noexcept {
    state.mode2908 = mode;
}

void Text7AB4::drawRightAligned(
    std::int32_t first,
    std::int32_t second,
    const char* text,
    std::int32_t font,
    TextState7AB4& state,
    const TextOps7AB4& ops) {
    if (text == nullptr) {
        return;
    }

    const std::int32_t textWidth =
        width(
            text,
            font,
            state,
            ops);

    setPosition(
        first,
        second - textWidth,
        state);

    drawString(
        text,
        font,
        state,
        ops);
}

void Text7AB4::drawAt(
    std::int32_t first,
    std::int32_t second,
    const char* text,
    std::int32_t font,
    TextState7AB4& state,
    const TextOps7AB4& ops) {
    setPosition(
        first,
        second,
        state);

    drawString(
        text,
        font,
        state,
        ops);
}

void Text7AB4::drawWithin(
    std::int32_t first,
    std::int32_t second,
    const char* text,
    std::int32_t font,
    std::int32_t remaining,
    TextState7AB4& state,
    const TextOps7AB4& ops) {
    setPosition(
        first,
        second,
        state);

    state.font8988 =
        ops.call954CFont(font);

    if (text == nullptr) {
        return;
    }

    const auto* cursor =
        reinterpret_cast<
            const std::uint8_t*>(
                text);

    for (;;) {
        // Unlike the preceding text routines, 0x7E6C zero-extends bytes.
        remaining -=
            glyphWidth(
                state.font8988,
                *cursor);

        if (*cursor == 0 ||
            remaining <= 0) {
            return;
        }

        drawGlyph(
            *cursor,
            state.font8988,
            state,
            ops);

        state.coord898C +=
            glyphWidth(
                state.font8988,
                *cursor);

        ++cursor;
    }
}

} // namespace ravenloft::game
