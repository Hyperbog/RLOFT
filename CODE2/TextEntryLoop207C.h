#pragma once
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct TextEntryLoop207CState {
    std::uint8_t valueC050 = 0;
    std::uint8_t valueEF4E = 0;
    std::int16_t mouseX9E31 = 0;
    std::int16_t mouseY9E33 = 0;
    std::int16_t boxLeft5F82 = 0, boxTop5F84 = 0;
    std::int16_t boxRight5F86 = 0, boxBottom5F88 = 0;
};

struct TextEntryLoop207COps {
    std::function<std::int32_t()> call1A1F4;
    std::function<void(std::int32_t)> call1F6C;
    std::function<void(std::uint8_t)> call291B0;
    std::function<void()> call1AACC;
    std::function<void(std::uint8_t)> call2E590;
    std::function<void(std::uint8_t)> call2A020;
    std::function<void()> call2E5D4;
    std::function<void()> call1A288;
    std::function<std::int32_t(std::int32_t,std::int32_t,std::int32_t,std::int32_t,std::int32_t)> call30070;
    std::function<void(std::int32_t,std::int32_t,std::int32_t,std::int32_t)> call1CE4;
    std::function<void(std::int32_t)> call30040;
    std::function<void()> call22D70;
};

class TextEntryLoop207C {
public:
    // Direct reconstruction of CODE.2 0x207C..0x21D7.
    static void run(TextEntryLoop207CState& state,
                    const TextEntryLoop207COps& ops);
};

} // namespace ravenloft::game
