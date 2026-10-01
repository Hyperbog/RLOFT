/*
 * Ravenloft: Strahd's Possession - CODE.1 reconstruction
 *
 * Source artifact:
 *   CODE.1
 *   277,787 bytes
 *   SHA-256 fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
 *
 * PURPOSE
 * -------
 * This translation unit is the evidence-first C++ reconstruction of CODE.1.
 * It is intentionally NOT a modernization.  Original machine-code behavior
 * takes precedence over readability, contemporary idioms, or inferred design.
 *
 * ACCOUNTABILITY RULE
 * -------------------
 * No game routine is represented as reconstructed until its original byte
 * range, instruction decoding, control flow, and externally visible effects
 * have been established.  Unknowns stay unknown rather than being filled by
 * assumptions.  Original file/runtime addresses will accompany reconstructed
 * blocks as the executable mapping is proven.
 *
 * CURRENT STATE
 * -------------
 * The protected-mode application image is still being mapped.  Consequently
 * this file currently records only verified binary/toolchain facts and the
 * low-level representation types that later translated blocks will require.
 * No application instructions are claimed converted yet; PROGRESS.md remains
 * at 0% until the first complete application region passes verification.
 */

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace code1 {

// --------------------------------------------------------------------------
// Fixed-width machine types
// --------------------------------------------------------------------------
// CODE.1 contains 16-bit real-mode/extender material and 32-bit protected-mode
// material.  Fixed-width types are mandatory here: host C++ `int`/`long`
// widths must never silently redefine an original x86 operation.
using u8  = std::uint8_t;
using s8  = std::int8_t;
using u16 = std::uint16_t;
using s16 = std::int16_t;
using u32 = std::uint32_t;
using s32 = std::int32_t;

static_assert(sizeof(u8)  == 1);
static_assert(sizeof(u16) == 2);
static_assert(sizeof(u32) == 4);
static_assert(std::numeric_limits<u8>::digits  == 8);
static_assert(std::numeric_limits<u16>::digits == 16);
static_assert(std::numeric_limits<u32>::digits == 32);

// --------------------------------------------------------------------------
// Immutable source identity
// --------------------------------------------------------------------------
// These values identify the exact artifact against which every reconstruction
// decision is checked.  They are metadata, not values recovered from program
// variables in the original executable.
constexpr std::size_t kSourceFileSize = 277787u;
constexpr char kSourceSha256[] =
    "fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed";

// --------------------------------------------------------------------------
// Verified file offsets discovered during initial executable mapping
// --------------------------------------------------------------------------
// IMPORTANT: these are FILE offsets only.  They must not be treated as runtime
// linear addresses until the DOS/4G/DOS16M load mapping has been established.
constexpr u32 kDosMzHeaderFileOffset       = 0x00000000u;
constexpr u32 kBoundBwHeaderFileOffset     = 0x0000F444u;
constexpr u32 kWatcomRuntimeTextFileOffset = 0x00039342u;
constexpr u32 kRavenloftTextFileOffset     = 0x0003E7ACu;

// The first two bytes are verified as the conventional DOS MZ signature.
constexpr u8 kMzSignature0 = 0x4Du; // 'M'
constexpr u8 kMzSignature1 = 0x5Au; // 'Z'

// A BW signature occurs at 0xF444 in a structured region adjacent to the
// embedded name "4GWPRO.EXP".  Its complete field interpretation is withheld
// here until each header field is mapped against the loader format.
constexpr u8 kBwSignature0 = 0x42u; // 'B'
constexpr u8 kBwSignature1 = 0x57u; // 'W'

// --------------------------------------------------------------------------
// x86 arithmetic helpers
// --------------------------------------------------------------------------
// Later reconstruction must preserve modulo-2^N integer behavior where the
// original instruction does so.  Unsigned fixed-width C++ arithmetic gives us
// a defined representation for that wrapping behavior without depending on
// host signed-overflow rules.
[[nodiscard]] constexpr u8 wrap8(u32 value) noexcept {
    return static_cast<u8>(value);
}

[[nodiscard]] constexpr u16 wrap16(u32 value) noexcept {
    return static_cast<u16>(value);
}

[[nodiscard]] constexpr u32 add32(u32 lhs, u32 rhs) noexcept {
    return lhs + rhs;
}

[[nodiscard]] constexpr u32 sub32(u32 lhs, u32 rhs) noexcept {
    return lhs - rhs;
}

// Rotate helpers model the value transformation only.  They do NOT claim to
// reproduce EFLAGS.  Any translated instruction sequence whose subsequent
// behavior depends on flags must model the required flags explicitly.
[[nodiscard]] constexpr u32 rol32(u32 value, unsigned count) noexcept {
    count &= 31u;
    if (count == 0u) {
        return value;
    }
    return static_cast<u32>((value << count) | (value >> (32u - count)));
}

[[nodiscard]] constexpr u32 ror32(u32 value, unsigned count) noexcept {
    count &= 31u;
    if (count == 0u) {
        return value;
    }
    return static_cast<u32>((value >> count) | (value << (32u - count)));
}

// --------------------------------------------------------------------------
// Explicit little-endian memory access
// --------------------------------------------------------------------------
// x86 stores multi-byte scalar values little-endian.  Directly casting a byte
// pointer to u16/u32 would introduce host alignment and aliasing assumptions,
// so reconstruction code uses byte-wise helpers when operating on emulated or
// image-backed memory.
[[nodiscard]] constexpr u16 load_le16(const u8* p) noexcept {
    return static_cast<u16>(
        static_cast<u16>(p[0]) |
        static_cast<u16>(static_cast<u16>(p[1]) << 8u));
}

[[nodiscard]] constexpr u32 load_le32(const u8* p) noexcept {
    return static_cast<u32>(
        static_cast<u32>(p[0]) |
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
// Minimal x86 condition state
// --------------------------------------------------------------------------
// This is deliberately not yet a complete CPU emulator.  It is a place to
// represent only flags that a verified translated block actually consumes.
// Fields will be added only when required by original instructions.
struct ConditionFlags {
    bool carry = false;
    bool zero = false;
    bool sign = false;
    bool overflow = false;
};

// Compute the subset of ADD flags above for a 32-bit operation.  Keeping flag
// derivation explicit prevents a later C++ expression from accidentally losing
// a branch condition that existed in the original x86 sequence.
[[nodiscard]] constexpr ConditionFlags add32_flags(u32 lhs, u32 rhs) noexcept {
    const u32 result = add32(lhs, rhs);

    ConditionFlags flags{};
    flags.carry = result < lhs;
    flags.zero = (result == 0u);
    flags.sign = (result & 0x80000000u) != 0u;
    flags.overflow = ((~(lhs ^ rhs) & (lhs ^ result)) & 0x80000000u) != 0u;
    return flags;
}

// Compute the same tracked subset for 32-bit SUB/CMP semantics.
[[nodiscard]] constexpr ConditionFlags sub32_flags(u32 lhs, u32 rhs) noexcept {
    const u32 result = sub32(lhs, rhs);

    ConditionFlags flags{};
    flags.carry = lhs < rhs; // x86 borrow indication for SUB/CMP.
    flags.zero = (result == 0u);
    flags.sign = (result & 0x80000000u) != 0u;
    flags.overflow = (((lhs ^ rhs) & (lhs ^ result)) & 0x80000000u) != 0u;
    return flags;
}

// --------------------------------------------------------------------------
// Reconstruction boundary
// --------------------------------------------------------------------------
// No application function is declared below this point yet.  The next phase
// must first establish the protected-mode image's segment/object mapping and
// entry point.  Adding guessed function boundaries here would violate the
// one-to-one, evidence-first requirement of this project.

} // namespace code1
