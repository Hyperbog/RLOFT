#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_f790_resolved_description.hpp"
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

int main() {
    using Static=rloft::OriginalF790StaticResources;
    std::vector<std::uint8_t> c(Static::kCode2FileBytes,0U);
    const auto w32=[&](std::size_t at,std::uint32_t n) {
        for(int i=0;i<4;++i)
            c[at+static_cast<std::size_t>(i)]=
                static_cast<std::uint8_t>(n>>(i*8));
    };
    c[0x27BDC]='L';c[0x27BDD]='E';
    w32(0x27BDC+0xC4+0x0C,1U);w32(0x27BDC+0xC4+0x10,59U);
    w32(0x27BDC+0xDC+0x0C,60U);w32(0x27BDC+0xDC+0x10,19U);
    const auto put=[&](std::size_t at,const std::string& s) {
        for(std::size_t i=0;i<s.size();++i)
            c[Static::kObject2DiskBase+at+i]=
                static_cast<std::uint8_t>(s[i]);
    };
    put(0x598,"; CHARGES: ");put(0x610," +");put(0x614,"; ");
    put(0x618," ");put(0x61C,"-");put(0x620,"/");put(0x624,".5");
    w32(Static::kObject2DiskBase+0x6D98,0x598U);
    c[Static::kObject2DiskBase+0x7330]=4U;
    c[Static::kObject2DiskBase+0x7332]=8U;
    w32(Static::kObject2DiskBase+0x7335,0x6FC4U);
    const Static statics(c);

    // Synthetic T records containing labels independently read from
    // shipped T. No copyrighted original archive is embedded in CI.
    std::vector<std::uint8_t> t(4+0x42*4,0U);
    t[0]=0x42;
    const auto wt32=[&](std::size_t at,std::uint32_t n) {
        for(int i=0;i<4;++i)
            t[at+static_cast<std::size_t>(i)]=
                static_cast<std::uint8_t>(n>>(i*8));
    };
    const std::size_t empty=t.size();t.push_back(0);t.push_back(0);
    for(int i=0;i<0x42;++i)
        wt32(4U+4U*static_cast<std::size_t>(i),
             static_cast<std::uint32_t>(empty));
    const auto add=[&](int index,const std::string& s) {
        const auto at=t.size();
        wt32(4U+4U*static_cast<std::size_t>(index),
             static_cast<std::uint32_t>(at));
        t.push_back(static_cast<std::uint8_t>(s.size()));t.push_back(0);
        for(char v:s)t.push_back(static_cast<std::uint8_t>(v)^0xDCU);
    };
    add(0x3d,"WEIGHT:");add(0x3e,"DAMAGE:");
    add(0x40,"A.C. BONUS:");add(0x41,"CAPACITY:");
    const rloft::OriginalTTextArchive archive(t);

    rloft::OriginalF790ItemDescriptionSource::Record rec{};
    const auto rename=[&](const char* s) {
        rec.fill(0U);
        for(std::size_t i=0;s[i]!=0;++i)
            rec[i]=static_cast<std::uint8_t>(s[i]);
    };
    const auto resolved=[&](int id,bool known=false) {
        return rloft::OriginalF790ResolvedDescription::compose(
            rec,id,known,statics,archive,[](int){return 0U;});
    };
    rename("POUCH");rec[0x2f]=1;rec[0x30]=2;rec[0x31]=0;
    assert(resolved(0)=="POUCH; WEIGHT:1; CAPACITY: 8");
    rename("SHIELD");rec[0x2f]=2;rec[0x34]=0xff;
    assert(resolved(33)=="SHIELD; A.C. BONUS: 1; ");
    rename("MACE");rec[0x2f]=9;
    rec[0x38]=1;rec[0x39]=6;rec[0x3a]=1;
    rec[0x3b]=1;rec[0x3c]=6;rec[0x3d]=0;
    assert(resolved(37)=="MACE; DAMAGE: 2-7/1-6");
    rename("WAND");rec[0x2f]=0;rec[0x30]=1;rec[0x31]=1;
    assert(resolved(75)=="WAND; WEIGHT:0.5");
    assert(resolved(75,true)=="WAND; WEIGHT:0.5; CHARGES: 0");
}
