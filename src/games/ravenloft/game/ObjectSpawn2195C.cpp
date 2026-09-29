#include "games/ravenloft/game/ObjectSpawn2195C.h"
#include "games/ravenloft/game/MovementVector2DF2C.h"
#include <stdexcept>

namespace ravenloft::game {
namespace {

void put16(std::array<std::uint8_t,22>& raw,std::size_t p,std::uint16_t v){
    raw[p]=static_cast<std::uint8_t>(v);
    raw[p+1]=static_cast<std::uint8_t>(v>>8u);
}
void put32(std::array<std::uint8_t,22>& raw,std::size_t p,std::uint32_t v){
    for(int i=0;i<4;++i)raw[p+i]=static_cast<std::uint8_t>(v>>(i*8));
}
void sync(data::RuntimePlacement22& a){
    put32(a.raw,0x00,static_cast<std::uint32_t>(a.yFixed));
    put32(a.raw,0x04,static_cast<std::uint32_t>(a.xFixed));
    put32(a.raw,0x08,a.state);
    put16(a.raw,0x0C,a.field0C);
    put16(a.raw,0x0E,a.type);
    put16(a.raw,0x10,a.field10);
    put16(a.raw,0x12,static_cast<std::uint16_t>(a.hitPoints));
    put16(a.raw,0x14,a.field14);
}
int truncDiv3(int n){return n/3;}
int oneWrap360(int d){
    if(d<0)d+=360;
    if(d>=360)d-=360;
    return d;
}
std::int16_t integerPart(std::int32_t fixed){
    return static_cast<std::int16_t>(static_cast<std::uint32_t>(fixed)>>16u);
}
std::uint16_t cellIndex(std::int32_t x,std::int32_t y,std::uint16_t width){
    const auto yi=static_cast<std::int32_t>(integerPart(y));
    const auto xi=static_cast<std::int32_t>(integerPart(x));
    return static_cast<std::uint16_t>(yi*static_cast<std::int32_t>(width)+xi);
}
}

ObjectSpawnResult2195C ObjectSpawn2195C::execute(
    int scatterParameter,
    std::uint16_t objectId,
    int placementFlagSource,
    std::int32_t sourceXFixed,
    std::int32_t sourceYFixed,
    int scatterFromFacing,
    int invokeBoundary1A344,
    std::vector<data::RuntimePlacement22>& runtimeObjects,
    const ObjectSpawnContext2195C& c) {

    if(c.levelWidth25619==0)
        throw std::invalid_argument("CODE.2 0x2195C requires DS:25619 level width");

    std::int32_t x=sourceXFixed;
    std::int32_t y=sourceYFixed;
    ObjectSpawnResult2195C result{};

    // 0x21960..0x219C4. Argument 6 scatters the object 1.5 world units from
    // the source, using the party/global facing rather than the NPC facing.
    if(scatterFromFacing!=0){
        if(!c.trig)throw std::invalid_argument("CODE.2 0x2195C scatter path requires SINTAB");
        const int q=truncDiv3(scatterParameter-160);
        const int angle=oneWrap360(c.facingDegreesC658-q);
        const auto v=MovementVector2DF2C::compute(*c.trig,0x00018000,angle,c.gridSnapC02F);
        x=static_cast<std::int32_t>(static_cast<std::uint32_t>(x)+static_cast<std::uint32_t>(v.deltaX));
        y=static_cast<std::int32_t>(static_cast<std::uint32_t>(y)+static_cast<std::uint32_t>(v.deltaY));
        result.scattered=true;
    }

    // 0x219D5..0x219E9. C02F retains the signed whole-cell word and forces
    // both fractional words to 0x8000 before collision classification.
    if(c.gridSnapC02F){
        x=static_cast<std::int32_t>((static_cast<std::uint32_t>(x)&0xFFFF0000u)|0x8000u);
        y=static_cast<std::int32_t>((static_cast<std::uint32_t>(y)&0xFFFF0000u)|0x8000u);
    }

    std::uint16_t cell=cellIndex(x,y,c.levelWidth25619);

    // 0x219EB..0x21A3E. 0x190 is the literal special-object mover id. The
    // original falls back exactly to the source point only on a zero return;
    // -1 and +1 both keep the scattered point.
    if(c.collision2D5D0){
        if(c.collision2D5D0(y,x,0x190)==0){
            x=sourceXFixed;
            y=sourceYFixed;
            cell=cellIndex(x,y,c.levelWidth25619);
            result.collisionFallback=true;
        }
    }

    // The executable has a 300-entry object work array. At exactly 300 it
    // memmoves records 1..299 over 0..298 and writes the new record at 299.
    // This is the only capacity case exercised by 0x2195C itself.
    std::size_t index=runtimeObjects.size();
    if(index==300){
        runtimeObjects.erase(runtimeObjects.begin());
        index=299;
    }
    if(index>300)
        throw std::runtime_error("CODE.2 0x2195C runtime object count exceeded original work-array boundary");

    data::RuntimePlacement22 rec{};
    rec.yFixed=y;
    rec.xFixed=x;
    rec.field0C=cell;
    rec.type=objectId;
    // 0x21A8B..0x21AAD: clear 0x05F8-equivalent field14 bits and source
    // bit 10 solely from argument 3 low bit. A fresh opening object record is
    // zeroed, so the literal operation reduces to this one bit.
    rec.field14=static_cast<std::uint16_t>((placementFlagSource&1)<<10u);
    sync(rec);

    if(index==runtimeObjects.size()) runtimeObjects.push_back(rec);
    else runtimeObjects[index]=rec;
    result.index=index;

    if(invokeBoundary1A344!=0 && c.boundary1A344)c.boundary1A344(-1,0);
    if(objectId==0x89u && c.boundary21880)c.boundary21880();
    return result;
}

} // namespace ravenloft::game
