#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_f790_static_resources.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

static std::vector<std::uint8_t> load(const char* path) {
    std::ifstream in(path,std::ios::binary);
    if(!in)throw std::runtime_error("original file unavailable");
    return {std::istreambuf_iterator<char>(in),std::istreambuf_iterator<char>()};
}
int main(int argc,char** argv) {
    using Static=rloft::OriginalF790StaticResources;
    if(argc==1) {
        // Copyright-free synthetic two-object LE fixture for CI.
        std::vector<std::uint8_t> v(Static::kCode2FileBytes,0U);
        const auto write32=[&](std::size_t p,std::uint32_t n) {
            for(int i=0;i<4;++i)v[p+static_cast<std::size_t>(i)]=
                static_cast<std::uint8_t>(n>>(8U*i));
        };
        v[0x27BDC]='L';v[0x27BDD]='E';
        write32(0x27BDC+0xC4+0x0C,1U);
        write32(0x27BDC+0xC4+0x10,59U);
        write32(0x27BDC+0xDC+0x0C,60U);
        write32(0x27BDC+0xDC+0x10,19U);
        const auto put=[&](std::size_t offset,const char* s) {
            std::size_t k=0;
            do {v[Static::kObject2DiskBase+offset+k]=
                     static_cast<std::uint8_t>(s[k]);} while(s[k++]!=0);
        };
        put(0x598,"; CHARGES: ");put(0x610," +");
        put(0x614,"; ");put(0x618," ");put(0x61C,"-");
        put(0x620,"/");put(0x624,".5");
        write32(Static::kObject2DiskBase+0x6D98,0x598U);
        v[Static::kObject2DiskBase+0x7330]=4U;
        v[Static::kObject2DiskBase+0x7332]=8U;
        write32(Static::kObject2DiskBase+0x7335,0x6FC4U);
        v[Static::kObject2DiskBase+0x6FC4]=71U;
        for(int i=1;i<4;++i) {
            const auto pos=Static::kObject2DiskBase+0x6FC4+
                           2U*static_cast<std::size_t>(i);
            v[pos]=0xff;v[pos+1U]=0xff;
        }
        const Static smoke(v);
        assert(smoke.ds_text(0x6D98)=="; CHARGES: ");
        assert(smoke.descriptor(0).count==4U);
        assert(smoke.initial_item(0,0)==71);
        assert(smoke.initial_item(0,1)==-1);
        bool rejected=false;
        try{(void)smoke.descriptor(-1);}catch(const std::out_of_range&){rejected=true;}
        assert(rejected);
        return 0;
    }
    if(argc!=3)throw std::invalid_argument("supply CODE.2 and OBJECTS.DAT paths");
    const auto binary=load(argv[1]); const auto objects=load(argv[2]);
    const Static statics(binary);
    assert(statics.ds_text(0x610)==" +");
    assert(statics.ds_text(0x614)=="; ");
    assert(statics.ds_text(0x618)==" ");
    assert(statics.ds_text(0x61C)=="-");
    assert(statics.ds_text(0x620)=="/");
    assert(statics.ds_text(0x624)==".5");
    assert(statics.ds_text(0x6D98)=="; CHARGES: ");
    assert(statics.descriptor(0).count==4U);
    assert(statics.descriptor(0).contents_offset==0x6FC4U);
    assert(statics.descriptor(2).suffix_code==0x64U);
    assert(statics.suffix_7332(3)==0x7FU);
    assert(statics.suffix_7332(20)==0xFFU);
    assert(statics.charge_67ec(0)==0x0CU);
    assert(statics.charge_67ec(15)==0xFDU);
    assert(statics.initial_item(0,0)==71);
    assert(statics.initial_item(0,1)==-1);
    assert(statics.initial_item(2,0)==47);
    assert(statics.initial_item(2,4)==3);
    assert(statics.initial_nested_half_units(0,objects)==0U);
    assert(statics.initial_nested_half_units(1,objects)==5U);
    assert(statics.initial_nested_half_units(2,objects)==8U);
    assert(statics.initial_nested_half_units(9,objects)==4U);
    assert(statics.initial_nested_half_units(20,objects)==0U);
    bool rejected=false;
    try { (void)statics.ds_text(0x6200); } catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);
    rejected=false;
    try { (void)statics.initial_item(0,4); } catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);
    rejected=false;
    auto wrong=binary;wrong[0x75F24+0x624]='X';
    try { (void)Static(wrong); } catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
    rejected=false;
    try { (void)statics.initial_nested_half_units(1,{}); }
    catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
}
