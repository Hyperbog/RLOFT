#pragma once

#include "Code1InterruptUpdate50C0.h"

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct Code1VgaState6060 {
    std::array<std::uint8_t, 0x55> block2B28{};
    std::array<std::uint8_t, 0x55> saved136E{};

    std::uint8_t byte1368 = 0;
    std::uint8_t byte1369 = 0;
    std::uint8_t byte136A = 0;
    std::uint8_t byte136B = 0;
    std::uint8_t byte136C = 0;
    std::uint8_t byte136D = 0;

    std::uint16_t savedSS23C3 = 0;
    std::uint32_t savedESP23C5 = 0;
};

struct Code1VgaOps6060 {
    std::function<std::uint8_t(std::uint16_t)> in8;
    std::function<void(std::uint16_t, std::uint8_t)> out8;
    std::function<void(std::uint16_t, std::uint16_t)> out16;
};

class Code1VgaSave6060 {
public:
    // Direct reconstruction of CODE.1 0x6060..0x60B1.
    // CODE.1 0x60B2..0x60B3 is alignment.
    static void run(Code1VgaState6060& state,
                    const Code1VgaOps6060& ops);
};

class Code1VgaRestore60B4 {
public:
    // Direct reconstruction of CODE.1 0x60B4..0x6100.
    // CODE.1 0x6101..0x6103 is alignment.
    static void run(Code1VgaState6060& state,
                    const Code1VgaOps6060& ops);
};

struct Code1InterruptFrame6104 {
    std::int32_t eax = 0;
    std::int32_t ebx = 0;
    std::int32_t ecx = 0;
    std::int32_t edx = 0;
    std::uint16_t esiSegment = 0;
};

struct Code1InterruptOps6104 {
    Code1VgaOps6060 vga;
    Code1InterruptUpdateOps50C0 update;

    std::function<void()> callCEC0;
    std::function<std::uint16_t()> currentSS;
    std::function<std::uint32_t()> currentESP;

    std::function<const std::array<std::int16_t, 2>*(
        std::uint32_t)> linearPointer;
};

class Code1Interrupt6104 {
public:
    // Portable reconstruction of the observable CODE.1 0x6104..0x6183
    // interrupt handler body. CPU flag/segment save/restore and stack switching
    // remain platform-backend responsibilities.
    static void run(const Code1InterruptFrame6104& frame,
                    Code1VgaState6060& vgaState,
                    Code1InterruptUpdateState50C0& updateState,
                    const Code1InterruptOps6104& ops);
};

struct Code1GlobalState61C0 {
    std::uint32_t dword046C = 0;
};

class Code1GlobalAccessor61C0 {
public:
    // Direct reconstruction of CODE.1 0x61C0..0x61CD.
    static std::uint32_t get(
        const Code1GlobalState61C0& state) noexcept;
};

class Code1DosPrint61D0 {
public:
    // Direct reconstruction of CODE.1 0x61D0..0x61FC.
    // CODE.1 0x61FD..0x61FF is alignment.
    static void print(
        char* text,
        const std::function<void(const char*)>& dosAh09);
};

} // namespace ravenloft::game
