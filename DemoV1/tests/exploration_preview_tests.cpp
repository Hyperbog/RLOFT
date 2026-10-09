#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/exploration_preview.hpp"
#include "rloft/exploration_state.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace {
std::uint16_t u16(const std::vector<std::uint8_t>& bytes,
                  std::size_t at) {
    return static_cast<std::uint16_t>(bytes.at(at)) |
           (static_cast<std::uint16_t>(bytes.at(at+1U)) << 8U);
}
}

int main() {
    const auto generated =
        rloft::ExplorationPreview::synthetic_party();
    assert(generated.size() == rloft::PartyImage::kSize);
    const auto party = rloft::PartyImage::from_bytes(generated);
    assert(party.name(0) == "EXPLORER");
    assert(party.name(1) == "GUIDE");
    assert(party.active(0));
    assert(party.active(1));
    assert(!party.active(2));
    assert(!party.active(3));
    for (std::size_t actor=0U; actor<2U; ++actor) {
        const auto base = actor*rloft::PartyImage::kActorSize;
        assert(u16(generated,base+0x10U)==0U);
        assert(u16(generated,base+0x1CU)==12U);
        assert(u16(generated,base+0x1EU)==12U);
        assert(u16(generated,base+0x20U)==1U);
        for (std::size_t slot=0U; slot<10U; ++slot) {
            assert(u16(generated,base+0x40U+2U*slot)==0xFFFFU);
        }
    }
    // A read-only existing CHARS party has precedence, regardless of
    // actor contents. It is copied intact, never rewritten to disk.
    auto existing = generated;
    existing[0]='X';
    const auto read_only = rloft::ExplorationPreview::party(existing);
    assert(read_only.uses_existing_chars);
    assert(read_only.chars == existing);
    assert(existing[0]=='X');
    const auto absent =
        rloft::ExplorationPreview::party(std::nullopt);
    assert(!absent.uses_existing_chars);
    assert(absent.chars==generated);
    // Invalid original CHARS bytes are not promoted into runtime party
    // structures. Preview uses an isolated temporary image instead.
    const auto invalid =
        rloft::ExplorationPreview::party(
            std::vector<std::uint8_t>{0xAAU,0xBBU});
    assert(!invalid.uses_existing_chars);
    assert(invalid.chars == generated);
    assert(rloft::ExplorationPreview::synthetic_party()==generated);

    // Original SAVE.DAT owns the four 0x248 character records at +0x77
    // (after the 0x77-byte global prefix). Its equipment and portraits
    // must NOT be replaced by the invented empty EXPLORER/GUIDE records.
    std::vector<std::uint8_t> save(
        0x77U+rloft::PartyImage::kSize,0U);
    save[0x52U]=22U; // original level-22 quick-start save
    auto save_party=generated;
    std::fill(save_party.begin(),save_party.begin()+16U,0U);
    constexpr char kSavedName[]="EHLIANNA";
    std::copy_n(kSavedName,8U,save_party.begin());
    save_party[0x12U]=13U;
    save_party[0x40U]=0xA6U; // original party item ID 166
    save_party[0x41U]=0U;
    std::copy(save_party.begin(),save_party.end(),
              save.begin()+0x77U);
    const auto from_save=rloft::ExplorationPreview::party(
        std::nullopt,save);
    assert(!from_save.uses_existing_chars);
    assert(from_save.uses_original_save_party);
    assert(from_save.chars==save_party);
    assert(rloft::PartyImage::from_bytes(from_save.chars)
           .name(0)=="EHLIANNA");
    assert(u16(from_save.chars,0x40U)==166U);
    assert(from_save.chars[0x12U]==13U);

    // Actual CHARS takes precedence: never replace an imported
    // character-creation party with an unrelated saved party.
    const auto chars_wins=rloft::ExplorationPreview::party(
        existing,save);
    assert(chars_wins.uses_existing_chars);
    assert(!chars_wins.uses_original_save_party);
    assert(chars_wins.chars==existing);
    save[0x52U]=21U;
    assert(!rloft::ExplorationPreview::party(
        std::nullopt,save).uses_original_save_party);
    save.resize(0x77U+rloft::PartyImage::kSize-1U);
    assert(!rloft::ExplorationPreview::party(
        std::nullopt,save).uses_original_save_party);
}
