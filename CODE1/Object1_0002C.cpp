#include <array>
#include <bit>
#include <cstdint>
#include <functional>

namespace ravenloft::code1 {

// Sole authority: original CODE.1 binary, SHA-256
// fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
//
// The virtual-address-zero boundary used by the existing CODE1 mapping occurs
// at original CODE.1 file offset 0x2E5DC. Direct inspection of CODE.1 proves
// that this boundary cuts through a live routine. Its prologue is 0x54 bytes
// earlier at file offset 0x2E588 and its RET is at mapped address 0x0028.

struct OpeningState {
    std::int32_t currentCharacter4CC = 0;
    // Raw dword bits: INC/DEC must wrap modulo 2^32, never invoke signed UB.
    std::uint32_t nesting4D0 = 0;
};

struct OpeningOps {
    // Targets retain their original object-relative identities until those
    // routines themselves are reconstructed from CODE.1.
    std::function<void(const std::array<std::int32_t, 13>&)> callF5AC;
    std::function<std::uint32_t()> callE584;
    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> callF6F8;

    // Original code writes through DS using a computed 32-bit address. Keeping
    // that address explicit preserves even out-of-range/corrupt-state behavior
    // for the eventual faithful memory layer instead of imposing C++ bounds.
    std::function<void(std::uint32_t, std::uint8_t)> writeDsByte;
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
void openingRoutineFFAC(OpeningState& state, const OpeningOps& ops) {
    // Binary PUSH order converted to cdecl argument order.
    ops.callF5AC({1, 0, 1, 2, 3, 4, 5, -1, -1, -1, 9, -1, 0x27});

    const std::uint32_t selection = ops.callE584();

    if (selection != 0u) {
        // MOV BL,AL / DEC BL: partial-register copy followed by 8-bit wrap.
        const std::uint8_t bl = static_cast<std::uint8_t>(
            static_cast<std::uint8_t>(selection) - std::uint8_t{1});

        // IMUL EAX,[04CC],0x248 keeps the low 32 bits; adding 0x680C is also
        // modulo 2^32 because the effective address is formed in 32-bit mode.
        const std::uint32_t recordOffset =
            static_cast<std::uint32_t>(state.currentCharacter4CC) * 0x248u;
        ops.writeDsByte(recordOffset + 0x680Cu, bl);

        state.nesting4D0 += 1u; // INC dword [04D0], modulo 2^32.

        // EDX contains the full return EAX, then DEC EDX. bit_cast passes the
        // exact resulting 32-bit pattern as the signed C++ callback argument.
        const std::uint32_t edxBits = selection - 1u;
        ops.callF6F8(std::bit_cast<std::int32_t>(edxBits), 0x114, 0x38, 2);
    } else {
        state.nesting4D0 -= 1u; // DEC dword [04D0], modulo 2^32.
        ops.callF6F8(-1, -1, -1, -1);
    }

    // 0x001F MOV EAX,1 immediately precedes TEST EAX,EAX. Therefore the JE at
    // 0x0026 is unreachable for every execution of this instruction stream.
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
// No classification is claimed yet for 0x0048 onward. Earlier generated
// analysis is deliberately not inherited by this restart; the next bytes will
// be classified again from original CODE.1 evidence.

} // namespace ravenloft::code1
