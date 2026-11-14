#include "rloft/original_code2_handoff.hpp"
#include <array>
#include <cassert>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
using namespace rloft;

struct Host : OriginalResourceStartupPort {
    using Args=std::vector<std::uint32_t>;
    std::vector<std::pair<std::uint32_t,Args>> calls;
    std::map<std::uint32_t,std::uint32_t> words;
    std::map<std::uint32_t,std::uint8_t> bytes;
    std::uint32_t chars_handle{},secondary{},configuration{0x12345678};
    std::uint32_t call(std::uint32_t addr,
        std::initializer_list<std::uint32_t> args) override {
        calls.push_back({addr,Args(args)});
        if(addr==0x37248||addr==0x43338)return chars_handle;
        if(addr==0x24F14)return secondary;
        if(addr==0x372E4||addr==0x40584)return configuration;
        return 0;
    }
    std::uint8_t read8(std::uint32_t addr) override {return bytes[addr];}
    std::uint32_t read32(std::uint32_t addr) override {return words[addr];}
    std::uint8_t read_pointer_field8(std::uint32_t,std::uint32_t) override {return 1;}
    void write8(std::uint32_t addr,std::uint8_t value) override {bytes[addr]=value;}
    void write32(std::uint32_t addr,std::uint32_t value) override {words[addr]=value;}
    bool seen(std::uint32_t addr) const {
        for(const auto& c:calls)if(c.first==addr)return true;
        return false;
    }
};
static void expect_prefix(Host& h,GameId game) {
    h.words[game==GameId::Menzoberranzan?0x17DAC:0x26B6C]=0x200000;
}
static void test_menzo() {
    for(const std::uint32_t handle:{0U,1234U}) {
        Host h;h.chars_handle=handle;expect_prefix(h,GameId::Menzoberranzan);
        const auto prefix=execute_code2_resource_prefix(GameId::Menzoberranzan,h);
        assert(prefix.next_code_offset==0x23977 && !prefix.chars_examined);
        const auto end=execute_code2_media_handoff(GameId::Menzoberranzan,prefix,h);
        assert(end.source_next==0x23A20 && end.upcoming_call==0x204DC);
        assert(end.chars_handle==handle && end.has_chars==(handle!=0));
        assert(h.words.at(0x39D58)==0x12345678 && h.seen(0x37248));
        assert(h.seen(handle?0x1B68C:0x1DF34));
        assert(!h.seen(handle?0x1DF34:0x1B68C));
        assert(h.calls.back().first==(handle?0x23790U:0x1DF34U));
        const auto iter=h.calls.begin()+static_cast<std::ptrdiff_t>(2);
        assert(iter->first==0x31444);
    }
}
static void test_stone() {
    for(const std::uint32_t handle:{0U,0xBEEF1234U})
    for(const std::uint32_t second:{0U,1U}) {
        Host h;h.chars_handle=handle;h.secondary=second;
        expect_prefix(h,GameId::StoneProphet);
        const auto prefix=execute_code2_resource_prefix(GameId::StoneProphet,h);
        assert(prefix.chars_handle==handle && prefix.chars_present==(handle!=0));
        const auto end=execute_code2_media_handoff(GameId::StoneProphet,prefix,h);
        assert(end.source_next==0x2D54D &&
               end.upcoming_call==(handle?0x4191CU:0x46600U));
        assert(end.has_chars==(handle!=0));
        assert(end.secondary_initializer_ran==(handle==0 && second!=0));
        assert(h.words.at(0x24E54)==1 && h.words.at(0x44764)==0x12345678);
        assert(h.seen(0x3C914) && h.seen(0x6E60));
        assert(h.seen(0x24F14)==(handle==0));
        assert(h.seen(0x216A0)==(handle!=0));
        assert(h.seen(0x25EA8)==(handle==0 && second!=0));
        bool setup3=false,setup83=false;
        for(const auto& c:h.calls)if(c.first==0x3C914) {
            setup3=(c.second==Host::Args{3});setup83=(c.second==Host::Args{0x83});
        }
        assert(setup3==(handle==0) && setup83==(handle!=0));
    }
}
static std::vector<std::uint8_t> fake_res0(std::uint32_t count) {
    std::vector<std::uint8_t> b(4+count*4+count*4,0);
    const auto set32=[&](std::size_t p,std::uint32_t v) {
        for(int i=0;i<4;++i)b[p+i]=std::uint8_t(v>>(i*8));
    };
    set32(0,count);
    for(std::uint32_t i=0;i<count;++i){
        const auto off=4+count*4+4*i;
        set32(4+4*i,off);
        b[off]='E';b[off+1]='H';b[off+2]=2;
    }
    return b;
}
static void test_media() {
    for(const auto& item:std::array<std::pair<GameId,unsigned>,3>{{
       {GameId::StrahdsPossession,141U},
       {GameId::Menzoberranzan,175U},
       {GameId::StoneProphet,174U}}}) {
        unsigned reads=0,chars_reads=0;
        bool has_chars=false;
        OriginalNativeMediaSession media(item.first,
          [&](std::string_view name) {
              ++reads;assert(name=="RES0");return fake_res0(item.second);
          },
          [&]() -> std::optional<std::vector<std::uint8_t>> {
              ++chars_reads;
              if(!has_chars)return std::nullopt;
              return std::vector<std::uint8_t>{1,2,3}; // opaque, not Strahd CHARS codec
          });
        assert(media.game()==item.first);
        const auto& a=media.res(0);
        const auto& b=media.res(0);
        assert(&a==&b && reads==1 && a.size()==4+item.second*8);
        assert(!media.chars());has_chars=true;
        assert(media.chars()->size()==3 && chars_reads==2);
        bool rejected=false;
        try {(void)media.res(13);}catch(const std::out_of_range&) {rejected=true;}
        assert(rejected && reads==1);
    }
    bool rejected=false;
    try {OriginalNativeMediaSession bad(GameId::StoneProphet,
        [](std::string_view){return fake_res0(175);},
        []{return std::optional<std::vector<std::uint8_t>>{};});
        (void)bad.res(0);
    } catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
}
static void test_failure() {
    Host h;OriginalStartupCheckpoint wrong{0x2D4B5,false,false,false};
    bool rejected=false;
    try {(void)execute_code2_media_handoff(GameId::StoneProphet,wrong,h);}
    catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
    rejected=false;
    try {(void)execute_code2_media_handoff(GameId::StrahdsPossession,wrong,h);}
    catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
    assert(!verify_code2_media_handoff_witness(GameId::Menzoberranzan,{}));
    assert(!verify_code2_media_handoff_witness(GameId::StoneProphet,{}));
}

