#include "rloft_native/native_world_system.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

using namespace rloft_native;
namespace {
void word(std::vector<std::uint8_t>& b,std::size_t pos,unsigned value) {
    b.at(pos)=std::uint8_t(value);
    b.at(pos+1)=std::uint8_t(value>>8);
}
void dword(std::vector<std::uint8_t>& b,std::size_t pos,unsigned value) {
    for(unsigned n=0;n<4;++n)b.at(pos+n)=std::uint8_t(value>>(8*n));
}
template<typename Fn> void rejects(Fn call) {
    bool failed=false;
    try{call();}catch(const std::exception&){failed=true;}
    assert(failed);
}
std::vector<std::uint8_t> level_record() {
    constexpr unsigned width=2,height=2;
    constexpr unsigned b0=29;
    constexpr unsigned b1=b0+0x2a4+width*height*2;
    constexpr unsigned b2=b1+2*(2+10);
    constexpr unsigned b3=b2+11,b4=b3+11,b5=b4+11;
    constexpr unsigned b6=b5+0x514;
    std::vector<std::uint8_t> record(b6+4+width*height+4,0);
    for(unsigned i=0;i<7;++i) {
        constexpr unsigned offsets[]{b0,b1,b2,b3,b4,b5,b6};
        dword(record,1+4*i,offsets[i]);
    }
    word(record,b0+1,width);word(record,b0+3,height);
    word(record,b0+0x28a,1); // family A
    word(record,b0+0x28c,1); // family C
    word(record,b0+0x28e,1); // family B
    word(record,b1,1);
    word(record,b1+12,1);
    record[b5]=1;record[b5+1]=5;
    word(record,b6,width);word(record,b6+2,height);
    dword(record,b6+4+width*height,0);
    return record;
}
std::vector<std::uint8_t> world_fixture() {
    std::vector<std::uint8_t> data(4+32*4,0);
    dword(data,0,32);
    auto record=level_record();
    for(unsigned level=0;level<32;++level) {
        dword(data,4+4*level,static_cast<unsigned>(data.size()));
        data.insert(data.end(),record.begin(),record.end());
    }
    return data;
}
std::vector<std::uint8_t> sintab_fixture() {
    constexpr unsigned count=127;
    constexpr unsigned block_bytes=0x205;
    constexpr unsigned start=0x1F42;
    std::vector<std::uint8_t> data(start+count*(2+block_bytes)+512,0);
    dword(data,0x1B38,127);
    dword(data,0x1B3C,508);
    dword(data,0x1B40,128);
    dword(data,0x1F3E,count*block_bytes);
    unsigned cursor=start;
    for(unsigned i=0;i<count;++i) {
        word(data,cursor,block_bytes);
        cursor+=2;
        for(unsigned t=0;t<129;++t)
            word(data,cursor+2*t,0x204);
        data[cursor+block_bytes-1]=0xc3;
        cursor+=block_bytes;
    }
    return data;
}
void run() {
    const auto levels=world_fixture();
    const auto decoded=decode_native_levels(levels);
    assert(decoded.levels.size()==32);
    assert(decoded.map_cells==128 && decoded.control_records==64);
    assert(decoded.active_event_slots==32);
    assert(decoded.active_event_commands==32);
    assert(decoded.levels.at(22).width==2);
    assert(decoded.levels.at(22).height==2);
    assert(decoded.levels.at(22).family_a==1);
    assert(decoded.levels.at(22).family_b==1);
    assert(decoded.levels.at(22).family_c==1);
    auto broken=levels;
    dword(broken,4,0x85);
    rejects([&]{static_cast<void>(decode_native_levels(broken));});
    broken=levels;
    broken[4+32*4+1+4*5]=0; // perturb script boundary while retaining file
    rejects([&]{static_cast<void>(decode_native_levels(broken));});
    broken=levels;
    broken[4+32*4+29+1]=0; // zero width
    rejects([&]{static_cast<void>(decode_native_levels(broken));});
    broken=levels;
    broken[4+32*4+29+0x2a4+8]=255; // negative 10-byte list count
    rejects([&]{static_cast<void>(decode_native_levels(broken));});

    const auto sintab=sintab_fixture();
    const auto proj=decode_native_sintab(sintab);
    assert(proj.scale_count==127 && proj.scales.size()==127);
    assert(proj.lookup_max==508 && proj.lookup_aux==128);
    assert(proj.actual_scale_bytes==127*0x205);
    assert(proj.declared_scale_bytes==proj.actual_scale_bytes);
    assert(proj.scales[0].initial_code_offset==0x204);
    broken=sintab;
    word(broken,0x1F42+2,0x205); // out of bounds generated code patch
    rejects([&]{static_cast<void>(decode_native_sintab(broken));});
    broken=sintab;
    broken.back()=1;
    rejects([&]{static_cast<void>(decode_native_sintab(broken));});
    broken=sintab;
    dword(broken,0x1B38,126);
    rejects([&]{static_cast<void>(decode_native_sintab(broken));});
    broken=sintab;
    broken.resize(0x1f42);
    rejects([&]{static_cast<void>(decode_native_sintab(broken));});
    NativeWorldSystem both{decoded,proj};
    const auto json=native_world_system_json(both);
    assert(json.find("\"level_count\":32")!=std::string::npos);
    assert(json.find("\"sintab_scales\":127")!=std::string::npos);
}
}
int main(){run();}
