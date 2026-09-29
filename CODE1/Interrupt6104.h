#pragma once

#include "InterruptUpdate50C0.h"
#include "VgaState6060.h"

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct InterruptFrame6104 {
    std::uint32_t eax = 0;
    std::uint32_t ebx = 0;
    std::uint32_t ecx = 0;
    std::uint32_t edx = 0;
    std::uint32_t esi = 0;
    std::uint32_t esp = 0;
    std::uint16_t ss = 0;
};

struct InterruptState6104 {
    std::uint8_t active1368 = 0;
    std::uint16_t savedSs23C3 = 0;
    std::uint32_t savedEsp23C5 = 0;

    VgaState6060 vga{};
    InterruptUpdateState50C0 update{};
};

struct InterruptOps6104 {
    std::function<void()> callCEC0;

    // The original converts the low 16 bits of ESI from a real-mode segment
    // into a linear address by shifting left four. The platform layer resolves
    // that address to the two-WORD sample consumed by 0x50C0.
    std::function<const std::array<std::int16_t, 2>*(
        std::uint32_t)> resolveLinearPointer;

    VgaOps6060 vga;
    InterruptUpdateOps50C0 update;
};

class Interrupt6104 {
public:
    // Direct reconstruction of CODE.1 0x6104..0x6183.
    //
    // PUSHFW/PUSHA/segment saves, private SS:ESP switching, and RETF are ABI
    // mechanics of the DOS interrupt wrapper. InterruptFrame6104 makes
    // those preserved inputs explicit while the observable CODE.1 call/state
    // sequence remains unchanged.
    static void run(const InterruptFrame6104& frame,
                    InterruptState6104& state,
                    const InterruptOps6104& ops);
};

} // namespace ravenloft::game
