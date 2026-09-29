#include "ResourceOwner79C8.h"

namespace ravenloft::game {
namespace {

std::int16_t readS16(
    const std::uint8_t* bytes) noexcept {
    return static_cast<std::int16_t>(
        std::uint16_t(bytes[0]) |
        (std::uint16_t(bytes[1]) << 8));
}

void write32(
    std::uint8_t* bytes,
    std::uint32_t value) noexcept {
    bytes[0] = static_cast<std::uint8_t>(value);
    bytes[1] = static_cast<std::uint8_t>(value >> 8);
    bytes[2] = static_cast<std::uint8_t>(value >> 16);
    bytes[3] = static_cast<std::uint8_t>(value >> 24);
}

std::uint32_t read32(
    const std::uint8_t* bytes) noexcept {
    return
        std::uint32_t(bytes[0]) |
        (std::uint32_t(bytes[1]) << 8) |
        (std::uint32_t(bytes[2]) << 16) |
        (std::uint32_t(bytes[3]) << 24);
}

} // namespace

std::uintptr_t ResourceOwner79C8::create(
    const ResourceOuter767C& outer,
    std::int32_t handle,
    std::uint8_t initialLocalF0,
    BufferedIoState72D0& streamState,
    const ResourceOwnerOps79C8& ops) {
    std::uint8_t localF0 =
        initialLocalF0;

    if (ops.call9AD4() != 0) {
        localF0 = 1;
        ops.call9AB4();
    }

    const auto* block =
        outer.descriptorBytes5 +
        static_cast<std::int32_t>(
            outer.offset3) -
        8;

    const std::int32_t length =
        readS16(block);

    const auto owner =
        Memory6F10::heapAllocate71C8(
            static_cast<std::size_t>(
                static_cast<std::uint32_t>(
                    length + 4)),
            ops.image.memory);

    auto* ownerBytes =
        reinterpret_cast<std::uint8_t*>(
            owner);

    for (std::int32_t i = 0;
         i < length;
         ++i) {
        ownerBytes[4 + i] =
            outer.descriptorBytes5[i];
    }

    ResourceOuter767C patched =
        outer;

    patched.offset3 = 8;
    patched.descriptorBytes5 = block;

    const auto resource =
        ResourceImages767C::load(
            patched,
            handle,
            streamState,
            ops.image);

    write32(
        ownerBytes,
        ops.encodePointer32(resource));

    if (localF0 != 0) {
        ops.call9AC4();
    }

    return owner;
}

void ResourceOwner79C8::release(
    std::uintptr_t owner,
    const ResourceOwnerOps79C8& ops) {
    const auto* bytes =
        reinterpret_cast<
            const std::uint8_t*>(
                owner);

    ResourceImages767C::release(
        ops.decodePointer32(
            read32(bytes)),
        ops.image);

    Memory6F10::heapFree7240(
        owner,
        ops.image.memory);
}

char* StringShift7A84::shift(
    char* text,
    std::int32_t delta) noexcept {
    char* const original = text;

    if (text != nullptr) {
        while (*text != '\0') {
            *text =
                static_cast<char>(
                    static_cast<std::uint8_t>(*text) +
                    static_cast<std::uint8_t>(delta));
            ++text;
        }
    }

    return original;
}

} // namespace ravenloft::game
