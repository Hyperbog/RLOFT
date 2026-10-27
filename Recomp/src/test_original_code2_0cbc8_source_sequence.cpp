// Compiled original 0CBC8 -> 0CADC transition/source input regression.
// All callback implementations below are observability probes ONLY, not
// retained-guest renderer/audio/player substitutes or DOS parity.
#include "original_code2_game_over_readiness.hpp"
#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>
namespace {
using code2::u32;
struct Evidence {
    u32 scene=0,pages=0,modal=0,audio=0,reset=0,page_select=0;
    u32 pumps=0,arts=0,presents=0,music=0,delay=0;
    u32 cursor_restore=0,sfx=0,allocs=0,rng=0,copies=0,frees=0;
    u32 cursor_redraw=0,input=0,exit=0;
    std::array<u32,9> original_art{};
    std::array<u32,2> modal_values{};
    u32 sound_id=0;
    u32 first_copy_page=0,first_copy_row=0,first_copy_size=0;
    u32 final_exit_code=0;
} ev;
void check(bool v,const char* msg){if(!v)throw std::runtime_error(msg);}
code2::Application18Boundary callbacks() {
    code2::Application18Boundary b{};
    b.call_1f1c8=+[]() noexcept {++ev.scene;};
    b.call_1ea78=+[]() noexcept {++ev.pages;};
    b.call_1f494=+[](u32 v) noexcept {
        if(ev.modal<2u)ev.modal_values[ev.modal]=v;
        ++ev.modal;
    };
    b.call_28168=+[]() noexcept {++ev.audio;};
    b.call_1a9f8=+[]() noexcept {++ev.reset;};
    b.call_291b0=+[](u32) noexcept {++ev.page_select;};
    b.call_2e608=+[]() noexcept {++ev.pumps;};
    b.call_2e510=+[](u32,u32,u32 art,u32) noexcept {
        if(ev.arts<ev.original_art.size())ev.original_art[ev.arts]=art;
        ++ev.arts;
    };
    b.call_1ea04=+[]() noexcept {++ev.presents;};
    b.call_28c54=+[](u32,u32) noexcept {++ev.music;};
    b.call_2e614=+[](u32,u32) noexcept {++ev.delay;};
    b.call_1aacc=+[]() noexcept {++ev.cursor_restore;};
    b.call_27f24=+[](u32 id) noexcept {++ev.sfx;ev.sound_id=id;};
    b.call_3104c=+[](u32 size) noexcept ->u32{
        if(size!=0xE10u)std::terminate();
        ++ev.allocs;return 0x200000u;
    };
    b.call_30ca2=+[]() noexcept ->u32{++ev.rng;return 0u;};
    b.call_2a078=+[](u32 page,u32 x,u32 y,u32,u32,u32 w,u32 h) noexcept {
        if(ev.copies==0u) {
            ev.first_copy_page=page;ev.first_copy_row=y;
            ev.first_copy_size=w*h;
            if(x!=0u)std::terminate();
        }
        ++ev.copies;
    };
    b.call_310b0=+[](u32 p) noexcept{
        if(p!=0x200000u)std::terminate();
        ++ev.frees;
    };
    b.call_1a288=+[]() noexcept {++ev.cursor_redraw;};
    b.call_1a1f4=+[]() noexcept ->u32 {
        constexpr u32 input[4]={1u,0u,0u,1u};
        if(ev.input>=4u)std::terminate();
        return input[ev.input++];
    };
    b.call_205b4=+[](u32 code) noexcept {
        ++ev.exit;ev.final_exit_code=code;
    };
    return b;
}
}
int main() {
    try {
        std::vector<code2::u8> bytes(0x300000u,0u);
        code2::Machine machine{};machine.data=bytes.data();
        bytes[0xC050u]=1u;
        code2::store_le32(bytes.data()+0x258DCu,0x1000u);
        code2::store_le32(bytes.data()+0x258D0u,0x2000u);
        auto b=callbacks();
        check(rloft::source_game_over::validate(
            b,bytes.data(),bytes.size()).ready,
            "fully populated source compositor boundary was rejected");
        auto withheld=b;b.call_2a078=nullptr;
        check(!rloft::source_game_over::validate(
            b,bytes.data(),bytes.size()).ready,
            "missing real VGA page-copy callback admitted");
        b=callbacks();
        code2::function_0cbc8(machine,b); // complete compiled source sequence
        check(ev.scene==1u && ev.pages==9u && ev.modal==2u &&
              ev.modal_values[0]==1u && ev.modal_values[1]==0u,
              "original scene/modal/page choreography");
        check(ev.audio==1u && ev.reset==1u &&
              ev.pumps==8u && ev.arts==9u && ev.presents==9u &&
              ev.music==1u && ev.delay==7u,
              "original indexed animation frame/music schedule");
        constexpr u32 offsets[9]={0u,1u,0u,1u,0u,2u,3u,4u,5u};
        for(u32 i=0;i<9u;++i)
            check(ev.original_art[i]==0x105Du+offsets[i],
                  "authentic CODE2 artwork sequence changed");
        check(ev.cursor_restore==1u && ev.cursor_redraw==1u &&
              ev.sfx==1u && ev.sound_id==0x2001u,
              "original cursor and sound requests lost");
        check(ev.allocs==1u && ev.frees==1u &&
              ev.copies==32000u && ev.rng==32400u &&
              ev.first_copy_page==0u && ev.first_copy_size==4u,
              "original 0CADC 400-record staged transition diverged");
        check(ev.input==4u && ev.exit==1u &&
              ev.final_exit_code==0xCu,
              "original mouse release/press wait and exit diverged");
        std::cout<<"ORIGINAL_0CBC8_0CADC_SOURCE_SEQUENCE_PASS"
                 <<" real_source=YES synthetic_callbacks=YES"
                 <<" original_artwork_sequence=YES"
                 <<" original_transition=YES source_wait=YES"
                 <<" live_host=NOT_CONNECTED retail_DOS_parity=NOT_CERTIFIED\n";
        return 0;
    }catch(const std::exception& e){
        std::cerr<<"FAIL "<<e.what()<<"\n";return 1;
    }
}
