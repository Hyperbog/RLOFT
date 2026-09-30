#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::code1 {

// Sole authority: original CODE.1 binary, SHA-256
// fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
//
// The virtual-address-zero boundary used by the existing CODE1 mapping occurs
// at original CODE.1 file offset 0x2E5DC.  Direct inspection of the original
// bytes proves that this boundary cuts through a live routine: that routine
// starts 0x54 bytes earlier at file offset 0x2E588 and returns at virtual
// 0x0028.  The reconstruction therefore includes the complete crossing
// routine rather than inventing an entry point at 0x0000.

struct OpeningState {
    std::int32_t currentCharacter4CC = 0;
    // Stored as raw dword bits so INC/DEC are defined modulo 2^32 exactly as
    // the original instructions, with no signed-overflow undefined behavior.
    std::uint32_t nesting4D0 = 0;
    std::array<std::uint8_t, 0x248 * 2> characterRecords{};
};

struct OpeningOps {
    // Calls are named by their original object-relative targets.  Their own
    // implementations are reconstructed when their byte ranges are reached.
    std::function<void(const std::array<std::int32_t, 13>&)> callF5AC;
    std::function<std::uint32_t()> callE584;
    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> callF6F8;
};

// Original CODE.1 file 0x2E588..0x2E604, mapped -0x54..0x0028.
// -054 push ebp                 -053 mov ebp,esp
// -051 push ebx
// -050..-036 push 27,-1,9,-1,-1,-1,5,4,3,2,1,0,1
// -031 call -0xA54             -02C add esp,34
// -029 call -0xA7C             -024 mov edx,eax
// -022 test eax,eax            -020 je 0004
// -01E mov bl,al               -01C dec bl
// -01A imul eax,[04CC],248
// -010 mov [eax+680C],bl
// -00A push 2                  -008 push 38
// -006 push 114                -001 inc dword [04D0]
// 0005 dec edx                 0006 push edx
// 0007 jmp 0017
// 0009 push -1 x4              0011 dec dword [04D0]
// 0017 call -0908              001C add esp,10
// 001F mov eax,1               0024 test eax,eax
// 0026 je -002E                0028 epilogue/ret
//
// The branch at 0x0026 is provably dead in this routine: EAX is assigned 1 at
// 0x001F immediately before TEST.  This is the literal control-flow result of
// the original instruction stream, not an inferred replacement behavior.
void openingRoutineFFAC(OpeningState& state, const OpeningOps& ops) {
    // PUSH order in the binary is reversed here into cdecl argument order.
    ops.callF5AC({
        1, 0, 1, 2, 3, 4, 5, -1, -1, -1, 9, -1, 0x27
    });

    const std::uint32_t selection = ops.callE584();

    if (selection != 0u) {
        // MOV BL,AL / DEC BL: only AL is copied and the decrement wraps at 8 bits.
        const std::uint8_t bl = static_cast<std::uint8_t>(
            static_cast<std::uint8_t>(selection) - std::uint8_t{1});

        // IMUL EAX,[04CC],0x248 retains the low 32-bit product.  In the valid
        // game state 04CC selects one of the two 0x248-byte records beginning
        // at DS:67F8; DS:680C is byte +0x14 within that record.
        const std::uint32_t offset =
            static_cast<std::uint32_t>(state.currentCharacter4CC) * 0x248u;
        state.characterRecords[static_cast<std::size_t>(offset + 0x14u)] = bl;

        // INC dword [04D0], exactly modulo 2^32.
        state.nesting4D0 += 1u;

        // EDX still holds the full EAX returned by -0xA7C. DEC is 32-bit.
        const std::uint32_t edx = selection - 1u;
        ops.callF6F8(static_cast<std::int32_t>(edx), 0x114, 0x38, 2);
    } else {
        // DEC dword [04D0], exactly modulo 2^32.
        state.nesting4D0 -= 1u;
        ops.callF6F8(-1, -1, -1, -1);
    }

    // 0x001F forces EAX=1; 0x0024 TEST therefore cannot take 0x0026 JE.
}

// 0x0029..0x002B: LEA EAX,[EAX+0], three-byte alignment NOP.

// 0x002C..0x0046
// 002C push ebp                 002D mov ebp,esp
// 002F mov eax,[ebp+08]         0032 mov dh,[eax]
// 0034 mov eax,[ebp+0C]         0037 mov dl,[eax]
// 0039 mov eax,[ebp+08]         003C mov [eax],dl
// 003E mov eax,[ebp+0C]         0041 mov [eax],dh
// 0043 mov esp,ebp              0045 pop ebp
// 0046 ret
void swapBytes002C(std::uint8_t* first, std::uint8_t* second) noexcept {
    const std::uint8_t dh = *first;
    const std::uint8_t dl = *second;
    *first = dl;
    *second = dh;
}

// 0x0047: NOP.
//
// No classification is claimed yet for 0x0048 onward.  Earlier generated
// analysis called 0x0048..0x006B a pointer table, but this restart does not
// inherit that conclusion.  It will be established again from CODE.1 itself.

} // namespace ravenloft::code1
