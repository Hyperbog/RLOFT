#include "rloft/exploration_preview.hpp"

#include "rloft/exploration_state.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rloft {
namespace {

void word(std::vector<std::uint8_t>& bytes,
          std::size_t position, std::uint16_t value) {
    bytes[position] = static_cast<std::uint8_t>(value);
    bytes[position+1U] =
        static_cast<std::uint8_t>(value >> 8U);
}

void init_actor(std::vector<std::uint8_t>& bytes,
                std::size_t actor, std::string_view name) {
    const auto base = actor * PartyImage::kActorSize;
    // CODE.1 4DC4 initializes unused actor/gear bytes to FF and 4EF0
    // clears the first two accepted actors' runtime tail.
    std::fill_n(bytes.begin() +
        static_cast<std::ptrdiff_t>(base),16U,0U);
    std::copy(name.begin(),name.end(),
        bytes.begin()+static_cast<std::ptrdiff_t>(base));
    std::fill(bytes.begin()+
        static_cast<std::ptrdiff_t>(base+0x10BU),
        bytes.begin()+
        static_cast<std::ptrdiff_t>(
            base+PartyImage::kActorSize),0U);

    word(bytes,base+0x10U,0U); // active
    bytes[base+0x12U]=0U;     // installed RES1 portrait index
    bytes[base+0x13U]=0U;     // male
    bytes[base+0x14U]=0U;     // human
    bytes[base+0x15U]=0U;     // fighter
    bytes[base+0x16U]=0xFFU;
    bytes[base+0x17U]=0xFFU;
    bytes[base+0x1AU]=0U;     // default character option
    bytes[base+0x1BU]=10U;
    word(bytes,base+0x1CU,12U); // current HP
    word(bytes,base+0x1EU,12U); // maximum HP
    word(bytes,base+0x20U,1U);  // first class level
    word(bytes,base+0x22U,0U);
    word(bytes,base+0x24U,0U);
    // No items or borrowed party equipment; FF is the original empty
    // object selector in each of the ten initial equipment slots.
    for(std::size_t i=0;i<10U;++i) {
        word(bytes,base+0x40U+i*2U,0xFFFFU);
    }
    for(std::size_t i=0;i<6U;++i) {
        bytes[base+0x32U+2U*i]=10U;
        bytes[base+0x33U+2U*i]=10U;
    }
}

} // namespace

std::vector<std::uint8_t> ExplorationPreview::synthetic_party() {
    std::vector<std::uint8_t> bytes(
        PartyImage::kSize,0xFFU);
    // These are placeholders, not a claim to emulate CODE.1 character
    // creation, combat, equipment or save state.
    init_actor(bytes,0U,"EXPLORER");
    init_actor(bytes,1U,"GUIDE");
    return bytes;
}

ExplorationPreviewParty ExplorationPreview::party(
    const std::optional<std::vector<std::uint8_t>>& existing,
    const std::optional<std::vector<std::uint8_t>>& original_save) {
    if (existing.has_value() &&
        existing->size() == PartyImage::kSize) {
        return {*existing,true,false};
    }

    // SAVE.DAT contains 0x77 bytes of original global state followed by
    // four verbatim 0x248-byte active-party records. In a Level22
    // comparison, those real portraits, items, abilities and conditions
    // are preferable to invented EXPLORER/GUIDE characters. Preserve the
    // unmodified original bytes; this is NOT a full save restore.
    constexpr std::size_t kSavePartyAt=0x77U;
    constexpr std::size_t kLevelAt=0x52U;
    constexpr std::size_t kPartyEnd=kSavePartyAt+PartyImage::kSize;
    if (original_save.has_value() &&
        original_save->size() >= kPartyEnd) {
        const auto& save=*original_save;
        const auto level=static_cast<std::uint32_t>(save[kLevelAt]) |
            (static_cast<std::uint32_t>(save[kLevelAt+1U])<<8U) |
            (static_cast<std::uint32_t>(save[kLevelAt+2U])<<16U) |
            (static_cast<std::uint32_t>(save[kLevelAt+3U])<<24U);
        if(level==ExplorationSessionState::kFreshLevelId) {
            auto begin=save.begin()+
                static_cast<std::ptrdiff_t>(kSavePartyAt);
            return {std::vector<std::uint8_t>(
                begin,begin+static_cast<std::ptrdiff_t>(PartyImage::kSize)),
                false,true};
        }
    }
    return {synthetic_party(),false,false};
}

} // namespace rloft
