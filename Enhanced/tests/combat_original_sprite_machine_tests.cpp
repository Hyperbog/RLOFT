#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_sprite_machine.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <vector>

static std::uint32_t ptr(const std::vector<std::uint8_t>& b,std::size_t at){
    return static_cast<std::uint32_t>(b[at]) |
           (static_cast<std::uint32_t>(b[at+1U])<<8U) |
           (static_cast<std::uint32_t>(b[at+2U])<<16U) |
           (static_cast<std::uint32_t>(b[at+3U])<<24U);
}
int main(){
    rloft::CombatOriginalSpriteMachineState s{};
    s.writable_code_object1.assign(0x2A676U,0xCCU);
    auto& c=s.writable_code_object1;
    c[0x2A609]=0x8A;c[0x2A60A]=0x83;
    c[0x2A670]=0x8A;c[0x2A671]=0x83;
    c[0x2A608]=0x43;
    using Model=rloft::CombatOriginalSpriteCodePatchModel;
    assert(Model::patch_2a584(s,0xFABC1234U));
    assert(ptr(c,0x2A60B)==0xFABC1234U);
    assert(ptr(c,0x2A672)==0xFABC1234U);
    assert(c[0x2A608]==0x43 && c[0x2A609]==0x8A);
    assert(Model::patch_2a584(s,0U));
    assert(ptr(c,0x2A60B)==0U && ptr(c,0x2A672)==0U);
    c[0x2A670]=0xFF;
    assert(!Model::patch_2a584(s,0x12345678U));
    assert(ptr(c,0x2A60B)==0U);
    c[0x2A670]=0x8A;
    c.resize(0x2A675U);
    assert(!Model::patch_2a584(s,0x12345678U));

    // 2A7A4 writes into a single CMP imm8 at 2A816.
    c.assign(0x2A817U,0xCCU);
    c[0x2A814U]=0x80U;
    c[0x2A815U]=0x3EU;
    c[0x2A816U]=0x04U;
    assert(Model::patch_2a6b0(s,0xFFU));
    assert(c[0x2A814U]==0x80U&&c[0x2A815U]==0x3EU);
    assert(c[0x2A816U]==0xFFU);
    assert(Model::patch_2a6b0(s,0x11U));
    assert(c[0x2A816U]==0x11U);
    c[0x2A815U]=0x31U;
    assert(!Model::patch_2a6b0(s,0x99U));
    assert(c[0x2A816U]==0x11U);
    c[0x2A815U]=0x3EU;
    c.resize(0x2A816U);
    assert(!Model::patch_2a6b0(s,0x12U));
    const auto* owned=std::getenv("RLOFT_TEST_CODE2");
    if(owned!=nullptr && *owned!='\0'){
        std::ifstream f(owned,std::ios::binary);assert(f.good());
        const std::vector<std::uint8_t> bytes{
            std::istreambuf_iterator<char>(f),std::istreambuf_iterator<char>()};
        assert(bytes.size()>=0x3AF24U+0x2A817U);
        c.assign(bytes.begin()+0x3AF24U,
                 bytes.begin()+0x3AF24U+0x2A817U);
        assert(ptr(c,0x2A60B)==0x12345678U);
        assert(ptr(c,0x2A672)==0x12345678U);
        assert(Model::patch_2a584(s,0x87654321U));
        assert(ptr(c,0x2A60B)==0x87654321U);
        assert(ptr(c,0x2A672)==0x87654321U);
        assert(c[0x2A609]==0x8A && c[0x2A670]==0x8A);
        // The actual original CODE.2 contains 80 3E 04 at
        // 2A814..2A816; the self patch changes ONLY byte 2A816.
        assert(c[0x2A814U]==0x80U);
        assert(c[0x2A815U]==0x3EU);
        assert(c[0x2A816U]==0x04U);
        assert(Model::patch_2a6b0(s,0x7FU));
        assert(c[0x2A816U]==0x7FU);
        assert(c[0x2A814U]==0x80U && c[0x2A815U]==0x3EU);
    }
}
