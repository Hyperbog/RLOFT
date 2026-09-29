#pragma once

#include "Code1DosServices6200.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct Code1BufferedIoState72D0 {
    std::int32_t handle2904 = 0;

    std::array<std::uint8_t, 0x400> buffer2470{};
    std::size_t bufferPos2870 = 0;
    std::int32_t bufferRemaining2874 = 0;

    std::uint8_t rleCount2878 = 0;
    std::size_t rlePos287C = 0;

    // Exact CODE.1 region DS:2880..2903.
    std::array<std::uint8_t, 0x84> rleBuffer2880{};

    const std::uint8_t* descriptor8994 = nullptr;
    std::uintptr_t outputArg899C = 0;
    std::int32_t outputCapacity89A4 = 0;
    std::int32_t outputIndex89B4 = 0;
    std::int32_t outputPos89B8 = 0;
    std::uint8_t* rawBuffer89C4 = nullptr;
    std::uint8_t* processedBuffer89C8 = nullptr;
    std::uint8_t outputDone89CE = 0;
    std::uint8_t outputMode89CF = 0;
};

struct Code1BufferedIoOps72D0 {
    Code1DosOps6200 dos;

    std::function<void(void*, const void*, std::size_t)> callD9EE;
    std::function<void()> call9AE8;

    std::function<void(std::int32_t, std::int32_t,
                       std::uint8_t*, std::int32_t,
                       std::uint8_t)> callA57C;

    std::function<void()> callDB80;
};

class Code1BufferedIo72D0 {
public:
    // CODE.1 0x72C9..0x72CF is zero/data padding.

    // CODE.1 0x72D0..0x72DE. 0x72DF is alignment.
    static void setHandle(std::int32_t handle,
                          Code1BufferedIoState72D0& state) noexcept;

    // CODE.1 0x72E0..0x7327.
    static void alignAndFill(Code1BufferedIoState72D0& state,
                             const Code1BufferedIoOps72D0& ops);

    // CODE.1 0x7328..0x735E. 0x735F is alignment.
    static void refill(Code1BufferedIoState72D0& state,
                       const Code1BufferedIoOps72D0& ops);

    // CODE.1 0x7360..0x738B.
    static std::uint8_t readByte(Code1BufferedIoState72D0& state,
                                 const Code1BufferedIoOps72D0& ops);

    // CODE.1 0x738C..0x7468. 0x7469..0x746B is alignment.
    static void emitByte(std::uint8_t value,
                         Code1BufferedIoState72D0& state,
                         const Code1BufferedIoOps72D0& ops);

    // CODE.1 0x746C..0x74CD. 0x74CE..0x74CF is alignment.
    static void beginOutput(std::uintptr_t arg899C,
                            const std::uint8_t* descriptor,
                            std::uint8_t* buffer,
                            std::int32_t capacity,
                            std::uint8_t mode,
                            Code1BufferedIoState72D0& state,
                            const Code1BufferedIoOps72D0& ops);

    // CODE.1 0x74D0..0x7569. 0x756A..0x756B is alignment.
    static void readBytes(std::uint8_t* destination,
                          std::int32_t count,
                          Code1BufferedIoState72D0& state,
                          const Code1BufferedIoOps72D0& ops);

    // CODE.1 0x756C..0x75EC. 0x75ED..0x75EF is alignment.
    static std::uint8_t readRleByte(Code1BufferedIoState72D0& state,
                                    const Code1BufferedIoOps72D0& ops);

    // CODE.1 0x75F0..0x762B.
    static void readRleBlock(std::uint8_t* destination,
                             std::int32_t width,
                             std::int32_t heightUnits,
                             Code1BufferedIoState72D0& state,
                             const Code1BufferedIoOps72D0& ops);

    // CODE.1 0x762C..0x7669.
    static void readRawBlock(std::uint8_t* destination,
                             std::int32_t width,
                             std::int32_t heightUnits,
                             Code1BufferedIoState72D0& state,
                             const Code1BufferedIoOps72D0& ops);
};

} // namespace ravenloft::game
