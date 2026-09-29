#include "games/ravenloft/game/OpeningGroundPickup223F4.h"

namespace ravenloft::game {

OpeningGroundPickupResult223F4 OpeningGroundPickup223F4::execute(
    std::size_t objectIndex,
    std::uint32_t clickFlags,
    std::vector<data::RuntimePlacement22>& runtimeObjects,
    HeldObjectState1A344& held,
    std::array<data::CharacterRecord,4>& party,
    const data::ObjectDatabase& objects,
    const std::array<std::vector<std::int16_t>,data::kContainerDescriptorCount>& containerItems,
    const GroundObjectPickupContext21704& supplied){

    OpeningGroundPickupResult223F4 out{};
    // 0x224E6 tests hit-record type==2, then 0x224EF requires click bit 0.
    // This class is entered only after the upstream type-2 selection.
    if((clickFlags&1u)==0 || objectIndex>=runtimeObjects.size())return out;
    out.handled=true;

    GroundObjectPickupContext21704 c=supplied;
    const auto exactOwns=[&](int id){
        return PartyOwnsObjectF4B0::evaluate(id,held,party,objects,containerItems);
    };
    if(!c.ownsObjectF4B0)c.ownsObjectF4B0=exactOwns;
    const auto picked=GroundObjectPickup21704::execute(objectIndex,runtimeObjects,held,c);
    out.picked=picked.picked;
    out.objectId=picked.objectId;
    if(!picked.picked)return out;

    // 0x225E7..0x226CF: any one of the four opening quest objects starts
    // 0x21F70 only when F4B0 proves ownership of the other three.
    switch(out.objectId){
    case 0x87: out.startMistTransition=exactOwns(0x88)&&exactOwns(0x74)&&exactOwns(0xE9); break;
    case 0x88: out.startMistTransition=exactOwns(0x87)&&exactOwns(0x74)&&exactOwns(0xE9); break;
    case 0x74: out.startMistTransition=exactOwns(0x87)&&exactOwns(0x88)&&exactOwns(0xE9); break;
    case 0xE9: out.startMistTransition=exactOwns(0x87)&&exactOwns(0x88)&&exactOwns(0x74); break;
    default: break;
    }
    return out;
}

} // namespace ravenloft::game
