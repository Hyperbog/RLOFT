#include "rloft/original_code2_global_state.hpp"
#include "rloft/original_bound_image.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

using namespace rloft;

struct Memory final : OriginalResourceStartupPort {
    using Args=std::vector<std::uint32_t>;
    std::map<std::uint32_t,std::uint8_t> bytes;
    std::map<std::uint32_t,std::uint32_t> words;
    std::vector<std::pair<std::uint32_t,Args>> calls;
    std::vector<std::uint32_t> reads;
    std::uint32_t call(std::uint32_t target,
                       std::initializer_list<std::uint32_t> args) override {
        calls.emplace_back(target,Args(args));
        return 0;
    }
    std::uint8_t read8(std::uint32_t addr) override {return bytes.at(addr);}
    std::uint32_t read32(std::uint32_t addr) override {
        reads.push_back(addr);return words.at(addr);
    }
    std::uint8_t read_pointer_field8(std::uint32_t,std::uint32_t) override {return 0;}
    void write8(std::uint32_t addr,std::uint8_t value) override {bytes[addr]=value;}
    void write32(std::uint32_t addr,std::uint32_t value) override {words[addr]=value;}
    std::uint16_t word16(std::uint32_t addr) const {
        return std::uint16_t(bytes.at(addr)|(unsigned(bytes.at(addr+1))<<8));
    }
};

static void expect_call(const Memory& m,std::size_t n,std::uint32_t target,
                        Memory::Args args) {
    assert(m.calls.at(n).first==target && m.calls.at(n).second==args);
}

static void global_state_test() {
    OriginalCode2Environment env;
    env.esi=0x12345;env.ebx=0x23456;env.edi=0x34567;env.local=0x45678;
    for(const auto game:{GameId::Menzoberranzan,GameId::StoneProphet}) {
        const bool menzo=game==GameId::Menzoberranzan;
        Memory m;
        m.words[menzo?0x3A660:0x4812C]=0xFFFF0000;
        m.words[menzo?0x3A658:0x4815C]=0x0002FFFF;
        const auto result=execute_code2_post_environment(game,env,m);
        assert(m.calls.size()==3);
        if(menzo) {
            assert(result.resource_entry==0x23934 && result.final_source_offset==0x23CD8);
            assert(m.word16(0x8FC4)==0x8000 && m.word16(0x8FC0)==0x8000);
            assert(m.word16(0x8FC6)==0x2345 && m.word16(0x8FC2)==0x3456);
            assert(m.words.at(0x8FC8)==0x34567 && m.words.at(0x8FBC)==0xFFFFFFFF);
            assert(m.bytes.at(0xF57C)==1 && m.words.at(0x15468)==1);
            assert(m.words.at(0x19454)==0xFFFF0000 && m.words.at(0x19450)==0x1FFFF);
            expect_call(m,0,0x23934,{0x45678});expect_call(m,1,0x23500,{});
            expect_call(m,2,0x23A94,{0xFFFFFFFF});
            assert((m.reads==std::vector<std::uint32_t>{0x3A660,0x3A658}));
            assert(m.words.at(0x3A5E4)==0 && m.words.at(0x39D60)==0);
            assert(m.words.at(0x8F84)==0 && m.words.at(0x8FA0)==0);
        } else {
            assert(result.resource_entry==0x2D45C && result.final_source_offset==0x2DEF0);
            assert(m.word16(0x16ECC)==0x8000 && m.word16(0x16EC8)==0x8000);
            assert(m.word16(0x16ECE)==0x2345 && m.word16(0x16ECA)==0x3456);
            assert(m.words.at(0x16ED0)==0x45678 && m.words.at(0x16EC4)==0xFFFFFFFF);
            assert(m.bytes.at(0x1DC40)==1 && m.words.at(0x24E3C)==77);
            assert(m.words.at(0x281BC)==0xFFFF0000 && m.words.at(0x281B8)==0x1FFFF);
            expect_call(m,0,0x2D45C,{0x34567});expect_call(m,1,0x2D010,{});
            expect_call(m,2,0x2D70C,{0xFFFFFFFF});
            assert((m.reads==std::vector<std::uint32_t>{0x4812C,0x4815C}));
            assert(m.words.at(0x4478C)==0 && m.words.at(0x4477C)==0);
            assert(m.words.at(0x16E8C)==0 && m.words.at(0x16EA8)==0);
        }
    }
    bool rejected=false;Memory m;
    try{(void)execute_code2_post_environment(GameId::StrahdsPossession,env,m);}
    catch(const std::invalid_argument&){rejected=true;}
    assert(rejected);
    assert(!verify_code2_global_state_witness(GameId::StrahdsPossession,{}));
    assert(!verify_code2_global_state_witness(GameId::StoneProphet,{0xC7,0x05}));
}

static void res_test() {
    const auto make=[](unsigned n){
        std::vector<std::uint8_t> b(4+4*n+2*n,0);
        for(unsigned i=0;i<4;++i)b[i]=std::uint8_t(n>>(i*8));
        for(unsigned i=0;i<n;++i) {
            const auto v=4+4*n+2*i;
            for(unsigned j=0;j<4;++j)b[4+4*i+j]=std::uint8_t(v>>(j*8));
            b[v]='E';b[v+1]='H';
        }
        return b;
    };
    for(auto game:{GameId::StrahdsPossession,GameId::Menzoberranzan,
                   GameId::StoneProphet}) {
        const unsigned expected=game==GameId::StrahdsPossession?141U:
                                game==GameId::Menzoberranzan?175U:174U;
        std::string requested;
        auto reader=[&](std::string_view s){requested=std::string(s);return make(expected);};
        auto loaded=load_shared_original_res(game,0,reader);
        assert(requested=="RES0" && inspect_original_res0(loaded).entries==expected);
        loaded=load_shared_original_res(game,11,reader);
        assert(requested=="RES11" && loaded==make(expected));
        bool threw=false;
        try{(void)load_shared_original_res(game,0,[&](std::string_view){return make(140);});}
        catch(const std::invalid_argument&){threw=true;}assert(threw);
        threw=false;
        try{(void)load_shared_original_res(game,13,reader);}
        catch(const std::out_of_range&){threw=true;}assert(threw);
    }
}
static std::vector<std::uint8_t> read(const char* path) {
    std::ifstream f(path,std::ios::binary);
    if(!f)throw std::runtime_error(std::string("missing file: ")+path);
    return {std::istreambuf_iterator<char>(f),std::istreambuf_iterator<char>()};
}
int main(int argc,char** argv) {
    global_state_test();res_test();
    if(argc==4) {
        auto id=game_id_from_key(argv[1]);
        if(!id || *id==GameId::StrahdsPossession)
            throw std::invalid_argument("expected menzo|stone-prophet");
        const OriginalBoundImage img(read(argv[2]));
        auto code=img.object_bytes(0);
        if(!verify_code2_global_state_witness(*id,code))
            throw std::runtime_error("original CODE.2 global-state witness mismatch");
        code[(*id==GameId::Menzoberranzan)?0x23B7D:0x2DD95]^=1;
        assert(!verify_code2_global_state_witness(*id,code));
        const auto media=load_shared_original_res(*id,0,
            [&](std::string_view name){assert(name=="RES0");return read(argv[3]);});
        assert(inspect_original_res0(media).entries==
               (*id==GameId::Menzoberranzan?175U:174U));
    } else if(argc!=1) {
        throw std::invalid_argument(
            "usage: original_code2_global_state_tests [menzo|stone-prophet CODE.2 RES0]");
    }
    std::cout<<"original_code2_global_state_tests: PASS\n";
}
