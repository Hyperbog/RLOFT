#include "games/ravenloft/game/GroundObjectPickup21704.h"
#include <stdexcept>

namespace ravenloft::game {

GroundObjectPickupResult21704 GroundObjectPickup21704::execute(std::size_t objectIndex,
                                                                std::vector<data::RuntimePlacement22>& runtimeObjects,
                                                                HeldObjectState1A344& held,
                                                                const GroundObjectPickupContext21704& c){
    if(objectIndex>=runtimeObjects.size())return {};
    const auto rec=runtimeObjects[objectIndex];
    GroundObjectPickupResult21704 r{};
    r.picked=true;
    r.objectId=rec.type;
    // 0x2170A..0x21718 extracts original runtime +0x14 bit 0x0400.
    r.heldState=(rec.field14&0x0400u)?1u:0u;
    held.objectId=static_cast<std::int16_t>(r.objectId);
    held.state=r.heldState;

    // 0x21733..0x2175D compacts the 22-byte array and decrements its count.
    runtimeObjects.erase(runtimeObjects.begin()+static_cast<std::ptrdiff_t>(objectIndex));

    const auto set=[&](int v){if(c.setEventStateBF44)c.setEventStateBF44(v);};
    const auto owns=[&](int id){return c.ownsObjectF4B0?c.ownsObjectF4B0(id):false;};
    const int id=held.objectId;
    if(id>=0x61 && id<=0x64 && owns(0x35))set(0x0F);
    if(id==0x49)set(0x25);
    if(id==0x9E)set(0x36);
    if(id==0x5F)set(0x37);
    if(id==0x8B)set(0x38);
    if(id==0x10)set(0x39); // exact 0xF2B4 predicate true for 0x10/0x5F, excluding 0x5F here.
    if(id>=0xA7 && id<=0xC9)set(0x3A);
    if(id==0x77)set(0x3C);
    if(id==0x75)set(0x29);
    if(id==0x74 && c.query1C1DC && c.query1C1DC(0x51)!=0)set(0x2F);
    return r;
}

} // namespace ravenloft::game
