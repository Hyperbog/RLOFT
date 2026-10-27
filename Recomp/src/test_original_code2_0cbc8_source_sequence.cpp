// Compiled original 0CBC8 -> 0CADC transition/source input regression.
// All callback implementations below are observability probes ONLY, not
// retained-guest renderer/audio/player substitutes or DOS parity.
#include "../../Code2/Code2.cpp"
#include "original_code2_game_over_readiness.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>
namespace {
using code2::u32;
code2::Machine* source_guest=nullptr;
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
    u32 shutdown_count=0;
    std::array<u32,6> shutdown_order{};
    u32 unrequested_video_reset=0;
} ev;
u32 original_party_dead_portraits=0u;
u32 source_scene_hud_blits=0u;
std::array<u32,7> source_scene_callbacks{};
u32 source_scene_stage=0u;
void check(bool v,const char* msg){if(!v)throw std::runtime_error(msg);}
void shutdown_step(u32 id) noexcept {
    if(ev.shutdown_count>=ev.shutdown_order.size())std::terminate();
    ev.shutdown_order[ev.shutdown_count++]=id;
}
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
        code2::Application46Boundary shutdown{};
        shutdown.call_1a1d0=+[]() noexcept {shutdown_step(1u);};
        shutdown.call_310b0=+[](u32 address) noexcept {
            if(address!=0x80000u && address!=0x81000u)
                std::terminate();
            shutdown_step(address==0x80000u?2u:3u);
        };
        shutdown.call_3810=+[]() noexcept {shutdown_step(4u);};
        shutdown.call_2f4e0=+[]() noexcept {shutdown_step(5u);};
        shutdown.call_290f4=+[](u32) noexcept {
            ++ev.unrequested_video_reset;
        };
        shutdown.call_352be=+[](code2::s32 status) noexcept {
            shutdown_step(6u);
            ++ev.exit;
            ev.final_exit_code=static_cast<u32>(status);
        };
        code2::function_205b4(
            *source_guest,shutdown,static_cast<code2::s32>(code));
    };
    return b;
}
}
int main() {
    try {
        std::vector<code2::u8> bytes(0x300000u,0u);
        code2::Machine machine{};machine.data=bytes.data();source_guest=&machine;
        bytes[0xC050u]=1u;
        code2::store_le32(bytes.data()+0x258DCu,0x1000u);
        code2::store_le32(bytes.data()+0x258D0u,0x2000u);
        code2::store_le32(bytes.data()+0xC65Cu,0x80000u);
        code2::store_le32(bytes.data()+0xC660u,0x81000u);
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
              ev.final_exit_code==0xCu && ev.shutdown_count==6u &&
              ev.shutdown_order==std::array<u32,6>{1u,2u,3u,4u,5u,6u} &&
              ev.unrequested_video_reset==0u,
              "original 205B4 cleanup order, mouse wait or exit diverged");
        // Source integration: 0CD44 -> 0AD24 -> 0893C -> 0CCF4 ->
        // 0CBC8 -> 0CADC. Observer callbacks record choreography only.
        // Other members are genuinely flagged unavailable in guest memory.
        std::fill(bytes.begin(),bytes.end(),0u);
        ev=Evidence{};
        bytes[0xC050u]=1u;
        code2::store_le32(bytes.data()+0x258DCu,0x1000u);
        code2::store_le32(bytes.data()+0x258D0u,0x2000u);
        code2::store_le32(bytes.data()+0xC65Cu,0x80000u);
        code2::store_le32(bytes.data()+0xC660u,0x81000u);
        code2::store_le32(bytes.data()+0xA7E4u,0x400u);
        code2::store_le16(bytes.data()+0x218F0u,0x0080u);
        code2::store_le16(bytes.data()+0x218FCu,2u);
        bytes[0x218E0u]='A';
        bytes[0x578u]='A';
        for(u32 slot=1;slot<4u;++slot)
            code2::store_le16(bytes.data()+0x218F0u+slot*0x248u,0xD040u);
        auto fatal=callbacks();
        fatal.call_19cc4=+[]() noexcept ->u32 {return 0x400u;};
        fatal.call_0ad24=+[](code2::s32 selected) noexcept {
            // This observer uses the original cleanup, not a fake return.
            if(selected!=0)std::terminate();
            code2::Application16Boundary cleanup{};
            cleanup.call_0893c=+[]() noexcept {
                // All members have empty timed-effect records here;
                // the original sweep still executes and must return.
                code2::Application12Boundary timers{};
                timers.call_19cc4=+[]() noexcept ->u32{return 0x400u;};
                code2::function_0893c(*source_guest,timers);
            };
            code2::function_0ad24(*source_guest,cleanup,selected);
        };
        fatal.call_14a60=+[](u32 token) noexcept ->u32 {
            code2::Application27Boundary select{};
            select.call_27c70=+[](u32 lhs,u32 rhs) noexcept ->u32 {
                return static_cast<u32>(
                    code2::function_27c70(*source_guest,lhs,rhs));
            };
            return static_cast<u32>(
                code2::function_14a60(*source_guest,select,token));
        };
        fatal.call_1c1dc=+[](u32 id) noexcept ->u32 {
            return code2::function_1c1dc(*source_guest,id);
        };
        check(rloft::source_game_over::validate(
            fatal,bytes.data(),bytes.size()).ready,
            "source lethal callback matrix incomplete");
        code2::function_0cd44(machine,fatal,0u,3u);
        check(code2::load_le16(bytes.data()+0x218FCu)==0u &&
              (bytes[0x218F1u]&0x40u)!=0u &&
              code2::load_le32(bytes.data()+0x21A92u)==3u &&
              code2::load_le32(bytes.data()+0x21A86u)==0x436u,
              "original lethal HP/status/timer data differs");
        check(ev.scene==1u && ev.arts==9u && ev.copies==32000u &&
              ev.allocs==1u && ev.frees==1u &&
              ev.input==4u && ev.exit==1u && ev.final_exit_code==0xCu &&
              ev.shutdown_count==6u && ev.unrequested_video_reset==0u,
              "actual 0CD44 did not enter original game-over compositor");
        // Exercise actual reconstructed 1F494's original four-party
        // high-bit/death redraw, rather than treating its compositor
        // callback as an opaque counter. Only final VGA art requests
        // are observed; the party dirty bits are source-owned.
        original_party_dead_portraits=0u;
        for(u32 slot=0u;slot<4u;++slot) {
            bytes[0xCF5Au+slot]=1u;
            code2::store_le16(
                bytes.data()+0x218F0u+slot*0x248u,0x8000u);
        }
        code2::Application44Boundary party{};
        party.call_2e510=+[](u32 row,u32 col,u32 id,u32 bank) noexcept {
            if(row!=0x113u || col>320u || bank!=0u ||
               id!=0x1010u)std::terminate();
            ++original_party_dead_portraits;
        };
        code2::function_1f494(machine,party,0u);
        check(original_party_dead_portraits==4u,
              "original 1F494 four dead-portrait source requests absent");
        for(u32 slot=0u;slot<4u;++slot)
            check(bytes[0xCF5Au+slot]==0u,
                  "original 1F494 dirty party state not cleared");
        // Exercise compiled 1F1C8's source-level world/HUD/actor
        // ordering on a controlled guest. Device leaf callbacks observe
        // requests; no world raster, DOS parity or synthetic VGA is
        // claimed by this fixture.
        std::fill(bytes.begin(),bytes.end(),0u);
        bytes[0xC050u]=1u;
        bytes[0x258A8u]=0xFFu;
        code2::store_le32(bytes.data()+0x22220u,0x800u);
        code2::store_le16(bytes.data()+0x255D4u,0x1234u);
        code2::store_le16(bytes.data()+0x255D0u,0x3456u);
        source_scene_hud_blits=0u;
        source_scene_callbacks={};
        source_scene_stage=0u;
        code2::Application43Boundary scene_ui{};
        scene_ui.call_2939c=+[](u32,u32,u32,u32) noexcept {
            ++source_scene_hud_blits;
        };
        code2::Application44Boundary scene{};
        scene.app43=&scene_ui;
        scene.call_291b0=+[](u32 page) noexcept {
            if(page!=1u)std::terminate();
            source_scene_callbacks[source_scene_stage++]=1u;
        };
        scene.call_33e28=+[](u32) noexcept {
            source_scene_callbacks[source_scene_stage++]=2u;
        };
        scene.call_33de0=+[](u32) noexcept {
            source_scene_callbacks[source_scene_stage++]=3u;
        };
        scene.call_34de8=+[]() noexcept {
            source_scene_callbacks[source_scene_stage++]=4u;
        };
        scene.call_1bdac=+[]() noexcept {
            source_scene_callbacks[source_scene_stage++]=5u;
        };
        scene.call_2c294=+[]() noexcept {
            source_scene_callbacks[source_scene_stage++]=6u;
        };
        scene.call_274a4=+[]() noexcept {
            source_scene_callbacks[source_scene_stage++]=7u;
        };
        scene.call_2f868=+[]() noexcept {
            if(source_scene_stage!=7u || source_scene_hud_blits!=14u)
                std::terminate();
        };
        code2::function_1f1c8(machine,scene);
        check(source_scene_stage==7u &&
              source_scene_callbacks==std::array<u32,7>{1u,2u,3u,4u,5u,6u,7u} &&
              source_scene_hud_blits==14u &&
              code2::load_le32(bytes.data()+0xFDBCu)==0xA8000u &&
              code2::load_le32(bytes.data()+0xC044u)==0x12u &&
              code2::load_le32(bytes.data()+0xC048u)==0x34u,
              "original 1F1C8 world/HUD/actor callback order diverged");
        std::cout<<"ORIGINAL_0CBC8_0CADC_SOURCE_SEQUENCE_PASS"
                 <<" real_source=YES synthetic_callbacks=YES"
                 <<" original_artwork_sequence=YES"
                 <<" original_transition=YES original_0CD44_chain=YES"
                 <<" original_205B4_cleanup=YES"
                 <<" original_1F494_dead_party=YES"
                 <<" original_1F1C8_scene_order=YES source_wait=YES"
                 <<" live_host=NOT_CONNECTED retail_DOS_parity=NOT_CERTIFIED\n";
        return 0;
    }catch(const std::exception& e){
        std::cerr<<"FAIL "<<e.what()<<"\n";return 1;
    }
}
