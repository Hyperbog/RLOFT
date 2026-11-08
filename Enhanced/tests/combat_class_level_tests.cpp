#include "rloft/combat_class_level.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace {
void klass(rloft::PartyImage& p,std::size_t actor,
           std::size_t lane,std::int8_t code) {
    p.bytes.at(actor*rloft::PartyImage::kActorSize+
        0x15U+lane)=static_cast<std::uint8_t>(code);
}
void level(rloft::PartyImage& p,std::size_t actor,
           std::size_t lane,std::int16_t value) {
    const auto index=actor*rloft::PartyImage::kActorSize+
        0x20U+lane*2U;
    p.bytes.at(index)=static_cast<std::uint8_t>(value);
    p.bytes.at(index+1U)=static_cast<std::uint8_t>(
        static_cast<std::uint16_t>(value)>>8U);
}
} // namespace

int main() {
    using rloft::CombatClassLevelModel;
    rloft::PartyImage p{};
    p.bytes.fill(0xFFU);
    for(std::size_t i=0;i<4U;++i) {
        assert(CombatClassLevelModel::for_class(p,i,4)==1);
    }
    // First matching class, not maximum across lanes.
    klass(p,0U,0U,4);level(p,0U,0U,5);
    klass(p,0U,1U,4);level(p,0U,1U,21);
    klass(p,0U,2U,4);level(p,0U,2U,14);
    assert(CombatClassLevelModel::for_class(p,0U,4)==5);
    klass(p,0U,0U,-1);
    assert(CombatClassLevelModel::for_class(p,0U,4)==21);
    klass(p,0U,1U,7);
    assert(CombatClassLevelModel::for_class(p,0U,4)==14);
    // The class byte and WORD are separately sign extended.
    klass(p,1U,0U,-1);level(p,1U,0U,-3);
    assert(CombatClassLevelModel::for_class(p,1U,-1)==-3);
    assert(CombatClassLevelModel::for_class(p,1U,255)==1);
    // Signed class level 0 is not substituted with fallback 1.
    klass(p,1U,1U,4);level(p,1U,1U,0);
    assert(CombatClassLevelModel::for_class(p,1U,4)==0);
    const auto before=p.bytes;
    (void)CombatClassLevelModel::for_class(p,0U,4);
    assert(p.bytes==before);
    bool rejected=false;
    try {(void)CombatClassLevelModel::for_class(p,4U,4);}
    catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);
}
