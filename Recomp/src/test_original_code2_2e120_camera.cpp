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
    check(f.get32(0xC658u)==356u,
          "Original 2E120 failed signed 80/20 degree heading accumulator");
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
        std::cout<<"ORIGINAL_CODE2_2E120_2DBAC_CAMERA_SOURCE_TEST_PASS"
                 <<" heading=ORIGINAL_2E120"
                 <<" displacement=ORIGINAL_2DBAC"
                 <<" tile_collision=ORIGINAL_2D5D0"
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
