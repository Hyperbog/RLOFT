#include "rloft/original_resource_startup.hpp"
#include "rloft/original_bound_image.hpp"
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace rloft;

struct Host final : OriginalResourceStartupPort {
    using Args = std::vector<std::uint32_t>;
    std::vector<std::pair<std::uint32_t,Args>> calls;
    std::map<std::uint32_t,std::uint8_t> bytes;
    std::map<std::uint32_t,std::uint32_t> words;
    std::uint32_t pointer{0x900000};
    std::uint8_t initialized{};
    std::uint32_t chars_handle{};
    std::uint32_t call(std::uint32_t address,
                       std::initializer_list<std::uint32_t> args) override {
        calls.emplace_back(address,Args(args));
        return address == 0x43338 ? chars_handle : 0;
    }
    std::uint8_t read8(std::uint32_t address) override {return bytes.at(address);}
    std::uint32_t read32(std::uint32_t address) override {return words.at(address);}
    std::uint8_t read_pointer_field8(std::uint32_t p,
                                     std::uint32_t field) override {
        assert(p==pointer && field==0x72);
        return initialized;
    }
    void write8(std::uint32_t a, std::uint8_t value) override {bytes[a]=value;}
    void write32(std::uint32_t a, std::uint32_t value) override {words[a]=value;}
};

static void call_at(const Host& h,std::size_t i,std::uint32_t addr,
                    std::vector<std::uint32_t> args) {
    assert(i<h.calls.size());
    assert(h.calls[i].first==addr && h.calls[i].second==args);
}
static void code1_test() {
    Host menzo;
    auto mc=execute_code1_media_prefix(GameId::Menzoberranzan,menzo);
    assert(mc.next_code_offset==0x10CC && menzo.calls.size()==9);
    call_at(menzo,0,0x6B7C,{0x1F0,11,0xC350,0x186A0});
    call_at(menzo,1,0x6D94,{0x1F4,1,254});
    call_at(menzo,2,0x5204,{3});
    call_at(menzo,3,0xA840,{0xB90,0,0x300});
    call_at(menzo,4,0x5340,{0xB90});
    call_at(menzo,5,0xFE8,{});
    call_at(menzo,6,0x39F0,{});
    call_at(menzo,7,0xE98,{});
    call_at(menzo,8,0xF48,{});
    for(unsigned i=0;i<256;++i)
        assert(menzo.bytes.at(0xE90+i)==(i==0x17 ? 1 : i));
    Host stone;
    auto sc=execute_code1_media_prefix(GameId::StoneProphet,stone);
    assert(sc.next_code_offset==0x68B6 && stone.calls.size()==9);
    call_at(stone,0,0xC480,{0x84,11,0xEA60,0x186A0});
    call_at(stone,1,0xC69C,{0x88,1,254});
    call_at(stone,2,0x677C,{});
    call_at(stone,3,0x65C0,{});
    call_at(stone,4,0x8A94,{3});
    call_at(stone,5,0xF790,{0x6EB8,0,2,100,256});
    call_at(stone,6,0x8BD8,{0x6EB8});
    call_at(stone,7,0xCACC,{});
    call_at(stone,8,0x63A8,{});
    for(unsigned i=0;i<256;++i)assert(stone.bytes.at(0x68B8+i)==i);
    bool rejected=false;
    try {(void)execute_code1_media_prefix(GameId::StrahdsPossession,stone);}
    catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
}
static void stone_interaction_test() {
    Host host;
    host.bytes[0x8750]=0;
    const auto result=execute_stone_code1_interaction_prefix(host);
    assert(result.next_code_offset==0x618B && host.words.at(0x8BC)==0);
    assert(host.bytes.at(0x89FE)==1 && host.calls.size()==4);
    call_at(host,0,0x85D8,{});
    call_at(host,1,0xF790,{0x6EB8,0,2,100,256});
    call_at(host,2,0x8BD8,{0x6EB8});
    call_at(host,3,0x7478,{});
    host.bytes[0x8750]=1;
    (void)execute_stone_code1_interaction_prefix(host);
    assert(host.bytes.at(0x89FE)==0);
}
static void code2_test() {
    for(auto game:{GameId::Menzoberranzan,GameId::StoneProphet}) {
        for(const auto ready:{0,1}) {
            Host host; host.initialized=std::uint8_t(ready);
            host.words[game==GameId::Menzoberranzan?0x17DAC:0x26B6C]=host.pointer;
            host.chars_handle=9;
            auto stage=execute_code2_resource_prefix(game,host);
            assert(stage.optional_initialization_called==(ready==0));
            if(game==GameId::Menzoberranzan) {
                assert(!stage.chars_examined && stage.next_code_offset==0x23977);
                call_at(host,0,0x3532C,{0x1054,11,0xEA60,0x186A0});
                call_at(host,host.calls.size()-1,0x35544,{0x1058,1,254});
                assert(host.calls.size()==(ready?2U:3U));
                if(!ready) call_at(host,1,0x237EC,{});
            } else {
                assert(stage.chars_examined && stage.chars_present &&
                       stage.next_code_offset==0x2D4E6);
                call_at(host,0,0x412D0,{0x12D0,11,0xF230,0x186A0});
                call_at(host,host.calls.size()-2,0x414EC,{0x12D4,1,254});
                call_at(host,host.calls.size()-1,0x43338,{0x12E8,0});
                if(!ready) call_at(host,1,0x2D310,{});
                host.chars_handle=0;
                const auto absent=execute_code2_resource_prefix(game,host);
                assert(absent.chars_examined && !absent.chars_present &&
                       absent.next_code_offset==0x2D4B5);
            }
        }
    }
}
static std::vector<std::uint8_t> read_file(const char* name) {
    std::ifstream f(name,std::ios::binary);
    if(!f) throw std::runtime_error(std::string("unavailable original: ")+name);
    return {std::istreambuf_iterator<char>(f),std::istreambuf_iterator<char>()};
}
static void res_test() {
    std::vector<std::uint8_t> res{2,0,0,0,12,0,0,0,16,0,0,0,
                                 'E','H',2,0,'E','H',3,0};
    assert(inspect_original_res0(res).entries==2);
    auto bad=res; bad[16]='X';
    bool rejected=false;
    try {(void)inspect_original_res0(bad);}
    catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
    bad=res;bad[8]=5;rejected=false;
    try {(void)inspect_original_res0(bad);}
    catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
}
int main(int argc,char** argv) {
    code1_test(); stone_interaction_test(); code2_test(); res_test();
    if(argc==5) {
        const auto id=game_id_from_key(argv[1]);
        if(!id || *id==GameId::StrahdsPossession)
            throw std::invalid_argument("provide menzo|stone-prophet");
        for(int i=0;i<2;++i) {
            const auto original=read_file(argv[i+2]);
            const OriginalBoundImage image(original);
            const auto code=image.object_bytes(0);
            const auto data=image.object_bytes(1);
            if(!verify_original_resource_startup_witness(*id,i==1,code,data))
                throw std::runtime_error("original startup opcode witness mismatch");
        }
        const auto media=inspect_original_res0(read_file(argv[4]));
        const auto expected=*id==GameId::Menzoberranzan?175U:174U;
        if(media.entries!=expected)throw std::runtime_error("original RES0 entry mismatch");
    } else if(argc!=1) {
        throw std::invalid_argument(
            "usage: original_resource_startup_tests [menzo|stone-prophet CODE.1 CODE.2 RES0]");
    }
    std::cout<<"original_resource_startup_tests: PASS\n";
}
