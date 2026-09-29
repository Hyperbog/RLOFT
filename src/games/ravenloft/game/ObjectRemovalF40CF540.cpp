#include "games/ravenloft/game/ObjectRemovalF40CF540.h"
#include <stdexcept>

namespace ravenloft::game {
namespace {
std::uint16_t u16raw(const data::CharacterRecord& c,std::size_t p){
    return static_cast<std::uint16_t>(c.raw[p]) |
           static_cast<std::uint16_t>(static_cast<std::uint16_t>(c.raw[p+1])<<8u);
}
void setU16raw(data::CharacterRecord& c,std::size_t p,std::uint16_t v){
    c.raw[p]=static_cast<std::uint8_t>(v&0xFFu);
    c.raw[p+1]=static_cast<std::uint8_t>((v>>8u)&0xFFu);
}
}

bool ContainerRemoveF40C::execute(
    int objectId,
    int targetId,
    const data::ObjectDatabase& objects,
    std::array<std::vector<std::int16_t>,data::kContainerDescriptorCount>& containerItems,
    std::array<std::uint8_t,data::kContainerDescriptorCount>& containerState){

    if(objectId<0)return false;
    if(static_cast<std::size_t>(objectId)>=objects.count())throw std::out_of_range("F40C object id");
    const auto& def=objects.at(static_cast<std::size_t>(objectId));
    if(def.raw[0x2F]!=1)return false;
    const auto descriptor=static_cast<std::int8_t>(def.raw[0x31]);
    if(descriptor<0 || static_cast<std::size_t>(descriptor)>=containerItems.size())
        throw std::out_of_range("F40C container descriptor");
    const auto di=static_cast<std::size_t>(descriptor);
    auto& items=containerItems[di];
    for(std::size_t slot=0;slot<items.size();++slot){
        const auto child=items[slot];
        if(child==-1)continue;
        if(child==static_cast<std::int16_t>(targetId)){
            items[slot]=-1;
            // 0xF452..0xF46D clears the corresponding DS:7339 slot bit.
            // Shipped descriptor capacities do not exceed the 8-bit bitfield.
            if(slot<8)containerState[di]=static_cast<std::uint8_t>(containerState[di] & ~(std::uint8_t(1u)<<slot));
            return true;
        }
        if(execute(child,targetId,objects,containerItems,containerState))return true;
    }
    return false;
}

bool ObjectRemoveF540::execute(
    int targetId,
    HeldObjectState1A344& held,
    std::array<data::CharacterRecord,4>& party,
    const data::ObjectDatabase& objects,
    std::array<std::vector<std::int16_t>,data::kContainerDescriptorCount>& containerItems,
    std::array<std::uint8_t,data::kContainerDescriptorCount>& containerState,
    const ObjectRemoveContextF540& c){

    // 0xF54B..0xF57B: held/cursor object always has priority.
    if(held.objectId!=-1){
        if(static_cast<std::int16_t>(held.objectId)==static_cast<std::int16_t>(targetId)){
            held.objectId=-1;
            held.state=0;
            return true;
        }
        if(ContainerRemoveF40C::execute(held.objectId,targetId,objects,containerItems,containerState))return true;
    }

    // 0xF581..0xF5ED: four 0x248-byte character records, skip status 0x8000,
    // then exactly 21 WORD object slots beginning at +0x40.
    for(std::size_t ci=0;ci<party.size();++ci){
        auto& ch=party[ci];
        if((u16raw(ch,0x10)&0x8000u)!=0)continue;
        for(std::size_t slot=0;slot<21;++slot){
            const auto off=0x40+slot*2;
            const auto objectId=static_cast<std::int16_t>(u16raw(ch,off));
            if(objectId==-1)continue;
            if(objectId==static_cast<std::int16_t>(targetId)){
                setU16raw(ch,off,0xFFFFu);
                if(slot<ch.carriedItems.size())ch.carriedItems[slot]=0xFFFFu;
                if(c.slotBoundaryF238)c.slotBoundaryF238(ci,slot,0);
                return true;
            }
            if(ContainerRemoveF40C::execute(objectId,targetId,objects,containerItems,containerState))return true;
        }
    }
    return false;
}

} // namespace ravenloft::game
