#include "games/ravenloft/game/HandItemPredicates.h"
#include <array>
#include <algorithm>

namespace ravenloft::game {
namespace {
template<std::size_t N> bool contains(const std::array<int,N>& a,int v){return std::find(a.begin(),a.end(),v)!=a.end();}
}

int HandItemPredicates::spellHandRange23F34(int objectId){
    return objectId>=0x7E && objectId<=0x86 ? 1 : 0;
}

int HandItemPredicates::predicate23E58(int objectId,const data::ObjectDatabase& objects){
    if(spellHandRange23F34(objectId))return 1;
    if(objectId>=0 && static_cast<std::size_t>(objectId)<objects.count()){
        const int cat=objects.at(static_cast<std::size_t>(objectId)).raw[0x2F];
        if(cat==9||cat==10||cat==11)return 1;
    }
    return objectId==0x30 ? 1 : 0;
}

int HandItemPredicates::characterHasClass23EAC(const data::CharacterRecord& c,int classId){
    return (static_cast<std::int8_t>(c.raw[0x15])==classId ||
            static_cast<std::int8_t>(c.raw[0x16])==classId ||
            static_cast<std::int8_t>(c.raw[0x17])==classId) ? 1 : 0;
}

int HandItemPredicates::predicate23EE4(int objectId){
    static constexpr std::array<int,6> ids{{0x2B,0x59,0x68,0x9F,0x102,0x103}};
    return contains(ids,objectId)?1:0;
}

int HandItemPredicates::predicate23F50(int objectId,const data::ObjectDatabase& objects){
    static constexpr std::array<int,6> ids{{0x2E,0x2F,0x41,0x57,0x59,0x5A}};
    if(contains(ids,objectId))return 1;
    if(objectId>=0 && static_cast<std::size_t>(objectId)<objects.count()){
        const int cat=objects.at(static_cast<std::size_t>(objectId)).raw[0x2F];
        if(cat==0x0F||cat==0x0D)return 1;
    }
    return 0;
}

int HandItemPredicates::predicate23FC4(int objectId,const data::ObjectDatabase& objects){
    if(spellHandRange23F34(objectId))return 1;
    if(predicate23F50(objectId,objects))return 1;
    if(predicate23EE4(objectId))return 1;
    static constexpr std::array<int,13> ids{{0x30,0x29,0xFE,0xFF,0x28,0xFC,0xFD,0x27,0xFA,0xFB,0x26,0xF8,0xF9}};
    return contains(ids,objectId)?1:0;
}

int HandItemPredicates::predicate24060(int objectId){
    if(spellHandRange23F34(objectId))return 1;
    if(predicate23EE4(objectId))return 1;
    static constexpr std::array<int,5> ids{{0x31,0x5B,0x26,0xF8,0xF9}};
    return contains(ids,objectId)?1:0;
}

} // namespace ravenloft::game
