#pragma once

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct CharacterChoiceState266C {
    std::uint8_t field680C = 0;
    std::array<std::uint8_t, 3> fields680D{{0xFFu, 0xFFu, 0xFFu}};

    std::array<std::int32_t, 9> previous8F4{};
    std::array<std::int32_t, 9> current8F8{};
    std::int32_t text940 = -1;
    std::int32_t text948 = -1;
    std::int32_t mode4D0 = 0;
    std::uint8_t buffer7138 = 0;
};

struct CharacterChoiceOps266C {
    std::function<void(std::int32_t, std::int32_t, std::int32_t)> call24D4;
    std::function<void(std::uint8_t)> call6588;
    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call86C4;
    std::function<void(std::int32_t)> call04B0;
    std::function<void(std::int32_t, std::int32_t)> call07E4;
    std::function<void(std::int32_t,
                       const std::array<std::int32_t, 9>&,
                       std::int32_t, std::int32_t, std::int32_t)> call1464;
    std::function<std::int32_t()> call53B4;
    std::function<std::int32_t()> call043C;
    std::function<std::int8_t(std::uint32_t)> readDsS8;

    // 0x28C2 CALL 0x15B0 is immediately followed by a conditional CALL 0x1F24.
    // CODE.1 does not establish EDX between those calls. Return the post-call
    // EDX snapshot so its low byte can be preserved as 0x1F24's inherited DL.
    std::function<std::uint32_t(std::int32_t, std::int32_t,
                                std::int32_t, std::int32_t)> call15B0;
};

class CharacterChoice266C {
public:
    // Direct reconstruction of CODE.1 0x266C..0x28E1.
    static void run(CharacterChoiceState266C& state,
                    const CharacterChoiceOps266C& ops);
};

} // namespace ravenloft::game
