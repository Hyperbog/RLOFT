#pragma once

#include "TextEntry2DFC.h"
#include "UiTextComposite00EC.h"

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct NameScreenState3020 {};

struct NameScreenOps3020 {
    UiTextDrawOps00EC draw;
    TextEntryOps2DFC textEntry;
    std::function<std::uint8_t(std::uint32_t)> readByte;
    std::function<std::uint32_t(std::uint32_t)> readDword;
    std::function<void(std::uint32_t, std::uint8_t)> writeByte;
    std::function<void(std::uint32_t, std::uint32_t)> writeDword;
    std::function<void(std::int32_t,const std::array<std::int32_t,9>&,
                       std::int32_t,std::int32_t,std::int32_t)> call1464;
    std::function<void(std::uint32_t)> call6588;
    std::function<void(std::uint32_t)> call53E4;
    std::function<void(std::uint32_t)> call7830;
    std::function<std::uint32_t(std::int32_t,std::int32_t,
                                std::int32_t,std::int32_t,std::int32_t)> call7868;
    std::function<void(std::int32_t,std::int32_t,
                       std::int32_t,std::int32_t)> call15B0;
};

class NameScreen3020 {
public:
    // Fidelity reconstruction of CODE.1 0x3020..0x3134.
    static void run(NameScreenState3020& state,
                    const NameScreenOps3020& ops);
};

} // namespace ravenloft::game
