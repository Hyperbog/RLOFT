#include "rloft/original_application_main.hpp"
#include "rloft/original_bound_image.hpp"
#include <array>
#include <cassert>
#include <initializer_list>
#include <fstream>
#include <iterator>
#include <map>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

using namespace rloft;

struct Host final : OriginalApplicationBoundary {
    std::vector<std::uint32_t> noargs;
    std::vector<std::pair<std::uint32_t,std::int32_t>> witharg;
    std::vector<std::string> getenv_names;
    std::vector<std::string> atoi_values;
    std::map<std::string,std::string> environment;
    std::int32_t call0(std::uint32_t original) override {
        noargs.push_back(original);
        return original==0x6134 ? 77 : 0;
    }
    std::int32_t call1(std::uint32_t original, std::int32_t arg) override {
        witharg.emplace_back(original,arg);
        return arg+11;
    }
    std::optional<std::string_view> getenv_call(std::uint32_t, std::string_view name) override {
        getenv_names.emplace_back(name);
        auto it=environment.find(std::string(name));
        if(it==environment.end())return std::nullopt;
        return std::string_view(it->second);
    }
    std::int32_t atoi_call(std::uint32_t, std::string_view value) override {
        atoi_values.emplace_back(value);
        return std::stoi(std::string(value));
    }
};

static void test_code1() {
    Host menzo;
    const auto a=execute_original_code1_main(GameId::Menzoberranzan,menzo);
    assert(a.kind==OriginalCode1Outcome::Kind::TailExit && a.value==10 && a.target==0xCC8E);
    assert((menzo.noargs==std::vector<std::uint32_t>{
        0x1044,0xC24,0x4280,0x6E1C,0x3D54,0x1198}));
    assert(menzo.witharg.empty());
    Host stone;
    const auto b=execute_original_code1_main(GameId::StoneProphet,stone);
    assert(b.kind==OriginalCode1Outcome::Kind::Returned && b.value==88);
    assert((stone.noargs==std::vector<std::uint32_t>{0x680C,0x6134}));
    assert((stone.witharg==std::vector<std::pair<std::uint32_t,std::int32_t>>{
        {0x6BB0,77}}));
    bool threw=false;
    try {(void)execute_original_code1_main(GameId::StrahdsPossession,stone);}
    catch(const std::invalid_argument&) {threw=true;}
    assert(threw);
}
static void test_code2() {
    for(auto game:{GameId::Menzoberranzan,GameId::StoneProphet}) {
        Host empty;
        const auto defaults=execute_original_code2_env_prefix(game,empty);
        assert(!defaults.gate_present);
        assert((empty.getenv_names==std::vector<std::string>{"ALSIU"}));
        assert(empty.atoi_values.empty());
        if(game==GameId::Menzoberranzan)
            assert(defaults.ebx==4 && defaults.esi==6 && defaults.edi==180 && defaults.local==1);
        else
            assert(defaults.ebx==27 && defaults.esi==232 && defaults.edi==41 && defaults.local==45);
        Host present;
        present.environment={{"ALSIU","11"},{"KHSSDKJ","22"},
                             {"CANLW","33"},{"SEPVON","44"}};
        const auto result=execute_original_code2_env_prefix(game,present);
        assert(result.gate_present && result.ebx==33 && result.esi==22);
        assert((present.getenv_names==std::vector<std::string>{
            "ALSIU","ALSIU","KHSSDKJ","CANLW","SEPVON"}));
        assert((present.atoi_values==std::vector<std::string>{"11","22","33","44"}));
        if(game==GameId::Menzoberranzan)
            assert(result.local==11 && result.edi==44 && result.continuation==0x23B7D);
        else
            assert(result.edi==11 && result.local==44 && result.continuation==0x2DD95);
        present.environment.erase("SEPVON");
        bool threw=false;
        try {(void)execute_original_code2_env_prefix(game,present);}
        catch(const std::runtime_error&) {threw=true;}
        assert(threw);
    }
}

