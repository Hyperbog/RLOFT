#include <array>
#include <cstdint>

namespace ravenloft::code1 {

// Original authority: CODE.1 SHA-256
// fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
//
// CODE.1 contains a bound LE executable.  Its first executable LE object has
// virtual size 0x11302 and is the address space used by the offsets below.
// This source is translated from that object as extracted directly from the
// original CODE.1 pages; no generated ASM is an input to this reconstruction.

// 0x002C..0x0046
// 002C  push ebp
// 002D  mov  ebp,esp
// 002F  mov  eax,[ebp+08]
// 0032  mov  dh,[eax]
// 0034  mov  eax,[ebp+0C]
// 0037  mov  dl,[eax]
// 0039  mov  eax,[ebp+08]
// 003C  mov  [eax],dl
// 003E  mov  eax,[ebp+0C]
// 0041  mov  [eax],dh
// 0043  mov  esp,ebp
// 0045  pop  ebp
// 0046  ret
//
// DH and DL are deliberately used as independent byte temporaries.  No flags
// produced in this routine are consumed before RET, so the exact observable
// operation is a byte swap at the two caller-provided addresses.
void swapBytes002C(std::uint8_t* first, std::uint8_t* second) noexcept {
    const std::uint8_t dh = *first;
    const std::uint8_t dl = *second;
    *first = dl;
    *second = dh;
}

// 0x0047 is a one-byte NOP alignment instruction.

// 0x0048..0x006B are data, not instructions.  They are nine little-endian
// object-relative targets.  LE relocation records identify each as an
// internal object-1 pointer.  Preserve all duplicates and ordering exactly.
inline constexpr std::array<std::uint32_t, 9> kTargets0048{
    0x000020DDu,
    0x000020DDu,
    0x000020DDu,
    0x0000210Fu,
    0x0000210Fu,
    0x0000210Fu,
    0x0000213Du,
    0x00002161u,
    0x0000217Cu,
};

} // namespace ravenloft::code1
