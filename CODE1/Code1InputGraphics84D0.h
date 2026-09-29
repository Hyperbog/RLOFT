#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <string>

namespace ravenloft::game {

struct Code1InputDeviceRecord84D0 {
    std::int16_t word0 = 0;
    std::int16_t word2 = 0;
    std::int16_t word4 = 0;
};

struct Code1StripDescriptor85A8 {
    std::uintptr_t handle = 0;
    std::uint16_t count = 0;
    std::uint16_t stride = 0;
};

struct Code1InputGraphicsState84D0 {
    Code1InputDeviceRecord84D0* input2A9C = nullptr;
    std::uint8_t activePage23E5 = 0;
    std::uint8_t pageDepth2B20 = 0;
    std::array<std::uint8_t, 10> pageStack89EC{};
    std::uint32_t waitAnchor89E8 = 0;
    std::function<void()> callback2B1C;
    std::function<void()> callback2B18;
};

struct Code1InputGraphicsOps84D0 {
    std::function<void(std::int32_t, std::int32_t, std::int32_t, std::uint8_t)> verticalLine65C0;
    std::function<void(std::int32_t, std::int32_t, std::int32_t, std::uint8_t)> horizontalLine65E8;
    std::function<void(std::int32_t, std::int32_t, std::uintptr_t)> callAE0C;
    std::function<void(std::int32_t, std::int32_t, std::int32_t, std::uintptr_t)> call6D54;
    std::function<void(std::int32_t, std::int32_t, std::int32_t, std::uintptr_t)> call67C8;
    std::function<Code1StripDescriptor85A8(std::int32_t, std::int32_t)> call954C;
    std::function<void(std::int32_t, std::uintptr_t)> callA5C8;
    std::function<void(std::int32_t, std::uintptr_t)> callA678;
    std::function<void(std::int32_t)> fatal9814;
    std::function<void(std::uint8_t)> setActivePage6588;
    std::function<std::uint32_t()> ticks7F50;
    std::function<void()> service9AE8;
    std::function<std::int32_t()> keyPending82A4;
    std::function<std::uint32_t()> readKey82C0;
    std::function<const Code1InputDeviceRecord84D0*()> callD0B4;
    std::function<void(std::int32_t)> callDF13;
    std::function<void()> callDF73;
};

class Code1InputGraphics84D0 {
public:
    static std::int32_t inputWord0(const Code1InputGraphicsState84D0& state) noexcept;
    static std::int32_t inputWord4IsOne(const Code1InputGraphicsState84D0& state) noexcept;
    static std::int32_t inputWord2WhenActive(const Code1InputGraphicsState84D0& state) noexcept;

    static void outlineRect(std::int32_t x, std::int32_t y,
                            std::int32_t width, std::int32_t height,
                            std::uint8_t color,
                            const Code1InputGraphicsOps84D0& ops);

    static void forEachStrip(std::int32_t value,
                             const Code1StripDescriptor85A8& descriptor,
                             const Code1InputGraphicsOps84D0& ops);

    static void drawStripClipped(std::int32_t x, std::int32_t y,
                                 std::int32_t start, std::int32_t end,
                                 const Code1StripDescriptor85A8& descriptor,
                                 const Code1InputGraphicsOps84D0& ops);
    static void drawWholeStripClipped(std::int32_t x, std::int32_t y,
                                      const Code1StripDescriptor85A8& descriptor,
                                      const Code1InputGraphicsOps84D0& ops);
    static void drawStripDirect(std::int32_t x, std::int32_t y,
                                std::int32_t start, std::int32_t end,
                                const Code1StripDescriptor85A8& descriptor,
                                const Code1InputGraphicsOps84D0& ops);
    static void drawWholeStripDirect(std::int32_t x, std::int32_t y,
                                     const Code1StripDescriptor85A8& descriptor,
                                     const Code1InputGraphicsOps84D0& ops);
    static void drawResourceStrip(std::int32_t x, std::int32_t y,
                                  std::int32_t resourceId, std::int32_t bank,
                                  const Code1InputGraphicsOps84D0& ops);

    static void forEachA5C8(const Code1StripDescriptor85A8& descriptor,
                            const Code1InputGraphicsOps84D0& ops);
    static void forEachA678(const Code1StripDescriptor85A8& descriptor,
                            const Code1InputGraphicsOps84D0& ops);

    static void pushActivePage(std::uint8_t page,
                               Code1InputGraphicsState84D0& state,
                               const Code1InputGraphicsOps84D0& ops);
    static void popActivePage(Code1InputGraphicsState84D0& state,
                              const Code1InputGraphicsOps84D0& ops);

    static void startWait(Code1InputGraphicsState84D0& state,
                          const Code1InputGraphicsOps84D0& ops);
    static std::uint32_t wait(std::uint32_t delay, std::uint32_t flags,
                              Code1InputGraphicsState84D0& state,
                              const Code1InputGraphicsOps84D0& ops);
    static void delay6(const Code1InputGraphicsOps84D0& ops,
                       Code1InputGraphicsState84D0& state);

    // 0x88D8 signed conversion was already reconstructed earlier.
    static std::string unsignedDecimal(std::uint32_t value);
    static std::int32_t signum(std::int32_t value) noexcept;

    static void setCallback2B1C(std::function<void()> callback,
                                Code1InputGraphicsState84D0& state);
    static void setCallback2B18(std::function<void()> callback,
                                Code1InputGraphicsState84D0& state);
    static void invokeCallback2B18(Code1InputGraphicsState84D0& state);
};

} // namespace ravenloft::game
