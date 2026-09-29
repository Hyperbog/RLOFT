#include "InputGraphics84D0.h"

#include <utility>

namespace ravenloft::game {

std::int32_t InputGraphics84D0::inputWord0(
    const InputGraphicsState84D0& state) noexcept {
    return state.input2A9C != nullptr ? state.input2A9C->word0 : 0;
}

std::int32_t InputGraphics84D0::inputWord4IsOne(
    const InputGraphicsState84D0& state) noexcept {
    return state.input2A9C != nullptr && state.input2A9C->word4 == 1 ? 1 : 0;
}

std::int32_t InputGraphics84D0::inputWord2WhenActive(
    const InputGraphicsState84D0& state) noexcept {
    if (state.input2A9C == nullptr || state.input2A9C->word0 == 0) return 0;
    return state.input2A9C->word2;
}

void InputGraphics84D0::outlineRect(
    std::int32_t x, std::int32_t y,
    std::int32_t width, std::int32_t height,
    std::uint8_t color,
    const InputGraphicsOps84D0& ops) {
    ops.verticalLine65C0(x, y, height, color);
    if (width > 2) ops.horizontalLine65E8(x + 1, y, width - 2, color);
    if (width > 1) ops.verticalLine65C0(x + width - 1, y, height, color);
    if (width > 2 && height > 1)
        ops.horizontalLine65E8(x + 1, y + height - 1, width - 2, color);
}

void InputGraphics84D0::forEachStrip(
    std::int32_t value,
    const StripDescriptor85A8& descriptor,
    const InputGraphicsOps84D0& ops) {
    for (std::int32_t i = 0; i < descriptor.count; ++i)
        ops.callAE0C(i, value, descriptor.handle);
}

void InputGraphics84D0::drawStripClipped(
    std::int32_t x, std::int32_t y,
    std::int32_t start, std::int32_t end,
    const StripDescriptor85A8& descriptor,
    const InputGraphicsOps84D0& ops) {
    if (end == -1) end = descriptor.count;
    else end += start;
    for (std::int32_t i = start; i < end; ++i) {
        ops.call6D54(i, x, y, descriptor.handle);
        x += descriptor.stride;
    }
}

void InputGraphics84D0::drawWholeStripClipped(
    std::int32_t x, std::int32_t y,
    const StripDescriptor85A8& descriptor,
    const InputGraphicsOps84D0& ops) {
    drawStripClipped(x, y, 0, -1, descriptor, ops);
}

void InputGraphics84D0::drawStripDirect(
    std::int32_t x, std::int32_t y,
    std::int32_t start, std::int32_t end,
    const StripDescriptor85A8& descriptor,
    const InputGraphicsOps84D0& ops) {
    if (end == -1) end = descriptor.count;
    else end += start;
    for (std::int32_t i = start; i < end; ++i) {
        ops.call67C8(i, x, y, descriptor.handle);
        x += descriptor.stride;
    }
}

void InputGraphics84D0::drawWholeStripDirect(
    std::int32_t x, std::int32_t y,
    const StripDescriptor85A8& descriptor,
    const InputGraphicsOps84D0& ops) {
    drawStripDirect(x, y, 0, -1, descriptor, ops);
}

void InputGraphics84D0::drawResourceStrip(
    std::int32_t x, std::int32_t y,
    std::int32_t resourceId, std::int32_t bank,
    const InputGraphicsOps84D0& ops) {
    drawWholeStripDirect(x, y, ops.call954C(resourceId, bank), ops);
}

void InputGraphics84D0::forEachA5C8(
    const StripDescriptor85A8& descriptor,
    const InputGraphicsOps84D0& ops) {
    for (std::int32_t i = 0; i < descriptor.count; ++i)
        ops.callA5C8(i, descriptor.handle);
}

void InputGraphics84D0::forEachA678(
    const StripDescriptor85A8& descriptor,
    const InputGraphicsOps84D0& ops) {
    for (std::int32_t i = 0; i < descriptor.count; ++i)
        ops.callA678(i, descriptor.handle);
}

void InputGraphics84D0::pushActivePage(
    std::uint8_t page,
    InputGraphicsState84D0& state,
    const InputGraphicsOps84D0& ops) {
    if (state.pageDepth2B20 == 10) ops.fatal9814(0x208);
    state.pageStack89EC[state.pageDepth2B20] = state.activePage23E5;
    ++state.pageDepth2B20;
    ops.setActivePage6588(page);
    state.activePage23E5 = page;
}

void InputGraphics84D0::popActivePage(
    InputGraphicsState84D0& state,
    const InputGraphicsOps84D0& ops) {
    if (state.pageDepth2B20 == 0) ops.fatal9814(0x21C);
    --state.pageDepth2B20;
    const std::uint8_t page = state.pageStack89EC[state.pageDepth2B20];
    ops.setActivePage6588(page);
    state.activePage23E5 = page;
}

void InputGraphics84D0::startWait(
    InputGraphicsState84D0& state,
    const InputGraphicsOps84D0& ops) {
    state.waitAnchor89E8 = ops.ticks7F50();
}

std::uint32_t InputGraphics84D0::wait(
    std::uint32_t delay, std::uint32_t flags,
    InputGraphicsState84D0& state,
    const InputGraphicsOps84D0& ops) {
    std::uint32_t deadline =
        state.waitAnchor89E8 != 0
            ? state.waitAnchor89E8 + delay
            : ops.ticks7F50() + delay;

    if ((flags & 0x04u) != 0) flags &= ~0x04u;

    for (;;) {
        ops.service9AE8();

        if ((flags & 0x01u) != 0 && ops.keyPending82A4() != 0) {
            state.waitAnchor89E8 = 0;
            return ops.readKey82C0();
        }

        if ((flags & 0x02u) != 0 && inputWord0(state) != 0) {
            const auto* sample = ops.callD0B4();
            if (sample->word0 != 0) {
                const std::int32_t button = sample->word0;
                do {
                    sample = ops.callD0B4();
                } while (sample->word0 != 0);
                state.waitAnchor89E8 = 0;
                return static_cast<std::uint32_t>(button + 0x1F8);
            }
        }

        // Preserved original quirk: bit 4 was cleared above.
        if ((flags & 0x04u) != 0) {
            state.waitAnchor89E8 = 0;
            return 0x1F0u;
        }

        if (ops.ticks7F50() >= deadline) {
            state.waitAnchor89E8 = 0;
            return 0;
        }
    }
}

void InputGraphics84D0::delay6(
    const InputGraphicsOps84D0& ops,
    InputGraphicsState84D0& state) {
    ops.callDF13(0x416);
    (void)wait(6, 0, state, ops);
    ops.callDF73();
}

std::string InputGraphics84D0::unsignedDecimal(std::uint32_t value) {
    return std::to_string(value);
}

std::int32_t InputGraphics84D0::signum(std::int32_t value) noexcept {
    if (value < 0) return -1;
    if (value > 0) return 1;
    return 0;
}

void InputGraphics84D0::setCallback2B1C(
    std::function<void()> callback,
    InputGraphicsState84D0& state) {
    state.callback2B1C = std::move(callback);
}

void InputGraphics84D0::setCallback2B18(
    std::function<void()> callback,
    InputGraphicsState84D0& state) {
    state.callback2B18 = std::move(callback);
}

void InputGraphics84D0::invokeCallback2B18(
    InputGraphicsState84D0& state) {
    if (state.callback2B18) state.callback2B18();
}

} // namespace ravenloft::game
