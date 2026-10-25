#include "original_code2_script_preflight.hpp"
// Compile and execute ORIGINAL reconstructed CODE.2 movement, camera,
// and tile-collision functions. All guest bytes here are synthetic test
// fixtures, never mistaken for original Level 22 retail framebuffer parity.
#include "../../Code2/Code2.cpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>
namespace {
using u32=code2::u32;
using s32=code2::s32;
struct SyntheticGuest {
    std::vector<code2::u8> data;
    code2::Machine machine{};
    SyntheticGuest():data(code2::kObject2VirtualSize,0u) {
        machine.data=data.data();
        put32(0x255D0u,0x00048000u);
        put32(0x255D4u,0x00048000u);
        put32(0xC658u,0u);
        put16(0x25619u,16u); // verified source map-axis register
        put16(0x2561Bu,16u);
        put32(0xF040u,4u);
        put32(0xF044u,4u);
        data[0xC02Fu]=1u; // Original 45-degree grid movement branch.
    }
    void put32(u32 a,u32 v) {code2::store_le32(data.data()+a,v);}
    void put16(u32 a,code2::u16 v) {code2::store_le16(data.data()+a,v);}
    u32 get32(u32 a) const {return code2::load_le32(data.data()+a);}
};
void check(bool good,const char* message) {
    if(!good)throw std::runtime_error(message);
}
code2::Application67Boundary original_movement_callbacks(code2::Application66Boundary& collision) {
    code2::Application67Boundary b{};
    b.app66=&collision;
    b.call_35680=&code2::function_35680;
    b.call_369a7=&code2::function_369a7;
    return b;
}
void test_heading_turn_source() {
    SyntheticGuest f;
    code2::Application66Boundary collision{};
    auto b=original_movement_callbacks(collision);
    f.put32(0xC6ECu,80u);
    code2::function_2e120(f.machine,b);
    check(f.get32(0xC658u)==270u,
          "Original 2E120 grid-mode 90-degree step deviated from original source");
    check(f.get32(0x255D0u)==0x48000u &&
          f.get32(0x255D4u)==0x48000u,
          "Original turn-only source unexpectedly moved camera");
    check(f.get32(0xC6E4u)==0u,
          "Original turning remainder did not preserve 16.16 source accumulator");
}
void test_unblocked_camera_translation() {
    SyntheticGuest f;
    code2::Application66Boundary collision{};
    auto b=original_movement_callbacks(collision);
    f.put32(0xC6E8u,0xFFFFFFF6u); // Original signed forward velocity -10.
    code2::function_2e120(f.machine,b);
    check(f.get32(0x255D0u)==0x48000u,
          "Original 2E120 unexpected X translation on north heading");
    check(f.get32(0x255D4u)==0x58000u,
          "Original 2E120 did not execute 2DBAC guest camera translation");
    check(f.get32(0xF044u)==5u && f.get32(0xF040u)==4u,
          "Original 2DBAC did not update guest tile coordinates");
}
void test_collision_prevents_camera_translation() {
    SyntheticGuest f;
    code2::Application66Boundary collision{};
    auto b=original_movement_callbacks(collision);
    // Dest tile row 5, col 4 has nonzero tile code and no pass-through flag.
    f.put16(0x198E0u+(5u*16u+4u)*2u,1u);
    f.put32(0xC6E8u,0xFFFFFFF6u);
    code2::function_2e120(f.machine,b);
    check(f.get32(0x255D4u)==0x48000u &&
          f.get32(0x255D0u)==0x48000u,
          "Original collision allowed camera movement through a blocked tile");
}
u32 transition_level=0u;
void track_original_secondary_transition(u32 destination) {
    transition_level=destination;
}
void test_secondary_object_collision() {
    SyntheticGuest f;
    // Original 2CC88 object table, distinct from 2C800 entity collision.
    // One object at the destination, one 0x1000 fixed-point-radius record.
    f.put16(0x258A6u,1u);
    f.put32(0x255DCu,0x23000u);
    f.put16(0x22246u,1u);
    f.put16(0x23000u+7u+2u,0x1000u);
    f.put32(0x2223Cu,0x48000u); // original object x
    f.put32(0x22238u,0x58000u); // original object y
    code2::Application65Boundary secondary{};
    code2::Application66Boundary collision{};
    collision.app65=&secondary;
    auto b=original_movement_callbacks(collision);
    f.put32(0xC6E8u,0xFFFFFFF6u);
    code2::function_2e120(f.machine,b);
    check(f.get32(0x255D0u)==0x48000u &&
          f.get32(0x255D4u)==0x48000u,
          "Original 2CC88 secondary object collision was not enforced");
    // Move the object away; the same original 2E120/2DBAC call must
    // succeed without injecting a host camera assignment.
    f.put32(0x2223Cu,0xA8000u);
    f.put32(0xC6E8u,0xFFFFFFF6u);
    code2::function_2e120(f.machine,b);
    check(f.get32(0x255D4u)==0x58000u,
          "Original 2CC88 object removal did not unblock source movement");
}
void test_original_secondary_level_event() {
    SyntheticGuest f;
    f.put16(0x258A6u,1u);
    f.put32(0x255DCu,0x23000u);
    f.put16(0x22246u,0x54u);
    f.put16(0x2224Cu,0x40u);
    f.put16(0x23000u+0x54u*7u+2u,0x1000u);
    f.put32(0x2223Cu,0x48000u);
    f.put32(0x22238u,0x58000u);
    f.put32(0xC6C8u,0x17u); // source special transition condition
    code2::Application65Boundary secondary{};
    secondary.call_181b4=&track_original_secondary_transition;
    transition_level=0u;
    const u32 r=code2::function_2cc88(
        f.machine,secondary,0x58000u,0x48000u,0xFFFFFFFFu);
    check(r==2u && transition_level==0x13u,
          "Original 2CC88 special-object transition ID or callback changed");
    check(f.get32(0x255D0u)==0x38000u &&
          f.get32(0x255D4u)==0xA8000u,
          "Original 2CC88 did not write source portal destination");
}
void test_sustained_original_camera_and_heading() {
    SyntheticGuest f;
    code2::Application65Boundary secondary{};
    code2::Application66Boundary collision{};
    collision.app65=&secondary;
    auto b=original_movement_callbacks(collision);
    const u32 original_y=f.get32(0x255D4u);
    for(unsigned tick=0;tick<5u;++tick) {
        f.put32(0xC6E8u,0xFFFFFFF6u);
        code2::function_2e120(f.machine,b);
        check(f.get32(0x255D4u)==original_y+(tick+1u)*0x10000u,
              "Sustained original 2E120 movement did not advance every tick");
    }
    f.put32(0xC6ECu,80u);
    code2::function_2e120(f.machine,b);
    check(f.get32(0xC658u)==270u,
          "Sustained original movement failed quarter-turn source integration");
    f.put32(0xC6ECu,0xFFFFFFB0u); // signed -80, reverse quarter-turn
    code2::function_2e120(f.machine,b);
    check(f.get32(0xC658u)==0u,
          "Original turning accumulator failed to restore source heading");
}
unsigned original_page_returns=0u;
u32 original_page_selected=0xFFFFFFFFu;
void trace_original_page(u32 page) {
    original_page_selected=page;
    ++original_page_returns;
}
void test_original_special_event_source_page_refresh() {
    SyntheticGuest f;
    code2::Application43Boundary ui{};
    ui.call_1a510=&trace_original_page;
    original_page_returns=0u;
    f.data[0xC050u]=0u;
    code2::function_1ea78(f.machine,ui);
    check(original_page_returns==1u && original_page_selected==0u &&
          f.data[0xC050u]==1u,
          "Source 1EA78 failed original 1A510 page handoff");
    code2::function_1ea78(f.machine,ui);
    check(original_page_returns==2u && original_page_selected==1u &&
          f.data[0xC050u]==0u,
          "Source 1EA78 failed second original C050 page flip");
}
unsigned original_tile_sound_requests=0u;
void trace_original_tile_sound(u32,u32,u32,u32) {
    ++original_tile_sound_requests;
}
void test_original_20c0c_animated_tile_event() {
    SyntheticGuest f;
    constexpr u32 rec=0x23000u;
    f.data[rec+1u]=5u; // original active bit + type 1
    f.data[rec+4u]=5u; // original cell row
    f.data[rec+5u]=4u; // original cell column
    f.data[rec+6u]=0u;
    f.data[rec+7u]=0u;
    code2::Application46Boundary b{};
    b.call_28110=&trace_original_tile_sound;
    original_tile_sound_requests=0u;
    const u32 changed=code2::function_20c0c(f.machine,b,5u,4u,rec);
    check(changed==1u && f.data[rec+7u]==1u &&
          f.data[0xCF50u]==1u && original_tile_sound_requests==1u,
          "Source 20C0C did not activate animated tile and enqueue 20A24 SFX");
    const u32 again=code2::function_20c0c(f.machine,b,5u,4u,rec);
    check(again==0u && original_tile_sound_requests==1u,
          "Source tile transition fired SFX twice without a state change");
}

SyntheticGuest* active_script_fixture=nullptr;
void script_set_bit(u32 bit) {
    code2::function_1c124(active_script_fixture->machine,bit);
}
void script_toggle_bit(u32 bit) {
    code2::function_1c1a0(active_script_fixture->machine,bit);
}
unsigned script_sound_requests=0u;
u32 script_last_sound_id=0u;
void script_queue_sound(u32 id,u32 aux,u32 channel,u32 priority) {
    if(aux!=0xFFu || channel!=0u || priority!=0u)
        throw std::runtime_error("Original 002A8 sound callback arguments changed");
    script_last_sound_id=id;
    ++script_sound_requests;
}
void test_original_002a8_live_script_operations() {
    SyntheticGuest f;
    active_script_fixture=&f;
    constexpr u32 scripts=0x23000u; // 50 x 26 original descriptor bytes.
    f.put32(0xCF44u,scripts);
    code2::Dispatch002A8Boundary dispatch{};
    dispatch.call_1c124=&script_set_bit;
    dispatch.call_1c1a0=&script_toggle_bit;
    dispatch.call_28110=&script_queue_sound;
    // Original tile setter + bit setter + BF44: all write via compiled 002A8.
    f.data[scripts]=3u;
    f.data[scripts+1u]=4u;
    f.data[scripts+2u]=5u; f.data[scripts+3u]=4u;
    f.data[scripts+4u]=0x23u;
    f.data[scripts+6u]=8u; f.data[scripts+9u]=0x3Au;
    f.data[scripts+11u]=12u; f.data[scripts+14u]=0x2Eu;
    const u32 tile=0x198E0u+(5u*16u+4u)*2u;
    f.put16(tile,0xFFC0u);
    auto ok=rloft::source_script::validate(
        f.data.data(),f.data.size(),scripts,0);
    check(ok.accepted && ok.commands==3u,"Original 002A8 script unexpectedly rejected");
    code2::function_002a8(f.machine,dispatch,0);
    check((code2::load_le16(f.data.data()+tile)&0x3Fu)==0x23u &&
          code2::function_1c1dc(f.machine,0x3Au)!=0u &&
          f.get32(0xBF44u)==0x2Eu,
          "Original compiled 002A8 failed tile/bit/BF44 side effects");

    // Original sound opcode passes the LE32 payload to the 28110 callback.
    const u32 second=scripts+0x1Au;
    f.data[second]=1u;f.data[second+1u]=5u;
    f.put32(second+4u,0x35u);
    ok=rloft::source_script::validate(
        f.data.data(),f.data.size(),scripts,1);
    check(ok.accepted,"Original 002A8 source sound event rejected");
    script_sound_requests=0u; script_last_sound_id=0u;
    code2::function_002a8(f.machine,dispatch,1);
    check(script_sound_requests==1u && script_last_sound_id==0x35u,
          "Original compiled 002A8 sound opcode did not queue exact request");

    // Whole-descriptor preflight blocks 7 (unconnected frame/dialog path)
    // even if an earlier command would otherwise change a live flag.
    const u32 third=scripts+2u*0x1Au;
    f.data[third]=2u;
    f.data[third+1u]=8u;f.data[third+4u]=0x3Bu;
    f.data[third+6u]=7u;
    ok=rloft::source_script::validate(
        f.data.data(),f.data.size(),scripts,2);
    check(!ok.accepted && code2::function_1c1dc(f.machine,0x3Bu)==0u,
          "Unconnected original 002A8 opcode was partially executed");
    f.data[third]=6u;
    check(!rloft::source_script::validate(
        f.data.data(),f.data.size(),scripts,2).accepted,
          "Oversized original 1A-byte script descriptor accepted");
    check(!rloft::source_script::validate(
        f.data.data(),f.data.size(),scripts,50).accepted,
          "Beyond 50 fixed original script descriptors was accepted");
    check(!rloft::source_script::validate(
        f.data.data(),f.data.size(),0u,0).accepted,
          "Missing original CF44 script table was accepted");
    check(rloft::source_script::validate(
        f.data.data(),f.data.size(),0u,-1).accepted,
          "Original -1 no-script sentinel was rejected");
    active_script_fixture=nullptr;
}
void test_signed_accumulation_and_absolute_motion() {
    SyntheticGuest f;
    code2::Application66Boundary collision{};
    auto b=original_movement_callbacks(collision);
    f.put32(0xC6F0u,0xFFFFFFF6u); // source turn indicates negative motion
    code2::function_2e120(f.machine,b);
    check(f.get32(0xC6E8u)==10u,
          "Original 369A7 signed absolute was not called");
    check(f.get32(0x255D0u)!=0x48000u ||
          f.get32(0x255D4u)!=0x48000u,
          "Original 2E120 after a turn failed to drive 2DBAC source guest");
}
}
int main(){
    try{
        test_heading_turn_source();
        test_unblocked_camera_translation();
        test_collision_prevents_camera_translation();
        test_signed_accumulation_and_absolute_motion();
        test_secondary_object_collision();
        test_original_secondary_level_event();
        test_original_20c0c_animated_tile_event();
        test_original_special_event_source_page_refresh();
        test_sustained_original_camera_and_heading();
        test_original_002a8_live_script_operations();
        std::cout<<"ORIGINAL_CODE2_2E120_2DBAC_CAMERA_SOURCE_TEST_PASS"
                 <<" heading=ORIGINAL_2E120"
                 <<" displacement=ORIGINAL_2DBAC"
                 <<" tile_collision=ORIGINAL_2D5D0"
                 <<" secondary_collision=ORIGINAL_2CC88"
                 <<" source_level_transition=ORIGINAL_181B4_CALLBACK"
                 <<" animated_tile_event=ORIGINAL_20C0C_20A24_SOUND"
                 <<" bounded_script_dispatch=ORIGINAL_002A8"
                 <<" page_handoff=ORIGINAL_1EA78_1A510"
                 <<" sustained_motion_ticks=5"
                 <<" heading_turns=2"
                 <<" synthetic_guest=ONLY"
                 <<" real_level22_camera=NOT_YET_CERTIFIED"
                 <<" dos_indexed_parity=OPEN\n";
        return 0;
    }catch(const std::exception& e) {
        std::cerr<<"ORIGINAL_CODE2_2E120_2DBAC_CAMERA_SOURCE_TEST_FAIL: "
                 <<e.what()<<"\n";
        return 1;
    }
}
