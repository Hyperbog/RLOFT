#pragma once

#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct Code1FontView7AB4 {
    std::uintptr_t glyphResource = 0;
    std::uint16_t glyphResourceWidth = 0;

    // Preserves the original indexing distinction:
    // most routines use a sign-extended byte, while 0x7E6C uses zero extension.
    std::function<std::uint8_t(std::int32_t)> widthAt;
};

struct Code1TextState7AB4 {
    Code1FontView7AB4 font8988{};
    std::int32_t coord898C = 0;
    std::int32_t coord8990 = 0;
    std::int32_t mode2908 = 0;
};

struct Code1TextOps7AB4 {
    std::function<Code1FontView7AB4(
        std::int32_t)> call954CFont;

    std::function<void(
        std::int32_t, std::int32_t,
        std::int32_t, std::uintptr_t)> call67C8;

    std::function<void(
        std::int32_t, std::int32_t,
        std::int32_t, std::uintptr_t,
        std::int32_t)> call6824;
};

class Code1Text7AB4 {
public:
    // CODE.1 0x7AB4..0x7AFB.
    static std::int32_t width(
        const char* text,
        std::int32_t font,
        Code1TextState7AB4& state,
        const Code1TextOps7AB4& ops);

    // CODE.1 0x7AFC..0x7B4F.
    static std::int32_t widthN(
        const char* text,
        std::int32_t font,
        std::int32_t count,
        Code1TextState7AB4& state,
        const Code1TextOps7AB4& ops);

    // CODE.1 0x7B50..0x7B72. 0x7B73 is alignment.
    static std::int32_t glyphWidth(
        std::int32_t glyph,
        std::int32_t font,
        Code1TextState7AB4& state,
        const Code1TextOps7AB4& ops);

    // CODE.1 0x7B74..0x7B93.
    static std::int32_t resourceWidth(
        std::int32_t font,
        Code1TextState7AB4& state,
        const Code1TextOps7AB4& ops);

    // CODE.1 0x7B94..0x7C08.
    // CODE.1 0x7C09..0x7C0B is alignment.
    static void drawChar(
        std::int32_t glyph,
        std::int32_t font,
        Code1TextState7AB4& state,
        const Code1TextOps7AB4& ops);

    // CODE.1 0x7C0C..0x7CAF.
    static void drawString(
        const char* text,
        std::int32_t font,
        Code1TextState7AB4& state,
        const Code1TextOps7AB4& ops);

    // CODE.1 0x7CB0..0x7D66. 0x7D67 is alignment.
    static void drawStringN(
        const char* text,
        std::int32_t font,
        std::int32_t count,
        Code1TextState7AB4& state,
        const Code1TextOps7AB4& ops);

    // CODE.1 0x7D68..0x7D7E. 0x7D7F is alignment.
    static void setPosition(
        std::int32_t first,
        std::int32_t second,
        Code1TextState7AB4& state) noexcept;

    // CODE.1 0x7D80..0x7DDB.
    static void drawCentered(
        std::int32_t first,
        std::int32_t second,
        std::int32_t extent,
        const char* text,
        std::int32_t font,
        Code1TextState7AB4& state,
        const Code1TextOps7AB4& ops);

    // CODE.1 0x7DDC..0x7DE7.
    static std::int32_t coord8990(
        const Code1TextState7AB4& state) noexcept;

    // CODE.1 0x7DE8..0x7DF3.
    static std::int32_t coord898C(
        const Code1TextState7AB4& state) noexcept;

    // CODE.1 0x7DF4..0x7E02. 0x7E03 is alignment.
    static void setMode(
        std::int32_t mode,
        Code1TextState7AB4& state) noexcept;

    // CODE.1 0x7E04..0x7E42. 0x7E43 is alignment.
    static void drawRightAligned(
        std::int32_t first,
        std::int32_t second,
        const char* text,
        std::int32_t font,
        Code1TextState7AB4& state,
        const Code1TextOps7AB4& ops);

    // CODE.1 0x7E44..0x7E6A. 0x7E6B is alignment.
    static void drawAt(
        std::int32_t first,
        std::int32_t second,
        const char* text,
        std::int32_t font,
        Code1TextState7AB4& state,
        const Code1TextOps7AB4& ops);

    // CODE.1 0x7E6C..0x7F40.
    // CODE.1 0x7F41..0x7F4F is fifteen zero bytes.
    static void drawWithin(
        std::int32_t first,
        std::int32_t second,
        const char* text,
        std::int32_t font,
        std::int32_t remaining,
        Code1TextState7AB4& state,
        const Code1TextOps7AB4& ops);
};

} // namespace ravenloft::game
