#pragma once

#include "Code1ResourceImages767C.h"

#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct Code1ResourceOwnerOps79C8 {
    Code1ResourceImageOps767C image;

    std::function<std::int32_t()> call9AD4;
    std::function<void()> call9AB4;
    std::function<void()> call9AC4;

    // Original owner records store a four-byte pointer at +0.
    // The source-port backend maps modern host addresses to/from that token.
    std::function<std::uint32_t(std::uintptr_t)> encodePointer32;
    std::function<std::uintptr_t(std::uint32_t)> decodePointer32;
};

class Code1ResourceOwner79C8 {
public:
    // CODE.1 0x79C6..0x79C7 is alignment.

    // Direct reconstruction of CODE.1 0x79C8..0x7A60.
    //
    // [ebp-0x10] is genuinely uninitialized in the original when 0x9AD4
    // returns zero. initialLocalF0 exposes that incoming stack byte rather than
    // silently "fixing" the original quirk.
    static std::uintptr_t create(
        const Code1ResourceOuter767C& outer,
        std::int32_t handle,
        std::uint8_t initialLocalF0,
        Code1BufferedIoState72D0& streamState,
        const Code1ResourceOwnerOps79C8& ops);

    // CODE.1 0x7A61..0x7A63 is alignment.
    // Direct reconstruction of CODE.1 0x7A64..0x7A83.
    static void release(
        std::uintptr_t owner,
        const Code1ResourceOwnerOps79C8& ops);
};

class Code1StringShift7A84 {
public:
    // Direct reconstruction of CODE.1 0x7A84..0x7AB0.
    // CODE.1 0x7AB1..0x7AB3 is alignment.
    static char* shift(
        char* text,
        std::int32_t delta) noexcept;
};

} // namespace ravenloft::game
