// Full original 2E608 / 2E614 timed transition driven by genuine
// 30708 IRQ8 ticks and original 2F868 event service. No host clock
// substitution and no live game-over time-service certification.
#include "../../Code2/Code2.cpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>
namespace {
code2::Machine* retained=nullptr;
std::uint32_t serviced=0u, source_interrupts=0u;
void check(bool pass,const char* reason) {
    if(!pass)throw std::runtime_error(reason);
}
std::uint32_t clock() noexcept {
    return code2::function_30700(*retained);
}
void pump() noexcept {
    code2::Application70Boundary services{};
    code2::function_2f868(*retained,services);
    // In this isolated regression an externally scheduled interrupt is
    // represented by its ACTUAL compiled IRQ8 handler, not a fake tick.
    code2::Application71Boundary irq{};
    code2::interrupt_30708(*retained,irq);
    ++serviced;
    ++source_interrupts;
}
}
int main() {
    try {
        std::vector<code2::u8> memory(0x300000u,0u);
        code2::Machine guest{};
        guest.data=memory.data();retained=&guest;
        code2::store_le32(memory.data()+0xF58Cu,100u);
        code2::Application67Boundary b{};
        b.call_30700=&clock;
        b.call_2f868=&pump;
        code2::function_2e608(guest,b);
        check(code2::load_le32(memory.data()+0x2B640u)==100u,
              "source 2E608 did not save guest clock");
        const auto result=code2::function_2e614(guest,b,0x12u,0u);
        check(result==0u && clock()==118u &&
              serviced==18u && source_interrupts==18u &&
              code2::load_le32(memory.data()+0x2B640u)==0u,
              "original 2E614 deadline/IRQ8 progression changed");
        // Original 2E614 computes a new deadline once the frame
        // token has been reset; no host advancing time implicitly.
        const auto again=code2::function_2e614(guest,b,4u,0u);
        check(again==0u && clock()==122u && serviced==22u &&
              code2::load_le32(memory.data()+0x2B640u)==0u,
              "source 2E614 fallback clock deadline diverged");
        std::cout<<"ORIGINAL_2E608_2E614_IRQ8_SOURCE_TIME_PASS"
                 <<" compiled_IRQ8=22 original_event_pumps=22"
                 <<" synthetic_scheduler=YES"
                 <<" live_0CBC8_timing=GUARDED DOS_parity=NOT_CERTIFIED\n";
        return 0;
    }catch(const std::exception& e) {
        std::cerr<<"FAIL "<<e.what()<<"\n";
        return 1;
    }
}
