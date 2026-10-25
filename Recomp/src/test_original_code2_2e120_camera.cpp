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
        test_sustained_original_camera_and_heading();
        std::cout<<"ORIGINAL_CODE2_2E120_2DBAC_CAMERA_SOURCE_TEST_PASS"
                 <<" heading=ORIGINAL_2E120"
                 <<" displacement=ORIGINAL_2DBAC"
                 <<" tile_collision=ORIGINAL_2D5D0"
                 <<" secondary_collision=ORIGINAL_2CC88"
                 <<" source_level_transition=ORIGINAL_181B4_CALLBACK"
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
