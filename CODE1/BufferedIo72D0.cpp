#include "BufferedIo72D0.h"

#include "RuntimeMemset84A0.h"

namespace ravenloft::game {

void BufferedIo72D0::setHandle(
    std::int32_t handle,
    BufferedIoState72D0& state) noexcept {
    state.handle2904 = handle;
}

void BufferedIo72D0::alignAndFill(
    BufferedIoState72D0& state,
    const BufferedIoOps72D0& ops) {
    const std::int32_t position =
        DosServices6200::seek(
            0, 1, state.handle2904, ops.dos);

    const std::int32_t remainder =
        position & 0x3FF;

    const std::int32_t count =
        0x400 - remainder;

    state.bufferRemaining2874 =
        DosServices6200::read(
            state.buffer2470.data(),
            count,
            state.handle2904,
            ops.dos);

    state.bufferPos2870 = 0;
}

void BufferedIo72D0::refill(
    BufferedIoState72D0& state,
    const BufferedIoOps72D0& ops) {
    if (state.bufferRemaining2874 == 0) {
        state.bufferRemaining2874 =
            DosServices6200::read(
                state.buffer2470.data(),
                0x400,
                state.handle2904,
                ops.dos);

        state.bufferPos2870 = 0;
    }
}

std::uint8_t BufferedIo72D0::readByte(
    BufferedIoState72D0& state,
    const BufferedIoOps72D0& ops) {
    if (state.bufferRemaining2874 == 0) {
        refill(state, ops);
    }

    const std::uint8_t value =
        state.buffer2470[state.bufferPos2870];

    --state.bufferRemaining2874;
    ++state.bufferPos2870;

    return value;
}

void BufferedIo72D0::emitByte(
    std::uint8_t value,
    BufferedIoState72D0& state,
    const BufferedIoOps72D0& ops) {
    if (state.outputDone89CE != 0) {
        return;
    }

    std::uint8_t* destination =
        state.outputMode89CF != 0
            ? state.processedBuffer89C8
            : state.rawBuffer89C4;

    destination[state.outputPos89B8] =
        value;

    ++state.outputPos89B8;

    if (state.outputPos89B8 <
        state.outputCapacity89A4) {
        return;
    }

    ops.call9AE8();
    state.outputPos89B8 = 0;

    if (state.outputMode89CF != 0) {
        ops.callA57C(
            state.outputIndex89B4,
            state.outputCapacity89A4,
            state.processedBuffer89C8,
            state.outputArg899C,
            state.descriptor8994[5]);

        ++state.outputIndex89B4;

        const std::int16_t count =
            static_cast<std::int16_t>(
                std::uint16_t(
                    state.descriptor8994[0]) |
                (std::uint16_t(
                    state.descriptor8994[1]) << 8));

        if (count >
            state.outputIndex89B4) {
            return;
        }
    }

    state.outputDone89CE = 1;
}

void BufferedIo72D0::beginOutput(
    std::uintptr_t arg899C,
    const std::uint8_t* descriptor,
    std::uint8_t* buffer,
    std::int32_t capacity,
    std::uint8_t mode,
    BufferedIoState72D0& state,
    const BufferedIoOps72D0& ops) {
    state.outputMode89CF = mode;
    state.outputDone89CE = 0;
    state.descriptor8994 = descriptor;
    state.outputPos89B8 = 0;
    state.outputIndex89B4 = 0;
    state.outputCapacity89A4 = capacity;
    state.outputArg899C = arg899C;

    if (mode != 0) {
        state.processedBuffer89C8 =
            buffer;
    } else {
        state.rawBuffer89C4 =
            buffer;
    }

    ops.callDB80();
}

void BufferedIo72D0::readBytes(
    std::uint8_t* destination,
    std::int32_t count,
    BufferedIoState72D0& state,
    const BufferedIoOps72D0& ops) {
    refill(state, ops);

    if (state.bufferRemaining2874 >=
        count) {
        ops.callD9EE(
            destination,
            state.buffer2470.data() +
                state.bufferPos2870,
            static_cast<std::size_t>(count));

        state.bufferPos2870 +=
            static_cast<std::size_t>(count);

        state.bufferRemaining2874 -=
            count;

        return;
    }

    const std::int32_t first =
        state.bufferRemaining2874;

    ops.callD9EE(
        destination,
        state.buffer2470.data() +
            state.bufferPos2870,
        static_cast<std::size_t>(first));

    count -= first;

    state.bufferRemaining2874 =
        DosServices6200::read(
            state.buffer2470.data(),
            0x400,
            state.handle2904,
            ops.dos);

    ops.callD9EE(
        destination + first,
        state.buffer2470.data(),
        static_cast<std::size_t>(count));

    state.bufferPos2870 =
        static_cast<std::size_t>(count);

    state.bufferRemaining2874 -=
        count;
}

std::uint8_t BufferedIo72D0::readRleByte(
    BufferedIoState72D0& state,
    const BufferedIoOps72D0& ops) {
    if (state.rleCount2878 == 0) {
        state.rlePos287C = 0;

        const std::uint8_t control =
            readByte(state, ops);

        const std::int8_t signedControl =
            static_cast<std::int8_t>(
                control);

        if (signedControl >= 0) {
            state.rleCount2878 =
                static_cast<std::uint8_t>(
                    control + 1u);

            readBytes(
                state.rleBuffer2880.data(),
                state.rleCount2878,
                state,
                ops);
        } else {
            const std::uint8_t count =
                static_cast<std::uint8_t>(
                    -static_cast<std::int32_t>(
                        signedControl) +
                    1);

            state.rleCount2878 =
                count;

            const std::uint8_t value =
                readByte(state, ops);

            state.rleBuffer2880[0] =
                value;

            RuntimeMemset84A0::set(
                state.rleBuffer2880.data() + 1,
                value,
                static_cast<std::size_t>(
                    count - 1u));
        }
    }

    const std::uint8_t value =
        state.rleBuffer2880[
            state.rlePos287C];

    --state.rleCount2878;
    ++state.rlePos287C;

    return value;
}

void BufferedIo72D0::readRleBlock(
    std::uint8_t* destination,
    std::int32_t width,
    std::int32_t heightUnits,
    BufferedIoState72D0& state,
    const BufferedIoOps72D0& ops) {
    const std::int32_t count =
        width * heightUnits * 8;

    for (std::int32_t i = 0;
         i < count;
         ++i) {
        *destination++ =
            readRleByte(state, ops);
    }
}

void BufferedIo72D0::readRawBlock(
    std::uint8_t* destination,
    std::int32_t width,
    std::int32_t heightUnits,
    BufferedIoState72D0& state,
    const BufferedIoOps72D0& ops) {
    const std::int32_t count =
        width * heightUnits * 8;

    for (std::int32_t i = 0;
         i < count;
         ++i) {
        *destination++ =
            readByte(state, ops);
    }
}

} // namespace ravenloft::game
