#include "rloft_native/native_world_payload.hpp"
#include "rloft_native/native_world_fingerprints.hpp"
#include "rloft_native/native_sha256.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

using namespace rloft_native;

namespace {
void word(std::vector<std::uint8_t>& bytes,std::size_t at,unsigned value) {
    bytes.at(at)=static_cast<std::uint8_t>(value);
    bytes.at(at+1)=static_cast<std::uint8_t>(value>>8);
}
void dword(std::vector<std::uint8_t>& bytes,std::size_t at,unsigned value) {
    for(unsigned n=0;n<4;++n)
        bytes.at(at+n)=static_cast<std::uint8_t>(value>>(8U*n));
}
template<typename F> void must_reject(F call) {
    bool failed=false;
    try { call(); }
    catch(const std::exception&) { failed=true; }
    assert(failed);
}
std::vector<std::uint8_t> level_record() {
    constexpr unsigned b0=29, w=2, h=2;
    constexpr unsigned b1=b0+0x2A4+w*h*2;
    constexpr unsigned b2=b1+2*(2+10);
    constexpr unsigned b3=b2+11,b4=b3+11,b5=b4+11;
    constexpr unsigned b6=b5+50*26;
    std::vector<std::uint8_t> rec(b6+4+w*h+4+3,0);
    constexpr unsigned cuts[]{b0,b1,b2,b3,b4,b5,b6};
    for(unsigned n=0;n<7;++n)
        dword(rec,1U+4U*n,cuts[n]);
    word(rec,b0+1,w);
    word(rec,b0+3,h);
    word(rec,b0+0x28A,1); // family A
    word(rec,b0+0x28C,1); // family C
    word(rec,b0+0x28E,1); // family B

    word(rec,b0+0x2A4,0x2400);
    word(rec,b0+0x2A6,0x003F);
    word(rec,b0+0x2A8,0x8001);
    word(rec,b0+0x2AA,0x1234);

    word(rec,b1,1);
    word(rec,b1+12,1);
    rec[b1+2]=0x34;rec[b1+3]=0x56;
    rec[b1+14]=0x78;rec[b1+15]=0x9A;
    for(unsigned k=0;k<11;++k) {
        rec[b2+k]=static_cast<std::uint8_t>(10+k);
        rec[b3+k]=static_cast<std::uint8_t>(30+k);
        rec[b4+k]=static_cast<std::uint8_t>(50+k);
    }

    rec[b5]=1;rec[b5+1]=5;
    rec[b5+26]=2;rec[b5+27]=12;rec[b5+32]=0;
    word(rec,b6,w);word(rec,b6+2,h);
    rec[b6+4]=1;rec[b6+5]=2;rec[b6+6]=3;rec[b6+7]=4;
    dword(rec,b6+8,3);
    rec[b6+12]=0xFA;rec[b6+13]=0xFB;rec[b6+14]=0xFC;
    return rec;
}
std::vector<std::uint8_t> world_fixture() {
    std::vector<std::uint8_t> bytes(4+32*4,0);
    dword(bytes,0,32);
    const auto rec=level_record();
    for(unsigned id=0;id<32;++id) {
        dword(bytes,4+4*id,static_cast<unsigned>(bytes.size()));
        bytes.insert(bytes.end(),rec.begin(),rec.end());
    }
    return bytes;
}
void run() {
    const auto original=world_fixture();
    const auto world=decode_native_world_payload(original);
    assert(world.levels.size()==32);
    assert(world.total_cells==128);
    assert(world.total_control_records==64);
    assert(world.total_event_commands==96);
    const auto fingerprints=native_world_payload_fingerprints_json(world,original);
    assert(fingerprints.find("\"decoded_section_roundtrip_equal\":true")!=std::string::npos);
    assert(fingerprints.find("\"total_cells\":128")!=std::string::npos);
    assert(fingerprints.find("\"total_commands\":96")!=std::string::npos);
    assert(native_sha256_hex({})==
           "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    const std::vector<std::uint8_t> abc{'a','b','c'};
    assert(native_sha256_hex(abc)==
           "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
    const std::vector<std::uint8_t> million(1000000U,'a');
    assert(native_sha256_hex(million)==
           "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0");
    const auto& level=world.level(22);
    assert(level.id==22 && level.width==2 && level.height==2);
    assert(level.cell_words.size()==4);
    assert(level.cell(0,0)==0x2400);
    assert(level.cell(1,0)==0x003F);
    assert(level.cell(0,1)==0x8001);
    assert(level.cell(1,1)==0x1234);
    assert(level.tile_code(0,0)==0);
    assert(level.tile_code(1,0)==0x3F);
    assert(level.tile_code(1,1)==0x34);

    assert(level.controls.size()==2);
    assert(level.controls[0].column==0 &&
           level.controls[1].column==1);
    assert(level.controls[0].bytes[0]==0x34);
    assert(level.controls[1].bytes[1]==0x9A);
    assert(level.families[0].size()==1);
    assert(level.families[1].size()==1);
    assert(level.families[2].size()==1);
    assert(level.families[0][0][0]==10);
    assert(level.families[1][0][0]==30);
    assert(level.families[2][0][0]==50);
    assert(level.event_slots[0][0]==1);
    assert(level.event_slots[0][1]==5);
    assert(level.event_slots[1][0]==2);
    assert(level.event_slots[1][1]==12);
    assert(level.event_slots[1][6]==0);
    assert(level.automap.size()==4 && level.automap[3]==4);
    assert(level.annotation.size()==3 && level.annotation[2]==0xFC);

    auto perturbed=world;
    perturbed.levels[22].cell_words[0]^=0x40U;
    must_reject([&]{static_cast<void>(
        native_world_payload_fingerprints_json(perturbed,original));});
    perturbed=world;
    perturbed.levels[22].controls[0].bytes[0]^=1U;
    must_reject([&]{static_cast<void>(
        native_world_payload_fingerprints_json(perturbed,original));});
    perturbed=world;
    perturbed.levels[22].event_slots[1][1]^=1U;
    must_reject([&]{static_cast<void>(
        native_world_payload_fingerprints_json(perturbed,original));});
    perturbed=world;
    perturbed.levels[22].automap[0]^=1U;
    must_reject([&]{static_cast<void>(
        native_world_payload_fingerprints_json(perturbed,original));});
    auto mutable_world=clone_native_mutable_world(world);
    assert(mutable_world.levels.size()==32);
    mutable_world.level(22).cell_words[0]=0xFFFF;
    mutable_world.level(22).automap[0]=0xFE;
    assert(level.cell(0,0)==0x2400);
    assert(level.automap[0]==1);
    assert(mutable_world.level(22).cell_words[0]==0xFFFF);
    must_reject([&]{static_cast<void>(level.cell(2,0));});
    must_reject([&]{static_cast<void>(level.cell(0,2));});
    must_reject([&]{static_cast<void>(world.level(32));});
    must_reject([&]{static_cast<void>(mutable_world.level(32));});

    auto damaged=original;
    // Corrupt the first level's packed width; both source-first
    // certification and payload parsing must reject this alteration.
    damaged.at(132+29+1)=0;
    must_reject([&]{static_cast<void>(decode_native_world_payload(damaged));});
    damaged=original;
    // Truncate last record's annotation while leaving the directory intact.
    damaged.pop_back();
    must_reject([&]{static_cast<void>(decode_native_world_payload(damaged));});
}
}
int main() { run(); }
