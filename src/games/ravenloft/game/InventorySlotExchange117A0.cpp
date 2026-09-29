#include "games/ravenloft/game/InventorySlotExchange117A0.h"
#include <stdexcept>

namespace ravenloft::game {
namespace {
constexpr std::size_t kObjectSlotBase = 0x40;  // DS:21920 - character base DS:218E0
constexpr std::size_t kSlotStateDword = 0x12B; // DS:21A0B - character base DS:218E0
constexpr std::size_t kObjectSlotCount = 21;

std::uint16_t readU16(const data::CharacterRecord& c,std::size_t p){
    return static_cast<std::uint16_t>(c.raw[p]) |
           static_cast<std::uint16_t>(static_cast<std::uint16_t>(c.raw[p+1])<<8u);
}
void writeU16(data::CharacterRecord& c,std::size_t p,std::uint16_t v){
    c.raw[p]=static_cast<std::uint8_t>(v&0xFFu);
    c.raw[p+1]=static_cast<std::uint8_t>((v>>8u)&0xFFu);
}
std::uint32_t readU32(const data::CharacterRecord& c,std::size_t p){
    return static_cast<std::uint32_t>(c.raw[p]) |
           (static_cast<std::uint32_t>(c.raw[p+1])<<8u) |
           (static_cast<std::uint32_t>(c.raw[p+2])<<16u) |
           (static_cast<std::uint32_t>(c.raw[p+3])<<24u);
}
void writeU32(data::CharacterRecord& c,std::size_t p,std::uint32_t v){
    c.raw[p]=static_cast<std::uint8_t>(v&0xFFu);
    c.raw[p+1]=static_cast<std::uint8_t>((v>>8u)&0xFFu);
    c.raw[p+2]=static_cast<std::uint8_t>((v>>16u)&0xFFu);
    c.raw[p+3]=static_cast<std::uint8_t>((v>>24u)&0xFFu);
}

void applyF238(data::CharacterRecord& ch,std::size_t slot,int value){
    // CODE.2 0xF238..0xF270 first re-reads the slot and returns unchanged if
    // it is -1. 0x117A0 has already written the held object before this call.
    const auto objectId=static_cast<std::int16_t>(readU16(ch,kObjectSlotBase+slot*2));
    if(objectId==-1)return;
    const auto shift=static_cast<unsigned>(slot&31u);
    std::uint32_t bits=readU32(ch,kSlotStateDword);
    const std::uint32_t mask=std::uint32_t(1u)<<shift;
    bits &= ~mask;
    // Preserve the original F238 arithmetic rather than normalizing value to
    // boolean: EDX is shifted by CL and ORed directly into the state dword.
    bits |= static_cast<std::uint32_t>(value)<<shift;
    writeU32(ch,kSlotStateDword,bits);
}
}

InventorySlotExchangeResult117A0 InventorySlotExchange117A0::executeOpeningEmptySlot(
    std::size_t characterIndex,
    std::size_t slotIndex,
    int /*allowContainerInsert*/,
    HeldObjectState1A344& held,
    std::array<data::CharacterRecord,4>& party,
    const InventorySlotExchangeContext117A0& context){

    if(characterIndex>=party.size())throw std::out_of_range("117A0 character index");
    if(slotIndex>=kObjectSlotCount)throw std::out_of_range("117A0 slot index");

    auto& ch=party[characterIndex];
    const auto slotOffset=kObjectSlotBase+slotIndex*2;
    const int oldObject=static_cast<std::int16_t>(readU16(ch,slotOffset));
    const std::uint32_t oldBits=readU32(ch,kSlotStateDword);
    const auto oldState=static_cast<std::uint8_t>((oldBits>>(slotIndex&31u))&1u);

    InventorySlotExchangeResult117A0 result{};
    result.oldSlotObject=oldObject;
    result.oldSlotState=oldState;

    // 0x11892: both empty is a no-op. The opening bridge only selects empty
    // slots; a nonempty target belongs to other 0x117A0 branches and is not
    // silently approximated here.
    if(held.objectId==-1 && oldObject==-1)return result;
    if(oldObject!=-1)return result;
    if(held.objectId<0)return result;

    const int incomingObject=held.objectId;
    const std::uint8_t incomingState=held.state;

    // 0x11933..0x11958: write held object, then F238(character,slot,state).
    writeU16(ch,slotOffset,static_cast<std::uint16_t>(incomingObject));
    if(slotIndex<ch.carriedItems.size())
        ch.carriedItems[slotIndex]=static_cast<std::uint16_t>(incomingObject);
    applyF238(ch,slotIndex,incomingState);
    if(context.slotBoundaryF238)
        context.slotBoundaryF238(characterIndex,slotIndex,incomingState);

    // 0x119F8 calls F790(oldObject,oldState) before 0x11A34 invokes 1A5B0
    // with that same old object/state pair. F790 is presentation-only for the
    // opening empty-slot path; 1A5B0 owns the held/cursor logical state.
    if(context.presentationF790)context.presentationF790(oldObject,oldState);
    held.objectId=oldObject;
    held.state=oldState;
    if(context.heldBoundary1A5B0)context.heldBoundary1A5B0(oldObject,oldState);

    result.handled=true;
    return result;
}

} // namespace ravenloft::game
