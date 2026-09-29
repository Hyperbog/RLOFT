#include "Code1Interrupt6104.h"

namespace ravenloft::game {

void Code1Interrupt6104::run(
    const Code1InterruptFrame6104& frame,
    Code1InterruptState6104& state,
    const Code1InterruptOps6104& ops) {
    // 0x610D..0x6114: establish the CODE.1 data context; ES then mirrors DS.
    ops.callCEC0();

    // 0x6115..0x611C: reject nested entry.
    if (state.active1368 != 0) {
        return;
    }

    state.active1368 = 1;

    // 0x6125..0x613D: save the interrupted stack before switching to the
    // original private interrupt stack at offset 0x23BF.
    state.savedSs23C3 = frame.ss;
    state.savedEsp23C5 = frame.esp;

    // 0x6147..0x6150: real-mode segment -> linear base.
    const std::uint32_t linearEsi =
        (frame.esi & 0xFFFFu) << 4u;

    const auto* sample =
        ops.resolveLinearPointer(linearEsi);

    // 0x6155..0x6162.
    Code1VgaSave6060::run(state.vga, ops.vga);

    Code1InterruptUpdate50C0::run(
        static_cast<std::int32_t>(frame.eax),
        static_cast<std::int32_t>(frame.ebx),
        static_cast<std::int32_t>(frame.ecx),
        static_cast<std::int32_t>(frame.edx),
        sample,
        state.update,
        ops.update);

    Code1VgaRestore60B4::run(state.vga, ops.vga);

    // 0x6167..0x6173 restores the interrupted SS:ESP and clears the reentry
    // guard. The C++ call frame itself already preserves the caller's CPU
    // registers/flags represented by Code1InterruptFrame6104.
    state.active1368 = 0;
}

} // namespace ravenloft::game
