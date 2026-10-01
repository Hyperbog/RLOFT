#include <cstdint>
#include <functional>

namespace ravenloft::code1 {

// Sole authority: original CODE.1 binary, SHA-256
// fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
//
// Direct byte search and contiguous verification against the original file
// establish CODE.1 virtual address 0x0000 at original file offset 0x2C724.
// The bytes from that file offset through virtual 0x11301 form the contiguous
// CODE.1 address image used by the existing address-labelled reconstruction.
// No generated disassembly is an input to this mapping.

struct ObjectStartOps0000 {
    // 0x0000 INT3.  Keeping the trap as an injected boundary preserves the
    // original architectural event without substituting a platform-specific
    // modern debugger primitive into the faithful core.
    std::function<void()> int3;
};

// 0x0000..0x0002
// 0000 CC       int3
// 0001 EB FD    jmp 0x0000
//
// If the INT3 handler returns, execution jumps back to the INT3 forever.
[[noreturn]] void objectStart0000(const ObjectStartOps0000& ops) {
    for (;;) {
        ops.int3();
    }
}

// 0x0003..0x000F are thirteen one-byte NOPs in the original byte stream:
// 0003 90             nop
// 0004 90             nop
// ...
// 000F 90             nop
// They have no externally observable effect and lead directly to 0x0010.
//
// 0x0010..0x0048 is reconstructed in Dice0010.cpp.
// 0x0049..0x004B is LEA EAX,[EAX+0], alignment with no observable effect.
// 0x004C..0x00EA is reconstructed in TextRecord004C.cpp.
// 0x00EB is NOP alignment.

} // namespace ravenloft::code1
