#pragma once

#include "UiSelection043C.h"
#include "UiTextComposite00EC.h"

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct CharacterVariantState2A64 {};

struct CharacterVariantOps2A64 {
    UiTextDrawOps00EC draw;

    // Raw object-1 data-segment access. 0x2A64 performs several deliberately
    // unchecked/indexed accesses, so the reconstruction must not replace them
    // with bounded C++ containers.
    std::function<std::uint8_t(std::uint32_t)> readByte;
    std::function<std::uint32_t(std::uint32_t)> readDword;
    std::function<void(std::uint32_t, std::uint8_t)> writeByte;
    std::function<void(std::uint32_t, std::uint32_t)> writeDword;

    std::function<void(std::uint8_t)> call6588;
    std::function<void(std::int32_t,
                       const std::array<std::int32_t, 9>&,
                       std::int32_t, std::int32_t, std::int32_t)> call1464;
    std::function<void(std::int32_t, std::int32_t)> call07E4;
    std::function<PointerSample043C()> pollPointer;
    std::function<std::int32_t(std::int32_t, std::int32_t)> call035C;
    std::function<void(std::uint32_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call55DC;
    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call15B0;

    // BYTE PTR [EBP + index - 0x25] when index is outside the six bytes
    // initialized at [EBP-0x24..EBP-0x1F]. This preserves the original
    // unchecked frame read without invoking host-language UB.
    std::function<std::uint8_t(std::int32_t)> readFrameByte;
};

class CharacterVariant2A64 {
public:
    // Fidelity reconstruction of CODE.1 object-1 0x2A64..0x2DF8.
    static void run(CharacterVariantState2A64& state,
                    const CharacterVariantOps2A64& ops);
};

} // namespace ravenloft::game
