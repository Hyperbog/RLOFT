/*
 * Ravenloft: Strahd's Possession - CODE.1 faithful reconstruction
 *
 * Immutable source:
 *   CODE.1
 *   277,787 bytes
 *   SHA-256 fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
 *
 * This file is intentionally a reconstruction, not a modernization. Every
 * translated region must remain traceable to original CODE.1 bytes. Unknown
 * semantics remain unknown rather than being replaced with guesses.
 */

#include <cstddef>
#include <cstdint>
#include <limits>

namespace code1 {

using u8  = std::uint8_t;
using s8  = std::int8_t;
using u16 = std::uint16_t;
using s16 = std::int16_t;
using u32 = std::uint32_t;
using s32 = std::int32_t;

static_assert(sizeof(u8) == 1);
static_assert(sizeof(u16) == 2);
static_assert(sizeof(u32) == 4);
static_assert(std::numeric_limits<u8>::digits == 8);
static_assert(std::numeric_limits<u16>::digits == 16);
static_assert(std::numeric_limits<u32>::digits == 32);

// --------------------------------------------------------------------------
// Exact source identity
// --------------------------------------------------------------------------
constexpr std::size_t kSourceFileSize = 277787u;
constexpr char kSourceSha256[] =
    "fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed";

// --------------------------------------------------------------------------
// Bound executable layout - verified directly from CODE.1
// --------------------------------------------------------------------------
constexpr u32 kOuterMzFileOffset          = 0x00000000u;
constexpr u32 kBoundBwFileOffset          = 0x0000F444u;
constexpr u32 kApplicationMzFileOffset    = 0x00025D24u;
constexpr u32 kApplicationLeFileOffset    = 0x00027BDCu;
constexpr u32 kApplicationDataFileOffset  = 0x0002C724u;

constexpr u8 kMzSignature0 = 0x4Du; // 'M'
constexpr u8 kMzSignature1 = 0x5Au; // 'Z'
constexpr u8 kBwSignature0 = 0x42u; // 'B'
constexpr u8 kBwSignature1 = 0x57u; // 'W'
constexpr u8 kLeSignature0 = 0x4Cu; // 'L'
constexpr u8 kLeSignature1 = 0x45u; // 'E'

// --------------------------------------------------------------------------
// Verified LE header values
// --------------------------------------------------------------------------
constexpr u16 kLeCpuType       = 0x0002u;
constexpr u16 kLeOsType        = 0x0001u;
constexpr u32 kLeModuleFlags   = 0x00000200u;
constexpr u32 kLeModulePages   = 24u;
constexpr u32 kLePageSize      = 0x00001000u;
constexpr u32 kLeObjectCount   = 2u;

constexpr u32 kEntryObject     = 1u;
constexpr u32 kEntryOffset     = 0x0000CC1Cu;
constexpr u32 kStackObject     = 2u;
constexpr u32 kInitialEsp      = 0x00009E90u;
constexpr u32 kAutoDataObject  = 2u;

// Object 1 is executable/readable and occupies LE pages 1 through 18.
constexpr u32 kCodeObjectVirtualSize = 0x00011302u;
constexpr u32 kCodeObjectBase        = 0x00010000u;
constexpr u32 kCodeObjectFlags       = 0x00002005u;
constexpr u32 kCodeObjectFirstPage   = 1u;
constexpr u32 kCodeObjectPageCount   = 18u;

// Object 2 is writable data/stack and occupies LE pages 19 through 24.
constexpr u32 kDataObjectVirtualSize = 0x00009E90u;
constexpr u32 kDataObjectBase        = 0x00030000u;
constexpr u32 kDataObjectFlags       = 0x00002003u;
constexpr u32 kDataObjectFirstPage   = 19u;
constexpr u32 kDataObjectPageCount   = 6u;

constexpr u32 kLeDataPagesRelativeToAppMz = 0x00006A00u;
constexpr u32 kFinalPageBytes              = 0x000005F7u;
constexpr u32 kApplicationPageDataBytes =
    (kLeModulePages - 1u) * kLePageSize + kFinalPageBytes;
static_assert(kApplicationMzFileOffset + kLeDataPagesRelativeToAppMz ==
              kApplicationDataFileOffset);
static_assert(kApplicationDataFileOffset + kApplicationPageDataBytes ==
              kSourceFileSize);

// --------------------------------------------------------------------------
// File <-> code-object address mapping
// --------------------------------------------------------------------------
[[nodiscard]] constexpr bool is_code_object_offset(u32 offset) noexcept {
    return offset < kCodeObjectVirtualSize;
}

[[nodiscard]] constexpr u32 code_offset_to_file_offset(u32 offset) noexcept {
    return kApplicationDataFileOffset + offset;
}

static_assert(code_offset_to_file_offset(kEntryOffset) == 0x00039340u);

constexpr u32 kEntryFileOffset             = 0x00039340u;
constexpr u32 kEntryJumpTarget             = 0x0000CC96u;
constexpr u32 kWatcomRuntimeTextFileOffset = 0x00039342u;
constexpr u32 kRavenloftTextFileOffset     = 0x0003E7ACu;

// --------------------------------------------------------------------------
// Explicit little-endian memory operations
// --------------------------------------------------------------------------
[[nodiscard]] constexpr u16 load_le16(const u8* p) noexcept {
    return static_cast<u16>(static_cast<u16>(p[0]) |
                            (static_cast<u16>(p[1]) << 8u));
}

[[nodiscard]] constexpr u32 load_le32(const u8* p) noexcept {
    return static_cast<u32>(static_cast<u32>(p[0]) |
                            (static_cast<u32>(p[1]) << 8u) |
                            (static_cast<u32>(p[2]) << 16u) |
                            (static_cast<u32>(p[3]) << 24u));
}

constexpr void store_le16(u8* p, u16 value) noexcept {
    p[0] = static_cast<u8>(value);
    p[1] = static_cast<u8>(value >> 8u);
}

constexpr void store_le32(u8* p, u32 value) noexcept {
    p[0] = static_cast<u8>(value);
    p[1] = static_cast<u8>(value >> 8u);
    p[2] = static_cast<u8>(value >> 16u);
    p[3] = static_cast<u8>(value >> 24u);
}

// --------------------------------------------------------------------------
// x86-width arithmetic and tracked condition flags
// --------------------------------------------------------------------------
[[nodiscard]] constexpr u32 add32(u32 lhs, u32 rhs) noexcept { return lhs + rhs; }
[[nodiscard]] constexpr u32 sub32(u32 lhs, u32 rhs) noexcept { return lhs - rhs; }

struct ConditionFlags {
    bool carry = false;
    bool zero = false;
    bool sign = false;
    bool overflow = false;
};

[[nodiscard]] constexpr ConditionFlags add32_flags(u32 lhs, u32 rhs) noexcept {
    const u32 result = add32(lhs, rhs);
    return {result < lhs, result == 0u, (result & 0x80000000u) != 0u,
            ((~(lhs ^ rhs) & (lhs ^ result)) & 0x80000000u) != 0u};
}

[[nodiscard]] constexpr ConditionFlags sub32_flags(u32 lhs, u32 rhs) noexcept {
    const u32 result = sub32(lhs, rhs);
    return {lhs < rhs, result == 0u, (result & 0x80000000u) != 0u,
            (((lhs ^ rhs) & (lhs ^ result)) & 0x80000000u) != 0u};
}

// --------------------------------------------------------------------------
// Minimal machine interface used by instruction-accountable translations
// --------------------------------------------------------------------------
// This is deliberately not an emulator. It exposes only state touched by a
// translated block. DOS services remain an injected boundary because replacing
// INT 21h with guessed host behavior would no longer be a faithful conversion.
struct CpuState {
    u32 eax = 0;
    u32 ebx = 0;
    u32 ecx = 0;
    u32 edx = 0;
    u32 esi = 0;
    u32 edi = 0;
    u32 ebp = 0;
    u32 esp = 0;
    u16 ds = 0;
    u16 es = 0;
    bool interrupt_enable = false;
};

struct Machine {
    CpuState cpu{};
    u8* data = nullptr; // logical object-2/DGROUP byte 0 is data[0]
    void (*int21)(Machine&) = nullptr;
};

[[nodiscard]] constexpr u8 low8(u32 value) noexcept {
    return static_cast<u8>(value);
}
[[nodiscard]] constexpr u8 high8(u32 value) noexcept {
    return static_cast<u8>(value >> 8u);
}
[[nodiscard]] constexpr u16 low16(u32 value) noexcept {
    return static_cast<u16>(value);
}
constexpr void set_low16(u32& value, u16 low) noexcept {
    value = (value & 0xFFFF0000u) | static_cast<u32>(low);
}
constexpr void set_high8(u32& value, u8 high) noexcept {
    value = (value & 0xFFFF00FFu) | (static_cast<u32>(high) << 8u);
}

// --------------------------------------------------------------------------
// Verified translation 1: protected-mode entry and first startup decision
// --------------------------------------------------------------------------
// Original object-1/file mapping:
//   CC1C / 39340: EB 78                    jmp CC96
//   CC1E..CC95:                            embedded runtime text/data
//   CC96 / 393BA: FB                       sti
//   CC97 / 393BB: 83 E4 FC                 and esp,-4
//   CC9A / 393BE: 8B DC                    mov ebx,esp
//   CC9C / 393C0: 89 1D 04 53 00 00        mov [5304],ebx
//   CCA2 / 393C6: 89 1D F0 52 00 00        mov [52F0],ebx
//   CCA8 / 393CC: 66 B8 24 00              mov ax,24h
//   CCAC / 393D0: 66 A3 FC 52 00 00        mov [52FC],ax
//   CCB2 / 393D6: BB 52 41 48 50           mov ebx,50484152h
//   CCB7 / 393DB: 2B C0                    sub eax,eax
//   CCB9 / 393DD: B4 30                    mov ah,30h
//   CCBB / 393DF: CD 21                    int 21h
//   CCBD / 393E1: A2 26 53 00 00           mov [5326],al
//   CCC2 / 393E6: 88 25 27 53 00 00        mov [5327],ah
//   CCC8 / 393EC: 8B C8                    mov ecx,eax
//   CCCA / 393EE: 2B F6                    sub esi,esi
//   CCCC / 393F0: BF 81 00 00 00           mov edi,81h
//   CCD1 / 393F5: C1 E8 10                 shr eax,10h
//   CCD4 / 393F8: 66 3D 58 44              cmp ax,4458h
//   CCD8 / 393FC: 0F 85 39 00 00 00        jne CD17
//
// The bytes skipped by EB 78 are intentionally not translated as instructions.
// The first bytes are the literal "WATCOM C/C++32 Run-Time system..." string.
// Treating them as x86 would create false code.
enum class StartupPath : u32 {
    FallThroughCcde = 0x0000CCDEu,
    BranchCd17      = 0x0000CD17u,
};

[[nodiscard]] StartupPath translate_cc1c_ccdd(Machine& m) noexcept {
    // CC1C: JMP CC96. No architectural state changes other than EIP; control
    // therefore enters this C++ block directly at the translated CC96 state.

    // CC96: STI
    m.cpu.interrupt_enable = true;

    // CC97: AND ESP,FFFFFFFCh
    m.cpu.esp &= 0xFFFFFFFCu;

    // CC9A..CCA2: preserve aligned startup stack in EBX and two DGROUP slots.
    m.cpu.ebx = m.cpu.esp;
    store_le32(m.data + 0x5304u, m.cpu.ebx);
    store_le32(m.data + 0x52F0u, m.cpu.ebx);

    // CCA8..CCAC: AX=24h, then save AX at DGROUP:52FC.
    set_low16(m.cpu.eax, 0x0024u);
    store_le16(m.data + 0x52FCu, low16(m.cpu.eax));

    // CCB2: EBX receives the literal 50484152h (ASCII bytes "RAHP" in the
    // register's numeric representation). No semantic name is assigned.
    m.cpu.ebx = 0x50484152u;

    // CCB7..CCBB: EAX=0; AH=30h; INT 21h. DOS function 30h obtains the DOS
    // version. The interrupt boundary is injected so exact DOS register side
    // effects are preserved by the caller rather than approximated here.
    m.cpu.eax = 0u;
    set_high8(m.cpu.eax, 0x30u);
    if (m.int21 != nullptr) {
        m.int21(m);
    }

    // CCBD..CCC8: save returned AL/AH and preserve full returned EAX in ECX.
    m.data[0x5326u] = low8(m.cpu.eax);
    m.data[0x5327u] = high8(m.cpu.eax);
    m.cpu.ecx = m.cpu.eax;

    // CCCA..CCCC: zero ESI and set EDI=81h.
    m.cpu.esi = 0u;
    m.cpu.edi = 0x00000081u;

    // CCD1..CCD8: inspect the original high word of returned EAX. The compare
    // is exactly AX==4458h after SHR EAX,16. Non-equality branches to CD17.
    m.cpu.eax >>= 16u;
    return low16(m.cpu.eax) != 0x4458u
        ? StartupPath::BranchCd17
        : StartupPath::FallThroughCcde;
}

// 74 original bytes are now instruction-accounted: the 2-byte entry jump plus
// 72 bytes at CC96..CCDD. The intervening 120 bytes are verified data and are
// excluded rather than falsely counted as translated instructions.
constexpr u32 kVerifiedTranslatedBytes = 74u;

} // namespace code1
