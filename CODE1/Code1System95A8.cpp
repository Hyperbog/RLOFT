#include "Code1System95A8.h"

#include <cstddef>

namespace ravenloft::game {
namespace {

std::int16_t signedWord(
    const std::array<std::uint8_t, 0x100>& bytes,
    std::size_t offset) noexcept {
    return static_cast<std::int16_t>(
        static_cast<std::uint16_t>(bytes[offset]) |
        (static_cast<std::uint16_t>(bytes[offset + 1]) << 8u));
}

} // namespace

std::int32_t Code1System95A8::farCallback(
    std::uint32_t thirdArgument,
    Code1SystemState95A8& state) noexcept {
    state.callbackValue23D0 = thirdArgument;
    return 0;
}

void Code1System95A8::initialize(
    std::uint32_t resourcePrefix,
    std::int32_t groupCount,
    Code1SystemState95A8& state,
    const Code1SystemOps95A8& ops) {
    // 0x95C0..0x95E5.
    state.word23EB = 0x0182;
    state.displayRows23D6 = 0x0C;
    state.byte2400 = 1;
    state.displayMode23D4 = 0xFF;
    state.specialItem2B10 = -1;

    // 0x95E8..0x9602.
    ops.registerFarCallbackE280(
        0x95A8,
        ops.currentCodeSegment());
    ops.installHandlers8398();
    ops.seed64B0(0);

    // 0x9605..0x9642.
    if (state.mode309C != 2) {
        state.config2AA0 =
            ops.allocateConfig256();
        state.config2AA0->fill(0);
    } else {
        state.config2AA0 =
            ops.copyConfigE360(
                state.externalConfigSource30A4);
    }

    auto& config = *state.config2AA0;

    // 0x9647..0x9689. Preserve the original lack of an open-failure check.
    if (config[0x64] == 0) {
        const std::int32_t handle =
            ops.openRead62A8("sound.cfg");

        (void)ops.read6304(
            config.data(),
            0x3B,
            handle);

        (void)ops.close62F0(handle);

        config[0x64] = 1;
        config[0x66] = 1;
    }

    // 0x968D.
    state.input2A9C = ops.callCF98();

    // 0x9697..0x96E7.
    if (signedWord(config, 0x08) != 0x71) {
        if (signedWord(config, 0x14) != 0) {
            ops.callC2D0(
                config.data() + 0x24,
                signedWord(config, 0x0A),
                signedWord(config, 0x0C),
                signedWord(config, 0x0E),
                signedWord(config, 0x10));
        }

        ops.callB0F8(
            config.data() + 0x16,
            signedWord(config, 0x00),
            signedWord(config, 0x02),
            signedWord(config, 0x04),
            signedWord(config, 0x06));
    }

    // 0x96EA..0x9705.
    ops.callD10C(0, 0x27E);
    ops.callD130(0, 0xC7);

    // 0x9708..0x971E.
    if (resourcePrefix != 0) {
        ops.openGroups8A04(
            resourcePrefix,
            groupCount);
        state.resourceIndexPresent = true;
    }
}

void Code1System95A8::setSpecial(
    std::int32_t value2B0C,
    std::int32_t specialItem,
    std::uint8_t color,
    Code1SystemState95A8& state,
    const Code1SystemOps95A8& ops) {
    // 0x9727..0x973D.
    if (state.specialItem2B10 != -1) {
        ops.unpin9488(
            state.specialItem2B10,
            0);
    }

    // 0x9740..0x9760.
    state.specialItem2B10 = specialItem;
    ops.load8E3C(specialItem, 0);
    ops.pin9440(specialItem, 0);

    // 0x9763..0x976E.
    state.value2B0C = value2B0C;
    state.color2B14 = color;
}

void Code1System95A8::shutdown(
    Code1SystemState95A8& state,
    const Code1SystemOps95A8& ops) {
    // 0x977D. The return value is deliberately ignored.
    (void)ops.callCF98();

    const auto& config = *state.config2AA0;
    const std::uint32_t anySubsystem =
        state.flags2B84 | state.flags2F48;

    // 0x9794..0x97B7.
    if (signedWord(config, 0x08) != 0x71) {
        ops.callB430();

        if (signedWord(config, 0x14) != 0) {
            ops.callC4A0();
        }

        if (anySubsystem != 0) {
            ops.callE6EA(0);
        }
    }

    // 0x97BA..0x97C8.
    if (state.resourceIndexPresent) {
        ops.shutdownIndex8C40();
        state.resourceIndexPresent = false;
    }

    ops.restoreHandlers8424();
}

void Code1System95A8::exitWithCode(
    std::int32_t code,
    const Code1SystemOps95A8& ops) {
    ops.callEDE0();

    // Original 0x97E0 is a tail jump to 0xCBF0.
    ops.callCBF0(code);
}

void Code1System95A8::shutdownDisplay(
    std::int32_t exitAfter,
    Code1SystemState95A8& state,
    const Code1SystemOps95A8& ops) {
    ops.display64DC(0xFF);
    shutdown(state, ops);

    if (exitAfter != 0) {
        exitWithCode(1, ops);
    }
}

void Code1System95A8::fatal(
    const std::string& message,
    Code1SystemState95A8& state,
    const Code1SystemOps95A8& ops) {
    if (state.specialItem2B10 != -1 &&
        (state.flags2B01 & 0x80u) == 0) {
        const std::int32_t response =
            prompt(message, 0, state, ops);

        shutdownDisplay(0, state, ops);

        if (response == 0x54) {
            ops.dosPrint61D0(message);
        }
    } else {
        shutdownDisplay(0, state, ops);
        ops.dosPrint61D0(message);
    }

    exitWithCode(1, ops);
}

std::int32_t Code1System95A8::prompt(
    const std::string& message,
    std::int32_t mode,
    Code1SystemState95A8& state,
    const Code1SystemOps95A8& ops) {
    // 0x987D / 0x9A9C: text-mode fallback.
    if (state.displayMode23D4 == 0xFF) {
        ops.dosPrint61D0(message);
        return 0;
    }

    std::uint8_t savedDisplayParameter = 0xFF;
    std::int32_t lineHeight = 0x140;

    const std::uint8_t savedPage =
        state.activePage23E5;
    const std::int32_t savedTextX =
        ops.getTextX7DDC();
    const std::int32_t savedTextY =
        ops.getTextY7DE8();

    // 0x98AB..0x98E9.
    if (state.displayMode23D4 != 5) {
        if (state.activePage23E5 !=
            state.drawPage23E6) {
            ops.setActivePage6588(
                state.drawPage23E6);
            state.activePage23E5 =
                state.drawPage23E6;
        }
    } else {
        ops.setActivePage6588(0);
        state.activePage23E5 = 0;
        lineHeight -= 8;
    }

    // 0x98E9..0x991F.
    const std::size_t captureSize =
        static_cast<std::size_t>(
            state.displayRows23D6) *
        0x140u;

    const std::uintptr_t savedScreen =
        ops.heapAllocate71C8(captureSize);

    ops.capture66C0(
        savedScreen,
        0,
        0,
        state.displayRows23D6,
        0x140);

    // 0x9922..0x994A.
    for (std::int32_t x = 0;
         x < state.displayRows23D6;
         ++x) {
        ops.lineA05C(
            x,
            0,
            x,
            lineHeight - 1,
            static_cast<std::uint8_t>(
                state.color2B14 - x));
    }

    // 0x994C..0x9997.
    if (state.specialItem2B10 != -1 &&
        !message.empty()) {
        const std::int32_t width =
            ops.textWidth7AB4(
                message,
                state.specialItem2B10);

        // x86 sequence at 0x996F performs signed /2 with truncation toward 0.
        const std::int32_t centeredX =
            0xA0 - (width / 2);

        ops.setTextPos7D68(
            centeredX,
            0);

        ops.drawText7C0C(
            message,
            state.specialItem2B10);
    }

    // 0x999A..0x99D8.
    if (state.displayMode23D4 == 5 &&
        state.drawPage23E6 != 0) {
        savedDisplayParameter =
            static_cast<std::uint8_t>(
                state.word23DF);

        const std::int32_t limit =
            0xC8 -
            static_cast<std::int32_t>(
                state.displayRows23D6);

        if (state.word23DF > limit) {
            ops.setDisplayParamA474(limit);
        }
    }

    std::int32_t result = 0;

    // 0x99D8..0x9A2B.
    if (mode > 1) {
        result =
            static_cast<std::int32_t>(
                ops.wait87D8(
                    static_cast<std::uint32_t>(mode),
                    0));
    } else if (mode == 1) {
        do {
            result =
                static_cast<std::int32_t>(
                    ops.readKey82C0());
        } while (result != 0x4E &&
                 result != 0x59 &&
                 result != 0x53);

        if (result == 0x53) {
            result = 0x59;
        }
    } else {
        do {
            result =
                static_cast<std::int32_t>(
                    ops.readKey82C0());
        } while (result != 0x20 &&
                 result != 0x54);
    }

    // 0x9A2B..0x9A6A.
    ops.restore6744(
        savedScreen,
        0,
        0,
        state.displayRows23D6,
        0x140);

    ops.heapFree7240(savedScreen);

    ops.setActivePage6588(savedPage);
    state.activePage23E5 = savedPage;

    ops.setTextPos7D68(
        savedTextX,
        savedTextY);

    // 0x9A6D..0x9A92.
    if (state.displayMode23D4 == 5 &&
        savedDisplayParameter != 0xFF &&
        state.drawPage23E6 != 0) {
        ops.setDisplayParamA474(
            savedDisplayParameter);
    }

    return result;
}

void Code1System95A8::suspendUi(
    Code1SystemState95A8& state) noexcept {
    state.flags2B01 |= 0x80u;
}

void Code1System95A8::resumeUi(
    Code1SystemState95A8& state) noexcept {
    state.flags2B01 &= 0x7Fu;
}

std::int32_t Code1System95A8::uiActive(
    const Code1SystemState95A8& state) noexcept {
    return (state.flags2B01 & 0x80u) == 0
        ? 1
        : 0;
}

} // namespace ravenloft::game
