#pragma once
#include <cstdint>
#include <functional>
#include <span>

namespace ravenloft::game {

struct TextDrawState1228 {
    // DS:0x5760 byte table. Each glyph entry is indexed by character and
    // byte +4 supplies the horizontal advance.
    std::span<const std::uint8_t> glyphTable5760;
};

struct TextDrawOps1228 {
    std::function<const std::uint8_t*(std::int32_t,std::int32_t)> call2F29C;
    std::function<void(std::int32_t,std::int32_t,std::int32_t,std::int32_t)> call296C0;
    std::function<std::int32_t(const std::uint8_t*,std::int32_t,std::int32_t)> call302CC;
    std::function<void(std::int32_t,std::int32_t,std::int32_t,std::int32_t)> call29804;
};

class TextDraw1228 {
public:
    // Direct reconstruction of CODE.2 0x1228..0x1398.
    static void run(const TextDrawState1228& state,
                    const TextDrawOps1228& ops,
                    std::int32_t arg0,
                    std::int32_t arg1);
};

} // namespace ravenloft::game
