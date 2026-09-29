#include "TimerKeyboard7F50.h"

#include <cstring>

namespace ravenloft::game {

std::uint32_t TimerKeyboard7F50::ticks(
    const TimerKeyboardState7F50& state) noexcept {
    return state.tick2934;
}

void TimerKeyboard7F50::timerInterrupt(
    TimerKeyboardState7F50& state,
    const TimerKeyboardOps7F50& ops) {
    ops.callCEC0();
    const std::uint32_t prior = state.tick2934;
    ++state.tick2934;
    ops.callDDE0(prior);
    ops.chainOldInt8();
}

void TimerKeyboard7F50::setTicks(
    std::uint32_t value,
    TimerKeyboardState7F50& state,
    const TimerKeyboardOps7F50& ops) {
    ops.enterCriticalDA18();
    state.tick2934 = value;
    ops.leaveCriticalDA1A();
}

void TimerKeyboard7F50::removeQueuedScan(
    std::uint8_t scan,
    TimerKeyboardState7F50& state) noexcept {
    std::uint32_t index = 0;
    while (index < state.queueCount2930) {
        if (state.queue89D0[index] == scan) {
            const std::uint32_t moveCount =
                static_cast<std::uint32_t>(state.queueCount2930) - index - 1u;
            std::memmove(state.queue89D0.data() + index,
                         state.queue89D0.data() + index + 1u,
                         moveCount);
            --state.queueCount2930;
            break;
        }
        ++index;
    }

    if (state.queueCount2930 == 0) {
        state.currentScan291A = 0;
    } else {
        state.currentScan291A =
            state.queue89D0[static_cast<std::uint8_t>(state.queueCount2930 - 1u)];
    }
    state.previousScan2919 = state.currentScan291A;
}

void TimerKeyboard7F50::queueScan(
    std::uint8_t scan,
    TimerKeyboardState7F50& state) noexcept {
    for (std::uint32_t index = 0; index < state.queueCount2930; ++index) {
        if (state.queue89D0[index] == scan) {
            removeQueuedScan(scan, state);
            break;
        }
    }

    const std::uint8_t index = state.queueCount2930;
    state.queue89D0[index] = scan;
    ++state.queueCount2930;
    state.currentScan291A = scan;
    state.previousScan2919 = state.currentScan291A;
}

std::int32_t TimerKeyboard7F50::queued(
    std::uint8_t scan,
    const TimerKeyboardState7F50& state) noexcept {
    for (std::uint32_t index = 0; index < state.queueCount2930; ++index) {
        if (state.queue89D0[index] == scan) {
            return 1;
        }
    }
    return 0;
}

void TimerKeyboard7F50::keyboardInterrupt(
    TimerKeyboardState7F50& state,
    const TimerKeyboardOps7F50& ops) {
    ops.callCEC0();

    std::uint32_t scan = ops.inPortDDE7(0x60);

    state.port61Latch2918 = ops.inPortDDE7(0x61);
    if (state.port61Latch2918 == 0x80u) {
        state.port61Latch2918 = ops.inPortDDE7(0x61);
        state.port61Latch2918 |= 0x80u;
    }

    ops.outPortDDF2(0x61,
        static_cast<std::uint8_t>(state.port61Latch2918 | 0x80u));
    ops.outPortDDF2(0x61, state.port61Latch2918);

    ops.enterCriticalDA18();

    if (scan > 0x80u) {
        scan &= 0x7Fu;

        if (scan == 0x2Au) {
            state.modifiers291B &= 0xFEu;
        } else if (scan == 0x36u) {
            state.modifiers291B &= 0xFDu;
        } else if (scan == 0x38u) {
            state.modifiers291B &= 0xFBu;
        } else if (scan == 0x1Du) {
            state.modifiers291B &= 0xF7u;
        } else if (scan >= 0x47u && scan <= 0x51u) {
            removeQueuedScan(static_cast<std::uint8_t>(scan), state);
        } else {
            state.currentScan291A = state.previousScan2919;
        }
    } else {
        if (scan == 0x2Au) {
            state.modifiers291B |= 0x01u;
        } else if (scan == 0x36u) {
            state.modifiers291B |= 0x02u;
        } else if (scan == 0x38u) {
            state.modifiers291B |= 0x04u;
        } else if (scan == 0x1Du) {
            state.modifiers291B |= 0x08u;
        } else if (scan >= 0x47u && scan <= 0x51u) {
            queueScan(static_cast<std::uint8_t>(scan), state);
            state.previousScan2919 = static_cast<std::uint8_t>(scan);
        }

        state.currentScan291A = static_cast<std::uint8_t>(scan);
    }

    if (state.currentScan291A == 0x10u &&
        (state.modifiers291B & 0x04u) != 0) {
        state.flags2B01 |= 0x01u;
        state.currentScan291A = 0;
    } else if (state.currentScan291A == 0x43u) {
        state.currentScan291A = 0;
    } else if (state.currentScan291A == 0x01u) {
        state.flags2B01 |= 0x02u;
    } else if (state.currentScan291A == 0x39u) {
        state.spaceEvent291C = 1;
    }

    ops.outPortDDF2(0x20, 0x20);
    ops.leaveCriticalDA1A();
}

void TimerKeyboard7F50::noop8270() noexcept {}

std::uint32_t TimerKeyboard7F50::modifiers(
    std::uint32_t mask,
    const TimerKeyboardState7F50& state) noexcept {
    return static_cast<std::uint32_t>(state.modifiers291B) & mask;
}

std::uint32_t TimerKeyboard7F50::takeSpaceEvent(
    TimerKeyboardState7F50& state) noexcept {
    const std::uint32_t value = state.spaceEvent291C;
    state.spaceEvent291C = 0;
    return value;
}

std::int32_t TimerKeyboard7F50::keyPending(
    TimerKeyboardState7F50& state,
    const TimerKeyboardOps7F50& ops) {
    ops.service9AE8();
    return state.currentScan291A != 0 ? 1 : 0;
}

std::uint32_t TimerKeyboard7F50::readKey(
    TimerKeyboardState7F50& state,
    const TimerKeyboardOps7F50& ops) {
    while (state.currentScan291A == 0) {
        ops.service9AE8();
    }
    const std::uint32_t result = state.keyMap2938[state.currentScan291A];
    state.currentScan291A = 0;
    return result;
}

void TimerKeyboard7F50::clearQueue(
    TimerKeyboardState7F50& state) noexcept {
    state.queueCount2930 = 0;
    state.previousScan2919 = 0;
}

void TimerKeyboard7F50::restoreCurrent(
    TimerKeyboardState7F50& state) noexcept {
    state.currentScan291A = state.previousScan2919;
    state.spaceEvent291C = 0;
}

std::uint32_t TimerKeyboard7F50::peekKey(
    TimerKeyboardState7F50& state,
    const TimerKeyboardOps7F50& ops) {
    ops.service9AE8();
    return state.keyMap2938[state.currentScan291A];
}

void TimerKeyboard7F50::divideErrorInterrupt(
    TimerKeyboardState7F50& state,
    const TimerKeyboardOps7F50& ops) {
    ops.callCEC0();
    ops.display64DC(0xFF);
    ops.callCF98();
    ops.setVectorDDFE(9, state.oldInt9);
    ops.setVectorDDFE(8, state.oldInt8);
    ops.chainOldInt0();
}

void TimerKeyboard7F50::installHandlers(
    TimerKeyboardState7F50& state,
    const TimerKeyboardOps7F50& ops) {
    if (state.handlersInstalled291D != 0) {
        return;
    }

    state.oldInt9 = ops.getVectorDE35(9);
    ops.setVectorDDFE(9,
        InterruptVector7F50{0x80BCu, ops.currentCodeSegment()});

    state.oldInt8 = ops.getVectorDE35(8);
    ops.setVectorDDFE(8,
        InterruptVector7F50{0x7F5Cu, ops.currentCodeSegment()});

    state.oldInt0 = ops.getVectorDE35(0);
    ops.setVectorDDFE(0,
        InterruptVector7F50{0x8338u, ops.currentCodeSegment()});

    state.handlersInstalled291D = 1;
}

void TimerKeyboard7F50::restoreHandlers(
    TimerKeyboardState7F50& state,
    const TimerKeyboardOps7F50& ops) {
    if (state.handlersInstalled291D == 0) {
        return;
    }

    ops.setVectorDDFE(9, state.oldInt9);
    ops.setVectorDDFE(8, state.oldInt8);
    ops.setVectorDDFE(0, state.oldInt0);
    state.handlersInstalled291D = 0;
}

std::int32_t TimerKeyboard7F50::handlersInstalled(
    const TimerKeyboardState7F50& state) noexcept {
    return state.handlersInstalled291D;
}

} // namespace ravenloft::game
