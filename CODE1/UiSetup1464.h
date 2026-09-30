#pragma once

#include "UiEntry10A4.h"
#include "UiTransition07E4.h"

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct UiSetupState1464 {
    std::int32_t mode4D0 = 0;
    std::array<std::int32_t, 9> x87C{};
    std::array<std::int32_t, 9> y880{};

    // CODE.1 uses eleven DWORD slots at DS:08F4 + index*8 and DS:08F8 +
    // index*8.  Slots 0..8 are UI entries; current slots 9/10 are the two
    // text-record indices consumed by 0x07E4.
    std::array<std::int32_t, 11> previous8F4Stride8{};
    std::array<std::int32_t, 11> current8F8Stride8{};

    // DS:0954 is a contiguous DWORD resource table.  DS:0958 is therefore
    // resource954[1], not independent state.
    std::array<std::int32_t, 9> resource954{};
    std::array<std::uint8_t, 12> animation964{};
    std::array<std::uint8_t, 12> flag970{};

    // DS:7138 is DWORD storage even though some callees deliberately read or
    // modify only its low byte.
    std::uint32_t buffer7138 = 0;
    std::uint8_t byte63F8 = 0;
    std::int32_t global713C = 0;

    // DS:04B8 is a pointer used as strcpy's source at 0x14E8. DS:7140 is the
    // destination character buffer used by strcpy/0x004C and then 0x10A4.
    const char* string4B8 = nullptr;
    char* text7140 = nullptr;
};

struct UiSetupOps1464 {
    UiEntryOps10A4 entry;
    UiTransitionOps07E4 transition;

    // 0x72A0 behaves as the original copy routine called with
    // (destination=DS:7140, source=[DS:04B8]).
    std::function<void(char*, const char*)> call72A0;
    // CODE.1 0x004C: destination buffer first, signed record index second.
    std::function<void(char*, std::int32_t)> call004C;
};

class UiSetup1464 {
public:
    // Direct-binary verified reconstruction of CODE.1 0x1464..0x15AD.
    static void run(std::int32_t characterPanelStage,
                    const std::array<std::int32_t, 9>& slotValues,
                    std::int32_t textIndex940,
                    std::int32_t textIndex948,
                    std::int32_t titleSelector,
                    UiSetupState1464& state,
                    const UiSetupOps1464& ops);
};

} // namespace ravenloft::game
