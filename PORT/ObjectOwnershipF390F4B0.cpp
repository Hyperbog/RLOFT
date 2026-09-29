#include "games/ravenloft/game/ObjectOwnershipF390F4B0.h"
#include <stdexcept>

namespace ravenloft::game {
namespace {
std::uint16_t u16raw(const data::CharacterRecord& c,std::size_t p){
    return static_cast<std::uint16_t>(c.raw[p]) |
           static_cast<std::uint16_t>(static_cast<std::uint16_t>(c.raw[p+1])<<8u);
}
}

bool ObjectContainsF390::evaluate(int objectId,
                                  int targetId,
                                  const data::ObjectDatabase& objects,
                                  const std::array<std::vector<std::int16_t>,data::kContainerDescriptorCount>& containerItems){
    // 0xF390 first compares the two WORD ids directly.
    if(static_cast<std::int16_t>(objectId)==static_cast<std::int16_t>(targetId))return true;
    if(objectId<0)return false;
    if(static_cast<std::size_t>(objectId)>=objects.count())throw std::out_of_range("F390 object id");
    const auto& def=objects.at(static_cast<std::size_t>(objectId));
    if(def.raw[0x2F]!=1)return false;
    const auto descriptor=static_cast<std::int8_t>(def.raw[0x31]);
    if(descriptor<0 || static_cast<std::size_t>(descriptor)>=containerItems.size())
        throw std::out_of_range("F390 container descriptor");
    for(const auto child:containerItems[static_cast<std::size_t>(descriptor)]){
        if(child==-1)continue;
        if(evaluate(child,targetId,objects,containerItems))return true;
    }
    return false;
}

bool PartyOwnsObjectF4B0::evaluate(int targetId,
                                   const HeldObjectState1A344& held,
                                   const std::array<data::CharacterRecord,4>& party,
                                   const data::ObjectDatabase& objects,
                                   const std::array<std::vector<std::int16_t>,data::kContainerDescriptorCount>& containerItems){
    if(held.objectId!=-1 && ObjectContainsF390::evaluate(held.objectId,targetId,objects,containerItems))return true;
    for(const auto& c:party){
        // 0xF4E1 masks the high status byte with 0x80, i.e. status 0x8000.
        if((u16raw(c,0x10)&0x8000u)!=0)continue;
        for(std::size_t slot=0;slot<21;++slot){
            const auto id=static_cast<std::int16_t>(u16raw(c,0x40+slot*2));
            if(id==-1)continue;
            if(ObjectContainsF390::evaluate(id,targetId,objects,containerItems))return true;
        }
    }
    return false;
}

} // namespace ravenloft::game
