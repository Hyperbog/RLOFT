#include "Code1InputState5230.h"

namespace ravenloft::game {

void Code1InstallHandler5230::run(
    std::int32_t value,
    Code1InputState5230& state,
    const Code1InputOps5230& ops) {
    state.word71AC = 0x0C;
    state.word71B4 = static_cast<std::uint16_t>(value);
    state.dword71B8 = 0x6104;
    state.word71C8 = ops.currentCodeSegment();

    ops.callCEC8(
        0x33,
        &state.word71AC,
        &state.block7190,
        &state.word71C8);
}

void Code1InputSnapshot527C::get(
    std::int16_t& first,
    std::int16_t& second,
    std::int16_t& third,
    const Code1InputState5230& state) noexcept {
    first = state.value71D8;
    second = state.value71D6;
    third = state.value71D4;
}

void Code1InputInit52AC::run(
    Code1InputState5230& state,
    const Code1InputOps5230& ops) {
    state.word71DA =
        static_cast<std::int16_t>(ops.call84D0());

    if (state.word71DA == 0) {
        return;
    }

    ops.callD130(0, 0x18F);
    ops.callD10C(0, 0x27E);
    ops.callD08C(0xC8, 0xA0);

    const auto* sample = ops.callD0B4();

    state.value71D8 = sample->word6;
    state.mouseX73E6 = sample->word6;

    state.value71D6 = sample->word4;
    state.mouseY73E8 = sample->word4;

    state.word73EC = 0;
    state.value71D4 = 0;
    state.word73EA = 0;

    state.byte73EE = state.byte23E6;

    const std::uintptr_t resource =
        ops.call954C(state.value7134 + 0x2F, 0);

    ops.callD1E8(
        &state.block71DC,
        resource,
        0x20A);

    ops.call6BC0(0, 0, 0x18F, 0x13F);

    Code1InstallHandler5230::run(
        0xFFFF,
        state,
        ops);
}

void Code1InputShutdown538C::run(
    Code1InputState5230& state,
    const Code1InputOps5230& ops) {
    if (state.word71DA != 0) {
        if (state.word73EC != 0) {
            ops.call5780();
        }

        ops.callCF98();
    }
}

std::int32_t Code1InputPoll53B4::poll(
    Code1InputState5230& state,
    const Code1InputOps5230& ops) {
    ops.call9AE8();

    if (state.word71DA == 0) {
        return 0;
    }

    const auto* sample = ops.callD0B4();
    state.word73EA = sample->word0;

    return static_cast<std::int32_t>(sample->word0);
}

} // namespace ravenloft::game
