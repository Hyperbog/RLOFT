#include "games/ravenloft/game/PlayerHandClick214B8.h"
#include <stdexcept>

namespace ravenloft::game {
namespace {
std::uint16_t u16(const std::array<std::uint8_t,0x248>& r,std::size_t o) {
    return static_cast<std::uint16_t>(r[o]) |
           static_cast<std::uint16_t>(static_cast<std::uint16_t>(r[o+1])<<8u);
}
}

PlayerHandClickResult214B8 PlayerHandClick214B8::execute(std::array<data::CharacterRecord,4>& party,
                                                          std::size_t characterIndex,
                                                          int handIndex,
                                                          std::uint32_t clickFlags,
                                                          const data::ObjectDatabase& objects,
                                                          const PlayerHandClickContext214B8& context) {
    if(characterIndex>=party.size()) throw std::out_of_range("214B8 character index");
    if(handIndex<0||handIndex>1) throw std::out_of_range("214B8 hand index");
    PlayerHandClickResult214B8 result{};

    // 0x214CD..0x214DE tests only the high 0x8000 bit of character +0x10.
    if((u16(party[characterIndex].raw,0x10)&0x8000u)!=0u) {
        result.path=PlayerHandClickResult214B8::Path::InactiveCharacter;
        return result;
    }

    // 0x214E4 gives click bit 3 priority over the normal attack arm.
    if((clickFlags&0x08u)!=0u) {
        result.path=PlayerHandClickResult214B8::Path::ItemExchangeBranch;
        return result;
    }
    if((clickFlags&0x01u)==0u) {
        result.path=PlayerHandClickResult214B8::Path::IgnoredClick;
        return result;
    }

    // 0x216CD..0x216DE masks the full +0x10 status word with 0xF240.
    if((u16(party[characterIndex].raw,0x10)&0xF240u)!=0u) {
        result.path=PlayerHandClickResult214B8::Path::CharacterStateBlocked;
        return result;
    }

    if(HandItemUsability24478::evaluate(characterIndex,handIndex,objects,context.usability24478)==0) {
        result.path=PlayerHandClickResult214B8::Path::HandUnusable;
        return result;
    }
    if(!context.attack26018) throw std::runtime_error("214B8 requires 0x26018 call boundary");

    result.path=PlayerHandClickResult214B8::Path::AttackDispatched;
    result.attack=context.attack26018(-1); // literal PUSH -1 at 0x216EE
    return result;
}

} // namespace ravenloft::game