static void test_gameplay_boundary() {
    for(const std::uint32_t handle : std::array<std::uint32_t,2>{{0U,0x12345678U}}) {
        Host h; h.bytes[0xBEE4]=6;
        const Code2MediaHandoff prior{0x23A20,handle,handle!=0,false,0x204DC};
        const auto b=execute_code2_gameplay_boundary(
            GameId::Menzoberranzan,prior,h,0xCAFE);
        assert(b.resource_initializer_returned && b.source_boundary==0x23A93);
        assert(!b.pending_original_call && h.words.at(0xF570)==0x150000);
        assert(h.words.at(0x12EAC)==1 && h.seen(0x204DC));
        assert(h.seen(0x1E474)==(handle!=0));
        assert(h.seen(0x1D6EC)==(handle!=0));
        if(handle) {
            assert(h.bytes.at(0x124E5)==7);
            assert(h.calls[h.calls.size()-1].first==0x1D6EC);
            assert(h.calls.back().second==Host::Args({0xCAFE,0xD4}));
        }
    }
    for(const std::uint32_t handle : std::array<std::uint32_t,2>{{0U,99U}}) {
        Host h; h.words[0x24E54]=1;h.words[0x15C20]=0xBEEF;
        h.bytes[0x44FDD]=2;h.bytes[0x1A5A8]=4;
        const Code2MediaHandoff prior{
            0x2D54D,handle,handle!=0,false,handle?0x4191CU:0x46600U};
        const auto b=execute_code2_gameplay_boundary(
            GameId::StoneProphet,prior,h,0);
        assert(b.opening_ui_drawn==(handle==0));
        assert(b.source_boundary==(handle?0x2D5C5U:0x2D6CCU));
        assert(b.pending_original_call==(handle?0x25468U:0U));
        assert(h.seen(0x46600)==(handle==0));
        assert(h.seen(0x4191C) && h.seen(0x3C9D4) && h.seen(0x3C9EC));
        assert(h.words.at(0x1DC34)==0x70000);
        if(!handle) {
            for(const auto& c:h.calls)if(c.first==0x46600)
                assert(c.second==Host::Args({0x1A5AC,0,6,0xBEEF,0x100}));
        }
    }
    assert(!verify_code2_gameplay_boundary_witness(GameId::Menzoberranzan,{}));
    assert(!verify_code2_gameplay_boundary_witness(GameId::StoneProphet,{}));
}

int main() {test_menzo();test_stone();test_media();test_failure();test_gameplay_boundary();}
