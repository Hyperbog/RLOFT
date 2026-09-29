#pragma once

// CODE.1 0xCBF0..0x11301: linked 32-bit DOS/Watcom runtime-support tail.
//
// This range is classified rather than rewritten as Ravenloft game logic.
// Evidence inside the original image includes:
//
//   * 0xCBF0/0xCC02 startup/termination thunks through imported runtime slots.
//   * 0xCC1D embedded "WATCOM C/C++32 Run-Time system" copyright text.
//   * 0xCC97 onward process/DOS-extender startup, PSP/environment handling,
//     runtime initialization and termination.
//   * 0xCEC8 onward low-level runtime/DOS service wrappers, including the
//     mouse-driver INT 33h support beginning in the 0xCFxx/0xD0xx region.
//   * 0xFFD4 onward the dense interrupt-vector thunk table (INT 00h, 01h,
//     ...), continuing through the runtime support tail.
//   * Object 1 ends immediately after the RET at 0x11301; its declared
//     virtual size is 0x11302 bytes.
//
// The original bytes remain the authority for DOS-era runtime behavior.
// Modern-port equivalents are supplied by platform/runtime adaptation rather
// than treating these compiler/runtime routines as game-authored mechanics.
//
// With this classification, every byte of CODE.1 object 1 from 0x00000 through
// 0x11301 has been converted or explicitly classified.

namespace ravenloft::game {

struct RuntimeTailCBF0 final {
    static constexpr unsigned begin = 0xCBF0;
    static constexpr unsigned endInclusive = 0x11301;
    static constexpr unsigned object1Size = 0x11302;
};

} // namespace ravenloft::game
