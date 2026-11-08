// Original CODE.2 title physical mouse transport; no fabricated guest menu.
#include "game/input/MouseDispatch.hpp"
#include <iostream>
#include <stdexcept>
using rloft::input::mouse_dispatch::Mailbox;
using rloft::input::mouse_dispatch::Sample;
void check(bool ok,const char* message) {
    if(!ok)throw std::runtime_error(message);
}
int main() {
    try {
        Mailbox mouse{};
        Sample sample{};
        mouse.publish({1u,211u,250u});
        mouse.publish({0u,211u,250u});
        check(mouse.consume(sample) && sample.buttons==1u &&
              sample.x==211u && sample.y==250u,
              "quick press edge was lost");
        check(mouse.consume(sample) && sample.buttons==0u,
              "quick release edge was lost");
        check(mouse.consume(sample) && sample.buttons==0u,
              "idle sample fabricated a press");
        for(unsigned i=0;i<64u;++i)mouse.publish({0u,100u+i,200u});
        check(mouse.consume(sample) && sample.x==163u,
              "pointer-only movement should coalesce");
        mouse.publish({1u,250u,260u});
        mouse.cancel_buttons();
        check(mouse.consume(sample) && sample.buttons==0u,
              "focus cancellation replayed button down");
        for(unsigned i=0;i<65u;++i)
            mouse.publish({(i&1u)?0u:1u,260u,270u});
        check(!mouse.consume(sample),"edge overflow must fail closed");
        std::cout<<"RLOFT_MOUSE_DISPATCH_MAILBOX_PASS"
                 <<" quick_click=RETAINED"
                 <<" motion=COALESCED"
                 <<" focus=RELEASED"
                 <<" overflow=REJECTED\n";
        return 0;
    }catch(const std::exception& error) {
        std::cerr<<"RLOFT_MOUSE_DISPATCH_MAILBOX_FAIL: "<<error.what()<<'\n';
        return 1;
    }
}
