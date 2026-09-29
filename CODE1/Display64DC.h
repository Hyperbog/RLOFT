#pragma once

#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct DisplayState64DC {
    std::uint8_t byte23D4 = 0;
    std::int16_t word23D7 = 0;
    std::int16_t word23D9 = 0;
    std::int16_t word23DB = 0;
    std::int16_t word23DD = 0;
    std::int16_t word23DF = 0;
    std::uint8_t byte23E5 = 0;
    std::uint8_t byte23E6 = 0;
};

struct DisplayOps64DC {
    std::function<void()> call9DF4;
    std::function<std::uint8_t(std::uint8_t, std::uint8_t&)> call9CA0;
    std::function<void(std::int32_t)> callA474;

    std::function<void(std::uint8_t)> callA35C;
    std::function<void(std::uint8_t)> callA36C;

    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call6BC0;

    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t,
                       std::uint8_t)> callA05C;

    std::function<void(void*, std::int32_t, std::int32_t)> callA010;
    std::function<void(std::int32_t, std::int32_t)> callA2B0;
    std::function<void(std::int32_t)> callA21C;
};

class Display64DC {
public:
    // CODE.1 0x64DC..0x6587.
    static void configure(std::int32_t mode,
                          DisplayState64DC& state,
                          const DisplayOps64DC& ops);

    // CODE.1 0x6588..0x65A3.
    static void setActivePage(std::uint8_t page,
                              DisplayState64DC& state,
                              const DisplayOps64DC& ops);

    // CODE.1 0x65A4..0x65BF.
    static void setDrawPage(std::uint8_t page,
                            DisplayState64DC& state,
                            const DisplayOps64DC& ops);

    // CODE.1 0x65C0..0x65E7.
    static void verticalLine(std::int32_t x,
                             std::int32_t y,
                             std::int32_t length,
                             std::uint8_t color,
                             const DisplayOps64DC& ops);

    // CODE.1 0x65E8..0x6615.
    // CODE.1 0x6616..0x6617 is alignment.
    static void horizontalLine(std::int32_t x,
                               std::int32_t y,
                               std::int32_t length,
                               std::uint8_t color,
                               const DisplayOps64DC& ops);

    // CODE.1 0x6618..0x6631.
    static void palette(void* buffer,
                        const DisplayOps64DC& ops);

    // CODE.1 0x6634..0x6653.
    static void paletteRange(std::int32_t offset,
                             std::int32_t colorCount,
                             void* buffer,
                             const DisplayOps64DC& ops);

    // CODE.1 0x6654..0x6671.
    // CODE.1 0x6672..0x6673 is alignment.
    static void shiftA(std::int32_t value,
                       const DisplayState64DC& state,
                       const DisplayOps64DC& ops);

    // CODE.1 0x6674..0x6693.
    static void shiftB(std::int32_t value,
                       const DisplayState64DC& state,
                       const DisplayOps64DC& ops);

    // CODE.1 0x6694..0x66A6.
    // CODE.1 0x66A7 is alignment.
    static void scroll(std::int32_t value,
                       const DisplayOps64DC& ops);

    // CODE.1 0x66A8..0x66BC.
    // CODE.1 0x66BD is alignment.
    static void scrollNegative(std::int32_t value,
                               const DisplayOps64DC& ops);
};

} // namespace ravenloft::game
