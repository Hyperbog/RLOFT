/*
 * Ravenloft: Strahd's Possession - CODE.1 faithful reconstruction
 *
 * Immutable source:
 *   CODE.1
 *   277,787 bytes
 *   SHA-256 fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
 *
 * This file is intentionally a reconstruction, not a modernization.  Every
 * translated region must remain traceable to original CODE.1 bytes.  Unknown
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
// 0x00000000 begins the outer MZ executable.
// 0x0000F444 begins the structured BW image named 4GWPRO.EXP.  This belongs
// to the bound DOS/4G environment and is not being mistaken for game code.
//
// At 0x00025D24 another MZ image begins.  Its MZ size fields identify a
// 7,864-byte DOS stub.  Immediately after that stub, at 0x00027BDC, is the
// LE (Linear Executable) header for the 32-bit protected-mode application.
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
// These fields are decoded from the LE header at file offset 0x27BDC.
// The executable declares an Intel 80386 target and DOS operating system,
// contains 24 pages, and has two load objects.
constexpr u16 kLeCpuType       = 0x0002u; // 80386 in the LE specification.
constexpr u16 kLeOsType        = 0x0001u; // OS/2 field value used for DOS target.
constexpr u32 kLeModuleFlags   = 0x00000200u;
constexpr u32 kLeModulePages   = 24u;
constexpr u32 kLePageSize      = 0x00001000u;
constexpr u32 kLeObjectCount   = 2u;

// LE entry state.  Object numbering in the file is one-based.
constexpr u32 kEntryObject     = 1u;
constexpr u32 kEntryOffset     = 0x0000CC1Cu;
constexpr u32 kStackObject     = 2u;
constexpr u32 kInitialEsp      = 0x00009E90u;
constexpr u32 kAutoDataObject  = 2u;

// Object 1: executable/readable code object.  It begins at logical address
// zero, has virtual size 0x11302, and occupies LE pages 1 through 18.
constexpr u32 kCodeObjectVirtualSize = 0x00011302u;
constexpr u32 kCodeObjectBase        = 0x00010000u;
constexpr u32 kCodeObjectFlags       = 0x00002005u;
constexpr u32 kCodeObjectFirstPage   = 1u;
constexpr u32 kCodeObjectPageCount   = 18u;

// Object 2: writable data/stack object.  It occupies pages 19 through 24.
constexpr u32 kDataObjectVirtualSize = 0x00009E90u;
constexpr u32 kDataObjectBase        = 0x00030000u;
constexpr u32 kDataObjectFlags       = 0x00002003u;
constexpr u32 kDataObjectFirstPage   = 19u;
constexpr u32 kDataObjectPageCount   = 6u;

// The LE data-page offset is 0x6A00 relative to the embedded application MZ,
// giving absolute CODE.1 file offset 0x2C724.  With 24 pages and the final
// partial-page byte count 0x5F7, the page data ends exactly at EOF.  This is
// an independent consistency check on the recovered application-image base.
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
// Object 1 uses the first LE data page, so a logical offset in object 1 maps
// directly to kApplicationDataFileOffset + logical_offset.  This mapping is
// valid only for offsets inside the recovered object-1 virtual extent.
[[nodiscard]] constexpr bool is_code_object_offset(u32 offset) noexcept {
    return offset < kCodeObjectVirtualSize;
}

[[nodiscard]] constexpr u32 code_offset_to_file_offset(u32 offset) noexcept {
    return kApplicationDataFileOffset + offset;
}

static_assert(code_offset_to_file_offset(kEntryOffset) == 0x00039340u);

// The first instruction at logical 0000:CC1C is EB 78, a short jump to CC96.
// Bytes CC1E..CC95 are therefore not executed by normal entry flow.  They
// contain the embedded Watcom run-time identification text and adjacent data.
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
[[nodiscard]] constexpr u32 add32(u32 lhs, u32 rhs) noexcept {
    return lhs + rhs;
}

[[nodiscard]] constexpr u32 sub32(u32 lhs, u32 rhs) noexcept {
    return lhs - rhs;
}

struct ConditionFlags {
    bool carry = false;
    bool zero = false;
    bool sign = false;
    bool overflow = false;
};

[[nodiscard]] constexpr ConditionFlags add32_flags(u32 lhs, u32 rhs) noexcept {
    const u32 result = add32(lhs, rhs);
    return {
        result < lhs,
        result == 0u,
        (result & 0x80000000u) != 0u,
        ((~(lhs ^ rhs) & (lhs ^ result)) & 0x80000000u) != 0u
    };
}

[[nodiscard]] constexpr ConditionFlags sub32_flags(u32 lhs, u32 rhs) noexcept {
    const u32 result = sub32(lhs, rhs);
    return {
        lhs < rhs,
        result == 0u,
        (result & 0x80000000u) != 0u,
        (((lhs ^ rhs) & (lhs ^ result)) & 0x80000000u) != 0u
    };
}

// --------------------------------------------------------------------------
// First recovered control-flow boundary
// --------------------------------------------------------------------------
// Entry execution is now proven to begin at object-1 offset CC1C/file 39340:
//
//   CC1C  EB 78    jmp CC96
//
// CC96 begins the Watcom run-time startup sequence.  That sequence aligns ESP,
// records initial run-time state, performs DOS INT 21h environment/version and
// memory-management calls, copies command/environment material, initializes
// run-time subsystems, and ultimately terminates through INT 21h/AH=4Ch.
//
// It is deliberately NOT translated into a high-level function in this commit.
// The exact INT 21h side effects, segment-register state, and Watcom start-up
// calling contract are being accounted for instruction-by-instruction first.
// Promoting this block prematurely would conceal machine state and violate the
// project's one-to-one requirement.

} // namespace code1
