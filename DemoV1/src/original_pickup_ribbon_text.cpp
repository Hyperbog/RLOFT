#include "rloft/original_pickup_ribbon_text.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace rloft {
namespace {

constexpr std::size_t kDataObjectFileBase=0x75F24U;
constexpr std::size_t kDgroupPrefixPointer=0xD67CU;

// Original 22510 uses an unbounded C copy. The source prefix in the
// audited 1.2a CODE.2 fits in 80 bytes; treat any other memory state
// as unresolved, rather than following an unbounded corrupt pointer.
constexpr std::size_t kAuditedPrefixLimit=80U;

std::uint32_t u32(const std::uint8_t* p) noexcept {
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1])<<8U) |
           (static_cast<std::uint32_t>(p[2])<<16U) |
           (static_cast<std::uint32_t>(p[3])<<24U);
}

} // namespace

std::optional<std::vector<std::uint8_t>>
OriginalPickupRibbonTextModel::source_prefix_from_code2(
    const std::vector<std::uint8_t>& code2) {
    constexpr std::size_t pointer_at=
        kDataObjectFileBase+kDgroupPrefixPointer;
    if(code2.size()<pointer_at+4U)
        return std::nullopt;

    const auto offset=static_cast<std::size_t>(
        u32(code2.data()+pointer_at));
    if(offset>=code2.size()-kDataObjectFileBase)
        return std::nullopt;
    const auto start=kDataObjectFileBase+offset;
    std::vector<std::uint8_t> prefix;
    for(std::size_t i=0U;i<kAuditedPrefixLimit &&
        start+i<code2.size();++i) {
        const auto b=code2[start+i];
        if(b==0U) {
            if(prefix.empty()) return std::nullopt;
            return prefix;
        }
        prefix.push_back(b);
    }
    return std::nullopt;
}

std::optional<std::vector<std::uint8_t>>
OriginalPickupRibbonTextModel::compose_unenchanted(
    const std::vector<std::uint8_t>& original_prefix,
    const ObjectDefinition& object,
    bool original_known_bit) {
    // CODE.2 22567/22575 requires known and nonzero +37.
    // For Level22's four relics +37 is zero in OBJECTS.DAT, so
    // the original always takes this exact simple copy path.
    if(original_known_bit && object.enchantment!=0U)
        return std::nullopt;
    if(original_prefix.empty() || object.name.empty())
        return std::nullopt;
    std::vector<std::uint8_t> out=original_prefix;
    for(unsigned char c:object.name)
        out.push_back(static_cast<std::uint8_t>(c));
    out.push_back(0U);
    return out;
}

} // namespace rloft
