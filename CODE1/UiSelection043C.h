#pragma once

#include "UiHitTest035C.h"

#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct PointerSample043C {
    std::int32_t buttons = 0; // DS:73EA after CODE.1 0x53B4
    std::int32_t x = 0;       // DS:73E6
    std::int32_t y = 0;       // DS:73E8
};

class UiSelection043C {
public:
    using PointerPoll = std::function<PointerSample043C()>;
    using KeyPoll = std::function<std::int32_t()>;

    // Direct reconstruction of CODE.1 0x043C..0x04AF.
    static std::int32_t waitForSelection(
        const UiHitTestState035C& hitState,
        std::int32_t& lastButtons4D4,
        const PointerPoll& pollPointer,
        const KeyPoll& pollKey);
};

} // namespace ravenloft::game