static void set_call(std::vector<std::uint8_t>& code,
                     std::uint32_t at, std::uint32_t to) {
    code.at(at)=0xE8;
    const auto relative=static_cast<std::uint32_t>(to-(at+5U));
    for(int i=0;i<4;++i)code.at(at+1+i)=static_cast<std::uint8_t>(relative>>(8*i));
}
static void set_bytes(std::vector<std::uint8_t>& code,
                      std::uint32_t at, std::initializer_list<std::uint8_t> seq) {
    for(auto value:seq)code.at(at++)=value;
}
static void test_original_opcodes() {
    for(auto game:{GameId::Menzoberranzan,GameId::StoneProphet}) {
        std::vector<std::uint8_t> code(0x55000);
        std::vector<std::uint8_t> data(0x2000);
        const auto code1=original_main(game,OriginalAppModule::Code1);
        set_call(code,code1.crt_call,code1.main_offset);
        set_bytes(code,code1.crt_call+5,{0x83,0xC4,0x08,0x50});
        if(game==GameId::Menzoberranzan) {
            set_bytes(code,0x127C,{0x55,0x89,0xE5});
            for(const auto& pair : std::array<std::pair<std::uint32_t,std::uint32_t>,6>{{
                {0x127F,0x1044},{0x1284,0x0C24},{0x1289,0x4280},
                {0x128E,0x6E1C},{0x1293,0x3D54},{0x1298,0x1198}}})
                set_call(code,pair.first,pair.second);
            set_bytes(code,0x129D,{0x6A,0x0A,0xE9,0xEA,0xB9,0,0});
        } else {
            set_bytes(code,0x6C58,{0x53,0x56,0x57,0x55,0x89,0xE5,
                0x81,0xEC,0,0,0,0});
            set_call(code,0x6C64,0x680C);
            set_call(code,0x6C69,0x6134);
            set_bytes(code,0x6C6E,{0x50});
            set_call(code,0x6C6F,0x6BB0);
            set_bytes(code,0x6C74,{0x83,0xC4,0x04,0x5D,0x5F,0x5E,0x5B,0xC3});
        }
        assert(verify_original_application_main(game,OriginalAppModule::Code1,code,data));
        code.at(code1.crt_call+1)^=1;
        assert(!verify_original_application_main(game,OriginalAppModule::Code1,code,data));
        code.at(code1.crt_call+1)^=1;

        const auto code2=original_main(game,OriginalAppModule::Code2);
        set_call(code,code2.crt_call,code2.main_offset);
        set_bytes(code,code2.crt_call+5,{0x83,0xC4,0x08,0x50});
        set_bytes(code,code2.main_offset,{0x53,0x56,0x57,0x55,0x89,0xE5,
            0x83,0xEC,0x04});
        const bool menzo=game==GameId::Menzoberranzan;
        if(menzo) {
            set_bytes(code,0x23AF5,{0xC7,0x45,0xFC,1,0,0,0,
                0xBE,6,0,0,0,0x68,0x74,0x10,0,0,
                0xBB,4,0,0,0,0xBF,0xB4,0,0,0});
        } else {
            set_bytes(code,0x2DD0D,{0xBF,0x29,0,0,0,0xBE,0xE8,0,0,0,
                0x68,0xF4,0x12,0,0,0xBB,0x1B,0,0,0,
                0xC7,0x45,0xFC,0x2D,0,0,0});
        }
        const auto getenv_target=menzo?0x3CA41U:0x4A920U;
        const auto atoi_target=menzo?0x3CA96U:0x4A97BU;
        const std::array<std::uint32_t,5> get_sites=menzo?
            std::array<std::uint32_t,5>{{0x23B10,0x23B21,0x23B3A,0x23B52,0x23B6A}}:
            std::array<std::uint32_t,5>{{0x2DD28,0x2DD39,0x2DD51,0x2DD69,0x2DD81}};
        const std::array<std::uint32_t,4> parse_sites=menzo?
            std::array<std::uint32_t,4>{{0x23B2A,0x23B43,0x23B5B,0x23B73}}:
            std::array<std::uint32_t,4>{{0x2DD42,0x2DD5A,0x2DD72,0x2DD8A}};
        for(auto site:get_sites)set_call(code,site,getenv_target);
        for(auto site:parse_sites)set_call(code,site,atoi_target);
        set_bytes(code,get_sites[0]+8,{0x85,0xC0,0x74,0x61});
        const std::array<std::string_view,4> names{{"ALSIU","KHSSDKJ","CANLW","SEPVON"}};
        const std::array<std::uint32_t,4> offsets=menzo?
            std::array<std::uint32_t,4>{{0x1074,0x107C,0x1084,0x108C}}:
            std::array<std::uint32_t,4>{{0x12F4,0x12FC,0x1304,0x130C}};
        for(std::size_t i=0;i<names.size();++i){
            for(std::size_t j=0;j<names[i].size();++j)
                data.at(offsets[i]+j)=static_cast<std::uint8_t>(names[i][j]);
        }
        assert(verify_original_application_main(game,OriginalAppModule::Code2,code,data));
        data.at(offsets[2])^=1;
        assert(!verify_original_application_main(game,OriginalAppModule::Code2,code,data));
        data.at(offsets[2])^=1;
        code.at(get_sites[1])=0x90;
        assert(!verify_original_application_main(game,OriginalAppModule::Code2,code,data));
    }
}

static std::vector<std::uint8_t> read(const char* name) {
    std::ifstream f(name,std::ios::binary);
    if(!f)throw std::runtime_error(std::string("cannot read original: ")+name);
    return {std::istreambuf_iterator<char>(f),std::istreambuf_iterator<char>()};
}
int main(int argc,char** argv) {
    test_code1(); test_code2(); test_original_opcodes();
    if(argc==4) {
        const auto game=game_id_from_key(argv[1]);
        if(!game)throw std::invalid_argument("use strahd|menzo|stone-prophet");
        for(int i=0;i<2;++i) {
            const auto b=read(argv[i+2]);
            const OriginalBoundImage image(b);
            const auto code=image.object_bytes(0);
            const auto data=image.object_bytes(1);
            if(!verify_original_application_main(*game,
                 i==0?OriginalAppModule::Code1:OriginalAppModule::Code2,code,data))
                throw std::runtime_error("original application call graph mismatch");
        }
    } else if(argc!=1) {
        throw std::invalid_argument("usage: original_application_main_tests [game CODE.1 CODE.2]");
    }
    return 0;
}
