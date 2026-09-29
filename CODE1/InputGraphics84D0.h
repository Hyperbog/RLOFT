#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <string>

namespace ravenloft::game {

struct InputDeviceRecord84D0 {
    std::int16_t word0 = 0;
    std::int16_t word2 = 0;
    std::int16_t word4 = 0;
};

struct StripDescriptor85A8 {
    std::uintptr_t handle = 0;
    std::uint16_t count = 0;
    std::uint16_t stride = 0;
};

struct InputGraphicsState84D0 {
    InputDeviceRecord84D0* input2A9C = nullptr;
    std::uint8_t activePage23E5 = 0;
    std::uint8_t pageDepth2B20 = 0;
    std::array<std::uint8_t, 10> pageStack89EC{};
    std::uint32_t waitAnchor89E8 = 0;
    std::function<void()> callback2B1C;
    std::function<void()> callback2B18;
};

struct InputGraphicsOps84D0 {
    std::function<void(std::int32_t, std::int32_t, std::int32_t, std::uint8_t)> verticalLine65C0;
    std::function<void(std::int32_t, std::int32_t, std::int32_t, std::uint8_t)> horizontalLine65E8;
    std::function<void(std::int32_t, std::int32_t, std::uintptr_t)> callAE0C;
    std::function<void(std::int32_t, std::int32_t, std::int32_t, std::uintptr_t)> call6D54;
    std::function<void(std::int32_t, std::int32_t, std::int32_t, std::uintptr_t)> call67C8;
    std::function<StripDescriptor85A8(std::int32_t, std::int32_t)> call954C;
    std::function<void(std::int32_t, std::uintptr_t)> callA5C8;
    std::function<void(std::int32_t, std::uintptr_t)> callA678;
    std::function<void(std::int32_t)> fatal9814;
    std::function<void(std::uint8_t)> setActivePage6588;
    std::function<std::uint32_t()> ticks7F50;
    std::function<void()> service9AE8;
    std::function<std::int32_t()> keyPending82A4;
    std::function<std::uint32_t()> readKey82C0;
    std::function<const InputDeviceRecord84D0*()> callD0B4;
    std::function<void(std::int32_t)> callDF13;
    std::function<void()> callDF73;
};

class InputGraphics84D0 {
public:
    static std::int32_t inputWord0(const InputGraphicsState84D0& state) noexcept;
    static std::int32_t inputWord4IsOne(const InputGraphicsState84D0& state) noexcept;
    static std::int32_t inputWord2WhenActive(const InputGraphicsState84D0& state) noexcept;

    static void outlineRect(std::int32_t x, std::int32_t y,
                            std::int32_t width, std::int32_t height,
                            std::uint8_t color,
                            const InputGraphicsOps84D0& ops);

    static void forEachStrip(std::int32_t value,
                             const StripDescriptor85A8& descriptor,
                             const InputGraphicsOps84D0& ops);

    static void drawStripClipped(std::int32_t x, std::int32_t y,
                                 std::int32_t start, std::int32_t end,
                                 const StripDescriptor85A8& descriptor,
                                 const InputGraphicsOps84D0& ops);
    static void drawWholeStripClipped(std::int32_t x, std::int32_t y,
                                      const StripDescriptor85A8& descriptor,
                                      const InputGraphicsOps84D0& ops);
    static void drawStripDirect(std::int32_t x, std::int32_t y,
                                std::int32_t start, std::int32_t end,
                                const StripDescriptor85A8& descriptor,
                                const InputGraphicsOps84D0& ops);
    static void drawWholeStripDirect(std::int32_t x, std::int32_t y,
                                     const StripDescriptor85A8& descriptor,
                                     const InputGraphicsOps84D0& ops);
    static void drawResourceStrip(std::int32_t x, std::int32_t y,
                                  std::int32_t resourceId, std::int32_t bank,
                                  const InputGraphicsOps84D0& ops);

    static void forEachA5C8(const StripDescriptor85A8& descriptor,
                            const InputGraphicsOps84D0& ops);
    static void forEachA678(const StripDescriptor85A8& descriptor,
                            const InputGraphicsOps84D0& ops);

    static void pushActivePage(std::uint8_t page,
                               InputGraphicsState84D0& state,
                               const InputGraphicsOps84D0& ops);
    static void popActivePage(InputGraphicsState84D0& state,
                              const InputGraphicsOps84D0& ops);

    static void startWait(InputGraphicsState84D0& state,
                          const InputGraphicsOps84D0& ops);
    static std::uint32_t wait(std::uint32_t delay, std::uint32_t flags,
                              InputGraphicsState84D0& state,
                              const InputGraphicsOps84D0& ops);
    static void delay6(const InputGraphicsOps84D0& ops,
                       InputGraphicsState84D0& state);

    // 0x88D8 signed conversion was already reconstructed earlier.
    static std::string unsignedDecimal(std::uint32_t value);
    static std::int32_t signum(std::int32_t value) noexcept;

    static void setCallback2B1C(std::function<void()> callback,
                                InputGraphicsState84D0& state);
    static void setCallback2B18(std::function<void()> callback,
                                InputGraphicsState84D0& state);
    static void invokeCallback2B18(InputGraphicsState84D0& state);
};

} // namespace ravenloft::game
