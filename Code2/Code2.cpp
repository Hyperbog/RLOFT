// Faithful reconstruction of Ravenloft: Strahd's Possession CODE.2.
//
// The immutable CODE.2 binary is controlling evidence. No translated machine-code
// bytes are claimed in this file yet; this is only the canonical source scaffold
// and verified executable-object identity for the upcoming decompilation.

#include <cstdint>

namespace code2 {

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using s8  = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;

constexpr u32 kObject1FileStart = 0x0003AF24u;
constexpr u32 kObject1VirtualBase = 0x00010000u;
constexpr u32 kObject1Length = 0x0003A852u;
constexpr u32 kObject1EntryOffset = 0x000353A0u;
constexpr u32 kVerifiedTranslatedBytes = 0u;
constexpr u32 kVerifiedNonCodeBytes = 0u;

constexpr u32 kObject2FileStart = 0x00075F24u;
constexpr u32 kObject2VirtualBase = 0x00050000u;
constexpr u32 kObject2VirtualSize = 0x0002DD10u;
constexpr u32 kObject2StoredBytes = 0x0001273Du;

static_assert(kObject1Length == 239698u);
static_assert(kObject2FileStart + kObject2StoredBytes == 0x00088661u);
static_assert(kVerifiedTranslatedBytes + kVerifiedNonCodeBytes <= kObject1Length);

} // namespace code2
