// Source-executed original LEVELS.DAT grid -> CODE.2 334C4 view preparation.
// Stops at the authentic 32954 camera/ray sweep callback. NOT a game frame.
#include "../../Code2/Code2.cpp"
#include "../../Code2/graphics/original_levels_grid.hpp"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
namespace {
using u8=std::uint8_t;using u16=std::uint16_t;using u32=std::uint32_t;
#define CHECK(x) do {if(!(x))throw std::runtime_error("334C4 original world: " #x);}while(false)
struct Probe {
    std::vector<u8> guest=std::vector<u8>(8u*1024u*1024u,0u);
    code2::Machine machine{};
    unsigned ray_handoffs=0;
    static Probe* current;
    static void stop_at_ray_sweep() noexcept {
        if(current)++current->ray_handoffs;
    }
    static u32 synthetic_fixed_mul(u32,u32) noexcept {return 0u;}
    Probe(){CHECK(current==nullptr);current=this;machine.data=guest.data();}
    ~Probe(){current=nullptr;}
    void prepare(const rloft::original_levels::LevelGrid& level) {
        const auto n=level.cells.size();
        CHECK(n==std::size_t(level.width)*level.height);
        CHECK(n>0u && n<=16384u);
        // CODE.2 181B4 reads exactly 0x2A4 bytes to 25618h and the grid
        // into 198E0h. Load identical original guest-memory placements.
        std::copy(level.header.begin(),level.header.end(),guest.begin()+0x25618u);
        unsigned marked=0;
        for(std::size_t i=0;i<n;++i) {
            code2::store_le16(guest.data()+0x198E0u+2u*i,level.cells[i]);
            if(level.cells[i]&0x40u)++marked;
        }
        CHECK(code2::load_le16(guest.data()+0x25619u)==level.width);
        CHECK(code2::load_le16(guest.data()+0x2561Bu)==level.height);
        // Bound but do not fabricate original LEVELS camera state: the
        // trig/look-up table is test-owned zeros and fixed mul returns zero.
        constexpr u32 table=0x40000u;
        code2::store_le32(guest.data()+0xC65Cu,table);
        code2::store_le32(guest.data()+0xC658u,0u);
        code2::Application90Boundary boundary{};
        boundary.call_35680=&synthetic_fixed_mul;
        boundary.call_32954=&stop_at_ray_sweep;
        code2::function_334c4(machine,boundary);
        CHECK(ray_handoffs==1u);
        CHECK(code2::load_le32(guest.data()+0xFD0Cu)==n);
        // Original 334C4 clears ONLY transient 0x0040 current-visibility
        // flags before casting rays. Persisted 0400 and all authored
        // wall/material/shading bits must be retained exactly.
        unsigned changed=0;
        for(std::size_t i=0;i<n;++i) {
            const auto after=code2::load_le16(guest.data()+0x198E0u+2u*i);
            CHECK(after==(level.cells[i]&static_cast<u16>(~0x0040u)));
            if(after!=level.cells[i])++changed;
        }
        CHECK(changed==marked);
        std::cout<<"CODE2_334C4_ORIGINAL_LEVEL_GRID_VIEW_PREP_PASS"
                 <<" level="<<level.level<<" width="<<level.width
                 <<" height="<<level.height<<" cells="<<n
                 <<" cleared_transient="<<changed<<"\n";
        std::cout<<"CODE2_32954_REAL_WORLD_RAYCAST_EXECUTED=NO\n";
        std::cout<<"FULL_GAME_CAMERA_OCCLUSION_PARITY_ACCEPTED=NO\n";
    }
};
Probe* Probe::current=nullptr;
std::vector<u8> synthetic_levels() {
    constexpr unsigned count=32,header_size=0x2A4u,rows=2u,cols=3u;
    const u32 segment=0x1Du+header_size+2u*cols*rows;
    std::vector<u8> archive(4u+count*4u+segment*count,0u);
    code2::store_le32(archive.data(),count);
    for(unsigned level=0;level<count;++level) {
        const u32 begin=4u+count*4u+segment*level;
        code2::store_le32(archive.data()+4u+level*4u,begin);
        code2::store_le32(archive.data()+begin+1u,0x1Du);
        for(unsigned b=1;b<7;++b)
            code2::store_le32(archive.data()+begin+1u+4u*b,segment);
        code2::store_le16(archive.data()+begin+0x1Du+1u,cols);
        code2::store_le16(archive.data()+begin+0x1Du+3u,rows);
        constexpr u16 source[6]={0x0440u,0x0040u,0x2001u,0xC040u,0x0481u,0u};
        const u32 grid=begin+0x1Du+header_size;
        for(unsigned i=0;i<6;++i)
            code2::store_le16(archive.data()+grid+i*2u,source[i]);
    }
    return archive;
}
}
int main(int argc,char**argv) {
    try {
        if(argc!=1 && !(argc==3 && std::string(argv[1])=="--levels"))
            throw std::invalid_argument("usage: rloft_original_code2_level_view [--levels /owned/LEVELS.DAT]");
        const auto data=(argc==3)?
            rloft::original_levels::read_archive(argv[2]):synthetic_levels();
        for(u32 id=0;id<32u;++id) {
            const auto original=rloft::original_levels::decode_grid(data,id);
            auto session=std::make_unique<Probe>();
            session->prepare(original);
        }
        std::cout<<"CODE2_ALL_32_ORIGINAL_LEVELS_VIEW_PREP_PASS\n";
        return 0;
    }catch(const std::exception& err) {
        std::cerr<<"CODE2_ORIGINAL_LEVELS_VIEW_PREP_FAIL: "<<err.what()<<"\n";
        return 1;
    }
}
