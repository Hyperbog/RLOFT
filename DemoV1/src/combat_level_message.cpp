#include "rloft/combat_level_message.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kDataObjectStart=0x75F24U;
constexpr std::size_t kPointerLocation=kDataObjectStart+0x67E4U;
constexpr std::size_t kFormatLocation=kDataObjectStart+0x548U;
constexpr std::size_t kFormatSize=18U;
constexpr std::size_t kActorNameCapacity=16U;

// CODE.2 data 548, pointed at by DWORD 67E4, is this sole DDA4
// level-up format. Other invocations of 3210F/385B0 may differ.
constexpr std::array<std::uint8_t,kFormatSize> kExpectedFormat{{
    '%','s',' ','g','a','i','n','s',' ','a',' ','l','e','v','e','l','.','\0'
}};
} // namespace

std::optional<CombatLevelMessageProjection>
CombatLevelMessageModel::inspect(
    const PartyImage& observed_party,
    std::size_t actor_index,
    const std::vector<std::uint8_t>& original_code2,
    const std::vector<std::uint8_t>& destination_759c) {
    if(actor_index>=PartyImage::kActorCount ||
       original_code2.size()<kFormatLocation+kFormatSize ||
       original_code2.size()<kPointerLocation+4U)
        return std::nullopt;
    // Serialized pointer is a 32-bit DGROUP offset, 0x548, read by
    // DDA4:DF58. The original file may not be host-pointer aligned.
    const auto ptr=static_cast<std::uint32_t>(
        static_cast<std::uint32_t>(original_code2[kPointerLocation]) |
        (static_cast<std::uint32_t>(
            original_code2[kPointerLocation+1U])<<8U) |
        (static_cast<std::uint32_t>(
            original_code2[kPointerLocation+2U])<<16U) |
        (static_cast<std::uint32_t>(
            original_code2[kPointerLocation+3U])<<24U));
    if(ptr!=0x548U) return std::nullopt;
    for(std::size_t i=0U;i<kFormatSize;++i)
        if(original_code2[kFormatLocation+i]!=kExpectedFormat[i])
            return std::nullopt;

    const auto name_at=actor_index*PartyImage::kActorSize;
    std::size_t name_length=0U;
    while(name_length<kActorNameCapacity &&
          observed_party.bytes[name_at+name_length]!=0U)
        ++name_length;
    // The original %s formatter walks until NUL, potentially past
    // actor's 16 name bytes. That other-memory read is not modeled.
    if(name_length==kActorNameCapacity) return std::nullopt;

    const auto generated_length=name_length+(kFormatSize-3U);
    // kExpectedFormat: two bytes "%s", 15 trailing content bytes,
    // and one final NUL. 3210F writes this NUL after 385B0 returns.
    if(generated_length>=destination_759c.size()) return std::nullopt;
    CombatLevelMessageProjection out{};
    out.formatted_byte_count=generated_length;
    out.projected_destination=destination_759c;
    out.formatted_c_string.reserve(generated_length+1U);
    for(std::size_t i=0U;i<name_length;++i)
        out.formatted_c_string.push_back(
            observed_party.bytes[name_at+i]);
    for(std::size_t i=2U;i<kFormatSize;++i)
        out.formatted_c_string.push_back(kExpectedFormat[i]);
    // Last byte copied from kExpectedFormat is the original NUL.
    for(std::size_t i=0U;i<out.formatted_c_string.size();++i)
        out.projected_destination[i]=out.formatted_c_string[i];
    return out;
}

} // namespace rloft
