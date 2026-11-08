// Media-free acceptance of extracted host services, not DOS hardware parity.
#include "game/ui/Presentation.hpp"
#include "game/input/Input.hpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
namespace {
void check(bool ok, const char* why) {
    if(!ok) throw std::runtime_error(why);
}
void test_presentation() {
    using namespace rloft::presentation;
    check(can_select_page(0u,true) && can_select_page(1u,true),
          "valid original page selection");
    check(!can_select_page(2u,true) && !can_select_page(0u,false),
          "page selection fails closed");
    check(can_push_20294(0u,true,true,true,{},0u),
          "initial original page push");
    check(!can_push_20294(1u,true,true,true,{},0u) &&
          !can_push_20294(0u,false,true,true,{},0u) &&
          !can_push_20294(0u,true,true,false,{},0u) &&
          !can_push_20294(0u,true,true,true,{},1u),
          "page push invalid prerequisites");
    check(can_clear_20294(0u,0u,400u,320u,0u,true,{1u,0u,0u}) &&
          can_clear_20294(0u,0u,400u,320u,0u,true,{1u,0u,1u}),
          "two genuine planar fill slots");
    check(!can_clear_20294(0u,0u,400u,320u,0u,true,{1u,0u,2u}) &&
          !can_clear_20294(0u,0u,400u,319u,0u,true,{1u,0u,0u}) &&
          !can_clear_20294(0u,0u,400u,320u,1u,true,{1u,0u,0u}),
          "reject extra and malformed fills");
    check(can_pop_20294(true,{1u,0u,2u},1u),
          "balanced original page pop");
    check(!can_pop_20294(true,{1u,0u,1u},1u) &&
          !can_pop_20294(true,{1u,0u,2u},0u),
          "reject incomplete page pop");
    check(can_push_20294(0u,true,true,true,{1u,1u,2u},0u),
          "second original page-push cycle");
}
void test_input() {
    using namespace rloft::input;
    Irq9Ports port{};
    std::uint8_t out=0xFFu;
    port.begin_scan(0x13u);
    check(port.read8(0x60u,out) && out==0x13u,"set-1 make");
    check(port.read8(0x61u,out) && out==0u,"initial port 61h");
    check(port.write8(0x61u,0x80u) &&
          port.read8(0x61u,out) && out==0x80u,
          "port 61h acknowledgment");
    check(port.write8(0x20u,0x20u) && port.eoi_count()==1u,
          "PIC end-of-interrupt");
    check(!port.read8(0x62u,out) && !port.write8(0x20u,0x21u),
          "reject unrecognized ports");
    port.begin_scan(0x93u);
    check(port.read8(0x60u,out) && out==0x93u &&
          port.read8(0x61u,out) && out==0u &&
          port.eoi_count()==1u, "set-1 break, persistent EOI count");
    check(classify_20440_mouse(8u,0u,0u,0x18Fu)==MouseCommand::RangeY &&
          classify_20440_mouse(7u,0u,0u,0x27Eu)==MouseCommand::RangeX &&
          classify_20440_mouse(4u,0u,0x140u,0xC8u)==MouseCommand::SetPosition &&
          classify_20440_mouse(3u,0u,0u,0u)==MouseCommand::ReadPosition,
          "exact original 20440 INT33 calls");
    check(classify_20440_mouse(8u,0u,0u,0xC7u)==MouseCommand::Unsupported &&
          classify_20440_mouse(4u,1u,0x140u,0xC8u)==MouseCommand::Unsupported &&
          classify_20440_mouse(12u,0u,0u,0u)==MouseCommand::Unsupported,
          "reject unsupported mouse calls");
    std::uint8_t scan=0u;
    check(is_host_set1_key(0xE013u) &&
          decode_host_set1_key(0xE013u,0x10000u,scan) && scan==0x13u,
          "bounded physical host scan");
    check(!decode_host_set1_key(0xE000u,0x10000u,scan) &&
          !decode_host_set1_key(0xE059u,0x10000u,scan) &&
          !decode_host_set1_key(0xFFFDu,0x10000u,scan) &&
          !decode_host_set1_key(0xE058u,0xF6F3u,scan),
          "invalid scan, diagnostic and truncated guest");
}
}
int main() {
    try {
        test_presentation(); test_input();
        std::cout<<"CODE2_PRESENTATION_INPUT_HOST_ABI_PASS"
                 <<" source_vga=20294"
                 <<" source_keys=308F8_30B40_F590"
                 <<" physical_dos_parity=OPEN\n";
        return 0;
    } catch(const std::exception& e) {
        std::cerr<<"CODE2_PRESENTATION_INPUT_HOST_ABI_FAIL: "
                 <<e.what()<<'\n';
        return 1;
    }
}
