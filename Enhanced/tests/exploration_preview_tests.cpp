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

    // A saved game must never be silently imported into this
    // freshly created character flow. Direct walkaround remains
    // explicitly synthetic if CHARS does not exist.
    assert(!rloft::ExplorationPreview::party(std::nullopt)
                .uses_existing_chars);
    assert(rloft::ExplorationPreview::party(std::nullopt)
                .chars == generated);

}
