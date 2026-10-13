#include "rloft_native/native_party_inventory.hpp"
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

using namespace rloft_native;
namespace {
void u16(std::vector<std::uint8_t>& out,std::size_t at,unsigned value) {
    out.at(at)=static_cast<std::uint8_t>(value);
    out.at(at+1U)=static_cast<std::uint8_t>(value>>8U);
}
void ch16(NativeCharsImage& out,std::size_t at,unsigned value) {
    out.at(at)=static_cast<std::uint8_t>(value);
    out.at(at+1U)=static_cast<std::uint8_t>(value>>8U);
}
std::vector<std::uint8_t> object_fixture() {
    std::vector<std::uint8_t> bytes(4U+336U*65U,0U);
    bytes[0]=0x50;bytes[1]=1;
    auto set=[&](unsigned id,const std::string& value,unsigned cat,unsigned icon) {
        const auto at=4U+id*65U;
        assert(value.size()<40U);
        for(std::size_t i=0;i<value.size();++i)
            bytes[at+i]=static_cast<std::uint8_t>(value[i]);
        bytes[at+0x2FU]=static_cast<std::uint8_t>(cat);
        u16(bytes,at+0x2DU,icon);
    };
    set(0,"POUCH",1,0);
    set(6,"QUIVER",1,6);
    set(12,"LEATHER ARMOR",4,12);
    set(70,"RING OF PROTECTION",5,70);
    return bytes;
}
NativePartyStart party_fixture() {
    NativeCharsImage chars{};
    for(unsigned actor=0;actor<2U;++actor) {
        const auto at=actor*0x248U;
        const char* label=actor?"ELENA":"VIKTOR";
        for(unsigned i=0;label[i];++i)
            chars[at+i]=static_cast<std::uint8_t>(label[i]);
        chars[at+0x1CU]=8;chars[at+0x1EU]=8;
        for(unsigned slot=0;slot<21U;++slot)
            ch16(chars,at+0x40U+2U*slot,0xFFFFU);
    }
    ch16(chars,0x40U+2U*0U,0U);
    ch16(chars,0x40U+2U*14U,12U);
    ch16(chars,0x40U+2U*12U,70U);
    ch16(chars,0x248U+0x40U+2U*19U,6U);
    // The original +12B bitset is independent of the object definition.
    chars[0x12BU]=0x01U; // slot0 known, ring and hand unknown.
    chars[0x248U+0x12DU]=0x08U; // bit 19 = third known-mask byte, 0x08
    return decode_native_party_start(chars);
}
template<class F> void rejects(F fn) {
    bool rejected=false;try{fn();}catch(const std::exception&){rejected=true;}
    assert(rejected);
}
} // namespace
int main(){
    const auto objects=object_fixture();
    const auto party=party_fixture();
    const auto result=inspect_native_party_inventory(party,objects);
    assert(result.object_definitions==336U&&result.occupied_total==4U);
    assert(!result.live_inventory_emulation_verified);
    assert(result.actors[0].actor_name=="VIKTOR");
    assert(result.actors[1].actor_name=="ELENA");
    assert(result.actors[0].occupied_count==3U);
    assert(result.actors[1].occupied_count==1U);
    assert(result.actors[0].slots[0].occupied);
    assert(result.actors[0].slots[0].object_id==0);
    assert(result.actors[0].slots[0].source_name=="POUCH");
    assert(result.actors[0].slots[0].source_category==1);
    assert(result.actors[0].slots[0].absolute_res1_ui_id==168U);
    assert(result.actors[0].slots[0].identified);
    assert(result.actors[0].slots[12].source_name=="RING OF PROTECTION");
    assert(!result.actors[0].slots[12].identified);
    assert(result.actors[0].slots[14].source_name=="LEATHER ARMOR");
    assert(!result.actors[0].slots[14].identified);
    assert(!result.actors[0].slots[20].occupied);
    assert(result.actors[0].slots[20].object_id==-1);
    assert(result.actors[1].slots[19].source_name=="QUIVER");
    assert(result.actors[1].slots[19].identified);
    assert(result.actors[1].slots[19].absolute_res1_ui_id==174U);
    assert(std::string(native_inventory_slot_label(14))=="Hand A");
    assert(std::string(native_inventory_slot_label(19))=="Quiver");
    rejects([&]{static_cast<void>(native_inventory_slot_label(21));});
    auto broken=objects;
    broken.resize(broken.size()-1);
    rejects([&]{static_cast<void>(
        inspect_native_party_inventory(party,broken));});
    broken=objects;broken[4U+12U*65U+0x2FU]=16U;
    rejects([&]{static_cast<void>(
        inspect_native_party_inventory(party,broken));});
    broken=objects;broken[4U+12U*65U+0x2DU]=0xFFU;
    rejects([&]{static_cast<void>(
        inspect_native_party_inventory(party,broken));});
    broken=objects;
    broken[4U+12U*65U]=0x1FU;
    rejects([&]{static_cast<void>(
        inspect_native_party_inventory(party,broken));});
    auto foreign=party;
    ch16(foreign.chars,0x40U+2U*14U,336U);
    rejects([&]{static_cast<void>(
        inspect_native_party_inventory(foreign,objects));});
    foreign=party;
    foreign.actors[0].name="FAKE";
    rejects([&]{static_cast<void>(
        inspect_native_party_inventory(foreign,objects));});
    foreign=party;
    ch16(foreign.chars,0x248U+0x40U+2U*19U,0xFFFFU);
    const auto no_quiver=inspect_native_party_inventory(foreign,objects);
    assert(!no_quiver.actors[1].slots[19].occupied);
    assert(no_quiver.occupied_total==3U);
}
