#include "Code1ResourceImages767C.h"

namespace ravenloft::game {
namespace {

std::uint16_t get16(
    const std::uint8_t* bytes,
    std::size_t offset) {
    return
        std::uint16_t(bytes[offset]) |
        (std::uint16_t(bytes[offset + 1]) << 8);
}

std::int16_t getS16(
    const std::uint8_t* bytes,
    std::size_t offset) {
    return static_cast<std::int16_t>(
        get16(bytes, offset));
}

void put16(
    std::uint8_t* bytes,
    std::size_t offset,
    std::uint16_t value) {
    bytes[offset] =
        static_cast<std::uint8_t>(value);
    bytes[offset + 1] =
        static_cast<std::uint8_t>(
            value >> 8);
}

Code1ResourceDescriptor767C decodeDescriptor(
    const std::uint8_t* bytes) {
    Code1ResourceDescriptor767C descriptor{};
    descriptor.frameCount = getS16(bytes, 0);
    descriptor.width = getS16(bytes, 2);
    descriptor.heightUnits = bytes[4];
    descriptor.marker = bytes[5];
    descriptor.encoding = bytes[6];
    return descriptor;
}

void initializeHeader(
    std::uint8_t* bytes,
    std::uint16_t word0,
    std::uint16_t frameCount,
    std::uint16_t width,
    std::uint8_t heightUnits,
    std::int32_t marker) {
    put16(bytes, 0, word0);
    put16(bytes, 2, frameCount);
    put16(bytes, 4, width);
    bytes[6] = heightUnits;
    bytes[7] = static_cast<std::uint8_t>(
        marker != 0xFF ? 0x80 : 0);
}

} // namespace

std::uintptr_t Code1ResourceImages767C::load(
    const Code1ResourceOuter767C& outer,
    std::int32_t handle,
    Code1BufferedIoState72D0& streamState,
    const Code1ResourceImageOps767C& ops) {
    Code1BufferedIo72D0::setHandle(
        handle, streamState);

    // 0x768D..0x7693: follow DWORD outer+5 before reading the descriptor.
    const auto descriptor =
        decodeDescriptor(
            outer.descriptorBytes5);

    if (descriptor.frameCount == 0) {
        ops.call9814(0x158);
    }

    const std::int32_t total =
        ops.callA558(
            descriptor.frameCount,
            descriptor.width,
            descriptor.heightUnits,
            8,
            descriptor.marker);

    const auto resource =
        Code1Memory6F10::heapAllocate71C8(
            static_cast<std::size_t>(
                static_cast<std::uint32_t>(
                    total + 8)),
            ops.memory);

    auto* header =
        reinterpret_cast<std::uint8_t*>(
            resource);

    initializeHeader(
        header,
        static_cast<std::uint16_t>(
            total / descriptor.frameCount),
        static_cast<std::uint16_t>(
            descriptor.frameCount),
        static_cast<std::uint16_t>(
            descriptor.width),
        descriptor.heightUnits,
        descriptor.marker);

    const std::int32_t bytesPerFrame =
        descriptor.width *
        static_cast<std::int32_t>(
            descriptor.heightUnits) *
        8;

    const auto temporary =
        Code1Memory6F10::heapAllocate71C8(
            static_cast<std::size_t>(
                static_cast<std::uint32_t>(
                    bytesPerFrame)),
            ops.memory);

    auto* temporaryBytes =
        reinterpret_cast<std::uint8_t*>(
            temporary);

    if (temporary == 0) {
        ops.call9814(0x17C);
    }

    Code1BufferedIo72D0::alignAndFill(
        streamState,
        ops.stream);

    switch (descriptor.encoding) {
    case 0:
        for (std::int32_t frame = 0;
             frame < descriptor.frameCount;
             ++frame) {
            ops.call9AE8();

            Code1BufferedIo72D0::readRawBlock(
                temporaryBytes,
                descriptor.width,
                descriptor.heightUnits,
                streamState,
                ops.stream);

            ops.stream.callA57C(
                frame,
                bytesPerFrame,
                temporaryBytes,
                resource,
                descriptor.marker);
        }
        break;

    case 1:
        for (std::int32_t frame = 0;
             frame < descriptor.frameCount;
             ++frame) {
            ops.call9AE8();

            Code1BufferedIo72D0::readRleBlock(
                temporaryBytes,
                descriptor.width,
                descriptor.heightUnits,
                streamState,
                ops.stream);

            ops.stream.callA57C(
                frame,
                bytesPerFrame,
                temporaryBytes,
                resource,
                descriptor.marker);
        }
        break;

    case 2:
        ops.call9814(0x1A0);
        break;

    case 3:
        Code1BufferedIo72D0::beginOutput(
            resource,
            outer.descriptorBytes5,
            temporaryBytes,
            bytesPerFrame,
            1,
            streamState,
            ops.stream);
        break;

    default:
        ops.call9814(0x1BC);
        break;
    }

    Code1Memory6F10::heapFree7240(
        temporary,
        ops.memory);

    if (header[7] != 0) {
        ops.callDC90(resource);
    }

    return resource;
}

void Code1ResourceImages767C::release(
    std::uintptr_t resource,
    const Code1ResourceImageOps767C& ops) {
    Code1Memory6F10::heapFree7240(
        resource,
        ops.memory);
}

std::int32_t Code1ResourceImages767C::allocationSize(
    std::uintptr_t resource) noexcept {
    const auto* bytes =
        reinterpret_cast<
            const std::uint8_t*>(
                resource);

    const std::uint16_t product =
        static_cast<std::uint16_t>(
            get16(bytes, 0) *
            get16(bytes, 2));

    std::int32_t result =
        static_cast<std::int32_t>(
            product) +
        8;

    if ((bytes[7] & 0x80u) != 0) {
        ++result;
    }

    return result;
}

std::uintptr_t Code1ResourceImages767C::capture(
    std::int32_t x,
    std::int32_t y,
    std::int32_t width,
    std::int32_t height,
    std::int32_t marker,
    Code1GraphicsState66C0& graphicsState,
    const Code1ResourceImageOps767C& ops) {
    if ((y & 3) != 0 ||
        (height & 7) != 0) {
        ops.call9814(0x1E0);
    }

    const std::int32_t total =
        ops.callA558(
            1,
            width,
            height / 8,
            8,
            marker);

    const auto resource =
        Code1Memory6F10::heapAllocate71C8(
            static_cast<std::size_t>(
                static_cast<std::uint32_t>(
                    total + 8)),
            ops.memory);

    auto* bytes =
        reinterpret_cast<std::uint8_t*>(
            resource);

    initializeHeader(
        bytes,
        static_cast<std::uint16_t>(total),
        1,
        static_cast<std::uint16_t>(width),
        static_cast<std::uint8_t>(
            height / 8),
        marker);

    const std::size_t markerBytes =
        marker != 0xFF ? 1u : 0u;

    Code1Graphics66C0::capture(
        resource + 8u + markerBytes,
        x,
        y,
        width,
        height,
        graphicsState,
        ops.graphics);

    if (marker != 0xFF) {
        bytes[8] =
            static_cast<std::uint8_t>(
                marker);
    }

    return resource;
}

std::uintptr_t Code1ResourceImages767C::fillCaptured(
    std::uintptr_t destination,
    std::int32_t word0Value,
    std::int32_t x,
    std::int32_t y,
    std::int32_t width,
    std::int32_t height,
    std::int32_t marker,
    Code1GraphicsState66C0& graphicsState,
    const Code1ResourceImageOps767C& ops) {
    auto* bytes =
        reinterpret_cast<std::uint8_t*>(
            destination);

    initializeHeader(
        bytes,
        static_cast<std::uint16_t>(
            word0Value),
        1,
        static_cast<std::uint16_t>(width),
        static_cast<std::uint8_t>(
            height / 8),
        marker);

    const std::size_t markerBytes =
        marker != 0xFF ? 1u : 0u;

    Code1Graphics66C0::capture(
        destination + 8u + markerBytes,
        x,
        y,
        width,
        height,
        graphicsState,
        ops.graphics);

    if (marker != 0xFF) {
        bytes[8] =
            static_cast<std::uint8_t>(
                marker);
    }

    return destination;
}

} // namespace ravenloft::game
