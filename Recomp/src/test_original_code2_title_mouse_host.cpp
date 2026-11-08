// Media-free ABI regression; this does not emulate title click selection.
#include "game/input/MouseLifecycle.hpp"
#include "game/ui/TitleInteraction.hpp"
#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>
namespace {
void check(bool ok,const char* why) {
    if(!ok)throw std::runtime_error(why);
}
void put16(std::uint8_t* p,std::uint16_t n) {
    p[0]=static_cast<std::uint8_t>(n);
    p[1]=static_cast<std::uint8_t>(n>>8u);
}
void put32(std::uint8_t* p,std::uint32_t n) {
    put16(p,static_cast<std::uint16_t>(n));
    put16(p+2u,static_cast<std::uint16_t>(n>>16u));
}
void callback_contract() {
    using namespace rloft::input::mouse_lifecycle;
    std::array<std::uint8_t,0xA7CCu> guest{};
    put16(guest.data()+0xA7ACu,0x000Cu);
    put16(guest.data()+0xA7B4u,0xFFFFu);
    put32(guest.data()+0xA7B8u,0x27894u);
    auto valid=[&] {
        return callback_frame_1a064(guest.data(),guest.size(),
                                    0x33u,0xA7ACu,0xA7C8u,0xA79Eu);
    };
    check(valid(),"original callback register frame");
    check(!callback_frame_1a064(nullptr,guest.size(),
                                0x33u,0xA7ACu,0xA7C8u,0xA79Eu) &&
          !callback_frame_1a064(guest.data(),guest.size()-1u,
                                0x33u,0xA7ACu,0xA7C8u,0xA79Eu),
          "missing or truncated guest");
    check(!callback_frame_1a064(guest.data(),guest.size(),
                                0x21u,0xA7ACu,0xA7C8u,0xA79Eu) &&
          !callback_frame_1a064(guest.data(),guest.size(),
                                0x33u,0xA7ACu,0xA7C8u,0xA79Fu),
          "invalid interrupt and segment frame");
    put32(guest.data()+0xA7B8u,0x27895u);
    check(!valid(),"invalid source ISR address");
    put32(guest.data()+0xA7B8u,0x27894u);
    check(may_install(true,0u) && !may_install(false,0u) &&
          !may_install(true,1u),"single installation");
    check(accepted_reply(true,0u,0u) &&
          !accepted_reply(true,0u,1u) &&
          !accepted_reply(true,1u,0u),"acknowledge successful source install only");
    check(result_pointer(0xA7C8u,guest.size()) &&
          !result_pointer(0xA7C9u,guest.size()) &&
          !result_pointer(0xA7C8u,guest.size()-1u),
          "original EAX result address");
}
void title_contract() {
    using namespace rloft::ui::title_interaction;
    std::array<std::uint8_t,0x2B7B0u> guest{};
    put16(guest.data()+0x2B7A8u,1u);
    put16(guest.data()+0x2B7ACu,155u);
    put16(guest.data()+0x2B7AEu,317u);
    MouseEvent event{};
    check(read_source_mouse_event(guest.data(),guest.size(),event),
          "original 33CB0 snapshot");
    check(event.event==1u && event.buttons==1u &&
          event.dos_x_doubled==310u && event.dos_y==317u &&
          event.reserved==0u,"retained 19D18 ABI");
    put16(guest.data()+0x2B7A8u,0u);
    put16(guest.data()+0x2B7ACu,0xFFFFu);
    check(read_source_mouse_event(guest.data(),guest.size(),event) &&
          event.buttons==0u && event.dos_x_doubled==0x1FFFEu,
          "no host-clamped cursor or fabricated click");
    check(!read_source_mouse_event(nullptr,guest.size(),event) &&
          !read_source_mouse_event(guest.data(),guest.size()-1u,event),
          "fail closed on invalid borrowed input snapshot");
    CallbackDispatch source{};
    check(source.event_mask({1u,0u,0x140u,0xC8u,0u})==0u,
          "original centered cursor cannot generate synthetic callback");
    check(source.event_mask({1u,0u,0x14Au,0xDCu,0u})==1u,
          "original fn0C movement reason");
    check(source.event_mask({1u,1u,0x14Au,0xDCu,0u})==2u,
          "original fn0C left press reason");
    check(source.event_mask({1u,1u,0x14Au,0xDCu,0u})==0u,
          "original fn0C holds do not repeat press");
    check(source.event_mask({1u,0u,0x14Au,0xDCu,0u})==4u,
          "original fn0C left release reason");
    check(source.event_mask({1u,2u,0x14Au,0xDCu,0u})==8u &&
          source.event_mask({1u,0u,0x14Au,0xDCu,0u})==16u,
          "original fn0C right button reasons");
    check(source.event_mask({1u,1u,0x150u,0xDDu,0u})==3u,
          "simultaneous motion and left press mask");
    check(!source_title_presented(0u) &&
          source_title_presented(1u) && source_title_presented(14u),
          "original title frame prerequisite");
}
}
int main() {
    try {
        callback_contract(); title_contract();
        std::cout<<"CODE2_TITLE_MOUSE_CALLBACK_HOST_ABI_PASS"
                 <<" source_event=1A1F4_19D18"
                 <<" source_callback=1A064_33A7C"
                 <<" mouse_driver_unregistration=UNMAPPED"
                 <<" retail_dos_parity=OPEN\n";
        return 0;
    }catch(const std::exception& e) {
        std::cerr<<"CODE2_TITLE_MOUSE_CALLBACK_HOST_ABI_FAIL: "
                 <<e.what()<<'\n';
        return 1;
    }
}
