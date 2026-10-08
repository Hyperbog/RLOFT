#ifdef NDEBUG
#undef NDEBUG  // Original-branch assertions must run in Release CTest.
#endif
#include "rloft/combat_level_change_gate.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>

namespace {
void word(rloft::PartyImage& party,
          std::size_t actor,
          std::size_t offset,
          std::uint16_t value) {
    const auto i=actor*rloft::PartyImage::kActorSize+offset;
    party.bytes[i]=static_cast<std::uint8_t>(value);
    party.bytes[i+1U]=static_cast<std::uint8_t>(value>>8U);
}
} // namespace

int main() {
    using rloft::CombatLevelChangeGateModel;
    rloft::PartyImage party{};
    for(std::size_t i=0U;i<4U;++i) {
        word(party,i,0x10U,0x8000U);
        const auto b=i*rloft::PartyImage::kActorSize;
        party.bytes[b+0x15U]=0xFFU;
        party.bytes[b+0x16U]=0xFFU;
        party.bytes[b+0x17U]=0xFFU;
    }
    const auto gate=[&](std::size_t actor,int mode) {
        return CombatLevelChangeGateModel::inspect(party,actor,mode);
    };
    assert(!gate(4U,0).has_value());
    assert(*gate(0U,0)==-1);
    word(party,0U,0x10U,0U);
    assert(*gate(0U,0)==-1 && *gate(0U,1)==-1);

    // 50DC zero-mode: class 3 has precedence across all three
    // slots, including over the class-1 level>=9 exception.
    party.bytes[0x17U]=3U;
    assert(*gate(0U,0)==2);
    party.bytes[0x16U]=3U;
    assert(*gate(0U,0)==1);
    party.bytes[0x15U]=3U;
    assert(*gate(0U,0)==0);

    party.bytes[0x15U]=1U;
    party.bytes[0x16U]=0xFFU;
    party.bytes[0x17U]=0xFFU;
    word(party,0U,0x20U,8U);
    assert(*gate(0U,0)==-1);
    word(party,0U,0x20U,9U);
    assert(*gate(0U,0)==0);
    word(party,0U,0x20U,0xFFFFU);
    assert(*gate(0U,0)==-1); // MOVSX signed WORD, not 65535
    word(party,0U,0x20U,9U);
    party.bytes[0x17U]=3U;
    assert(*gate(0U,0)==2);

    // 50DC nonzero-mode: class 4 only, first slot wins.
    assert(*gate(0U,1)==-1);
    party.bytes[0x17U]=4U;
    assert(*gate(0U,1)==2);
    party.bytes[0x16U]=4U;
    assert(*gate(0U,1)==1);
    party.bytes[0x15U]=4U;
    assert(*gate(0U,1)==0);
    assert(*gate(0U,-1)==0);
    assert(*gate(0U,0)==-1);

    // Exact 0xF240 mask suppresses BOTH paths, regardless of HP.
    for(std::uint16_t bit: {0x0040U,0x0200U,0x1000U,0x2000U,
                           0x4000U,0x8000U}) {
        word(party,0U,0x10U,bit);
        assert(*gate(0U,0)==-1 && *gate(0U,1)==-1);
    }
    // 0x0100 is not excluded, but 0x0040 is.
    word(party,0U,0x10U,0x0100U);
    assert(*gate(0U,1)==0);
    word(party,0U,0x10U,0x0140U);
    assert(*gate(0U,1)==-1);

    // Actor-stride correctness; unaffected neighbor's fields win.
    word(party,1U,0x10U,0U);
    party.bytes[1U*party.kActorSize+0x16U]=4U;
    assert(*gate(1U,1)==1);
    assert(*gate(1U,0)==-1);
    assert(*gate(0U,1)==-1);
}
