#pragma once

#include "Code1TextEntry2DFC.h"
#include "Code1UiTextComposite00EC.h"

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct Code1NameScreenState3020 {
    std::array<char,16> name67F8{};
    std::uintptr_t handle958 = 0;
    std::int32_t value92C = 0;
    std::int32_t value930 = 0;
    std::uint8_t buffer7138 = 0;
};

struct Code1NameScreenOps3020 {
    Code1UiTextDrawOps00EC draw;
    Code1TextEntryOps2DFC textEntry;
    std::function<void(std::int32_t,
                       const std::array<std::int32_t,9>&,
                       std::int32_t,std::int32_t,std::int32_t)> call1464;
    std::function<void(std::uint8_t)> call6588;
    std::function<void(std::uint8_t)> call53E4;
    std::function<void(std::uintptr_t)> call7830;
    std::function<std::uintptr_t(std::int32_t,std::int32_t,
                                 std::int32_t,std::int32_t,std::int32_t)> call7868;
    std::function<void(std::int32_t,std::int32_t,
                       std::int32_t,std::int32_t)> call15B0;
};

class Code1NameScreen3020 {
public:
    // Direct reconstruction of CODE.1 0x3020..0x3134.
    static void run(Code1NameScreenState3020& state,
                    const Code1NameScreenOps3020& ops);
};

} // namespace ravenloft::game
