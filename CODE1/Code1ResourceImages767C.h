#pragma once

#include "Code1BufferedIo72D0.h"
#include "Code1Graphics66C0.h"
#include "Code1Memory6F10.h"

#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct Code1ResourceDescriptor767C {
    std::int16_t frameCount = 0;
    std::int16_t width = 0;
    std::uint8_t heightUnits = 0;
    std::uint8_t marker = 0xFF;
    std::uint8_t encoding = 0;
};

struct Code1ResourceImageOps767C {
    Code1MemoryOps6F10 memory;
    Code1BufferedIoOps72D0 stream;
    Code1GraphicsOps66C0 graphics;

    std::function<std::int32_t(
        std::int32_t, std::int32_t, std::int32_t,
        std::int32_t, std::int32_t)> callA558;

    std::function<void(std::int32_t)> call9814;
    std::function<void()> call9AE8;
    std::function<void(std::uintptr_t)> callDC90;
};

class Code1ResourceImages767C {
public:
    // CODE.1 0x766A..0x766B is alignment.
    // CODE.1 0x766C..0x767B is the four-entry dispatch table used below.

    // CODE.1 0x767C..0x782E. 0x782F is alignment.
    static std::uintptr_t load(
        const Code1ResourceDescriptor767C& descriptor,
        std::int32_t handle,
        Code1BufferedIoState72D0& streamState,
        const Code1ResourceImageOps767C& ops);

    // CODE.1 0x7830..0x7842. 0x7843 is alignment.
    static void release(
        std::uintptr_t resource,
        const Code1ResourceImageOps767C& ops);

    // CODE.1 0x7844..0x7867.
    static std::int32_t allocationSize(
        std::uintptr_t resource) noexcept;

    // CODE.1 0x7868..0x7941. 0x7942..0x7943 is alignment.
    static std::uintptr_t capture(
        std::int32_t x,
        std::int32_t y,
        std::int32_t width,
        std::int32_t height,
        std::int32_t marker,
        Code1GraphicsState66C0& graphicsState,
        const Code1ResourceImageOps767C& ops);

    // CODE.1 0x7944..0x79C5.
    static std::uintptr_t fillCaptured(
        std::uintptr_t destination,
        std::int32_t word0Value,
        std::int32_t x,
        std::int32_t y,
        std::int32_t width,
        std::int32_t height,
        std::int32_t marker,
        Code1GraphicsState66C0& graphicsState,
        const Code1ResourceImageOps767C& ops);
};

} // namespace ravenloft::game
