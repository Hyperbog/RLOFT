#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_f790_runtime_description.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
void put32(std::vector<std::uint8_t>& v,std::size_t at,std::uint32_t n) {
    for(int i=0;i<4;++i)
        v[at+static_cast<std::size_t>(i)]=
          static_cast<std::uint8_t>(n>>(8*i));
}
}

int main() {
    using Static=rloft::OriginalF790StaticResources;
    using Gateway=rloft::OriginalF790RuntimeDescription;
    std::vector<std::uint8_t> c(Static::kCode2FileBytes,0U);
    c[0x27BDC]='L';c[0x27BDD]='E';
    put32(c,0x27BDC+0xC4+0x0C,1U);
    put32(c,0x27BDC+0xC4+0x10,59U);
    put32(c,0x27BDC+0xDC+0x0C,60U);
    put32(c,0x27BDC+0xDC+0x10,19U);
    const auto ds=[&](std::size_t off,const std::string& str) {
        const auto p=Static::kObject2DiskBase+off;
        for(std::size_t i=0;i<str.size();++i)
            c[p+i]=static_cast<std::uint8_t>(str[i]);
    };
    ds(0x598,"; CHARGES: ");ds(0x610," +");ds(0x614,"; ");
    ds(0x618," ");ds(0x61C,"-");ds(0x620,"/");ds(0x624,".5");
    put32(c,Static::kObject2DiskBase+0x6D98,0x598U);
    c[Static::kObject2DiskBase+0x7330]=4U;
    c[Static::kObject2DiskBase+0x7332]=8U;
    c[Static::kObject2DiskBase+0x7330+20U*10U+2U]=0x7fU;
    c[Static::kObject2DiskBase+0x67EC]=8U;
    const Static original_code2(c);

    std::vector<std::uint8_t> ts(4U+0x42U*4U,0U);
    put32(ts,0U,0x42U);
    const auto empty=ts.size();
    ts.push_back(0U);ts.push_back(0U);
    for(std::size_t i=0;i<0x42U;++i)
        put32(ts,4U+4U*i,static_cast<std::uint32_t>(empty));
    const auto t=[&](std::size_t index,const std::string& label) {
        put32(ts,4U+4U*index,static_cast<std::uint32_t>(ts.size()));
        ts.push_back(static_cast<std::uint8_t>(label.size()));
        ts.push_back(0U);
        for(char ch:label)ts.push_back(static_cast<std::uint8_t>(ch)^0xDCU);
    };
    t(0x3dU,"WEIGHT:");
    t(0x3eU,"DAMAGE:");
    t(0x40U,"A.C. BONUS:");
    t(0x41U,"CAPACITY:");
    const rloft::OriginalTTextArchive original_t(ts);

    constexpr std::size_t count=90U;
    std::vector<std::uint8_t> original_objects(4U+count*0x41U,0U);
    put32(original_objects,0U,static_cast<std::uint32_t>(count));
    const auto object=[&](std::size_t id,const char* name,
                          std::uint8_t category,std::uint8_t weight,
                          std::uint8_t subtype) {
        const auto p=4U+id*0x41U;
        std::size_t j=0;
        while(name[j]!=0) {
            original_objects[p+j]=static_cast<std::uint8_t>(name[j]);
            ++j;
        }
        original_objects[p+0x2FU]=category;
        original_objects[p+0x30U]=weight;
        original_objects[p+0x31U]=subtype;
    };
    object(0,"POUCH",1U,2U,0U);
    object(1,"GEM",0U,5U,0U);
    object(2,"ARMOR",2U,3U,0U);
    object(3,"SWORD",9U,4U,0U);
    object(4,"ROCK",0U,9U,0U);
    object(74,"SPECIAL",0U,1U,0U);
    object(80,"BAG",1U,4U,20U);
    original_objects[4U+2U*0x41U+0x34U]=0xfeU;
    original_objects[4U+2U*0x41U+0x37U]=3U;
    const auto weapon=4U+3U*0x41U;
    original_objects[weapon+0x38U]=1U;
    original_objects[weapon+0x39U]=6U;
    original_objects[weapon+0x3AU]=1U;
    original_objects[weapon+0x3BU]=1U;
    original_objects[weapon+0x3CU]=6U;
    original_objects[weapon+0x3DU]=0U;
    const auto parsed=rloft::ObjectCatalog::parse(original_objects);
    rloft::InventoryRuntimeState inventory{};
    inventory.containers[0].slot_count=2U;
    inventory.containers[0].items={1,-1};
    inventory.containers[20].slot_count=8U;
    inventory.containers[20].items.assign(8U,4);

    const auto describe=[&](int id,bool known=false) {
        return Gateway::compose(original_objects,parsed,inventory,id,known,
                                original_code2,original_t);
    };
    assert(describe(0)=="POUCH; WEIGHT:3.5; CAPACITY: 8");
    inventory.containers[0].items[0]=4;
    assert(describe(0)=="POUCH; WEIGHT:5.5; CAPACITY: 8");
    inventory.containers[0].items[0]=-1;
    assert(describe(0)=="POUCH; WEIGHT:1; CAPACITY: 8");

    // Even with heavy mutable contents Bag of Holding uses the exact
    // original F5F8 subtype20 exception (object80 weight field).
    assert(describe(80)=="BAG; WEIGHT:4");
    inventory.containers[20].items.assign(8U,-1);
    assert(describe(80)=="BAG; WEIGHT:4");
    assert(describe(2,true)=="ARMOR +3; A.C. BONUS: 2; ");
    assert(describe(3)=="SWORD; DAMAGE: 2-7/1-6");
    assert(describe(74,true)=="SPECIAL; WEIGHT:0.5; CHARGES: 8");
    assert(describe(-1).empty());

    bool rejected=false;
    try { (void)describe(900); } catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);
    rejected=false;
    auto damaged=original_objects;
    damaged[4U+0x2FU]=7U;
    try { (void)Gateway::compose(damaged,parsed,inventory,0,false,
                                  original_code2,original_t); }
    catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
    rejected=false;
    damaged=original_objects;
    damaged[0]=0U;
    try { (void)Gateway::compose(damaged,parsed,inventory,0,false,
                                  original_code2,original_t); }
    catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
    rejected=false;
    damaged=original_objects;damaged.pop_back();
    try { (void)Gateway::compose(damaged,parsed,inventory,0,false,
                                  original_code2,original_t); }
    catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
}
