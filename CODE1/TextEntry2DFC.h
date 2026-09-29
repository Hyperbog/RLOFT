#pragma once

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct TextEntryState2DFC {
    std::array<char, 16> name67F8{};
    std::uint8_t buffer7138 = 0;
};

struct TextEntryOps2DFC {
    std::function<std::uint32_t()> call7F50;
    std::function<void(std::int32_t, std::int32_t)> call7D68;
    std::function<void(std::uint8_t)> call6588;
    std::function<std::int32_t()> call7DE8;
    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call9EB0;
    std::function<void(std::uint8_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call550C;
    std::function<std::int32_t()> call831C;
    std::function<void(std::uint8_t)> call53E4;
    std::function<std::int32_t()> call82C0;
    std::function<std::int32_t(const char*, std::int32_t)> call7AB4;
    std::function<std::int32_t(std::int32_t, std::int32_t)> call7B50;
    std::function<void(std::int32_t, std::int32_t, std::int32_t,
                       const char*, std::int32_t)> call7D80;
};

class TextEntry2DFC {
public:
    // Direct reconstruction of CODE.1 0x2DFC..0x301F.
    static void run(std::int32_t x,
                    std::int32_t maxCharacters,
                    TextEntryState2DFC& state,
                    const TextEntryOps2DFC& ops);
};

} // namespace ravenloft::game
