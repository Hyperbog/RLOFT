// Source-backed IRQ8 host-time dispatch regression.
// An injected steady_clock timestamp is ONLY a deterministic test clock;
// no DOS timer frequency or production hardware scheduling is certified.
#include "../../Code2/Code2.cpp"
#include "original_code2_irq8_elapsed_scheduler.hpp"
#include <chrono>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>
namespace {
std::uint32_t tick_callbacks=0u,chain_callbacks=0u;
bool ordered=true;
void on_source_tick(std::uint32_t old_tick) noexcept {
    if(old_tick!=40u+tick_callbacks)ordered=false;
    ++tick_callbacks;
}
void on_old_vector(std::uint32_t offset,std::uint16_t selector) noexcept {
    if(offset!=0x12345678u || selector!=0x4321u ||
       chain_callbacks!=tick_callbacks-1u)ordered=false;
    ++chain_callbacks;
}
void require(bool yes,const char* message) {
    if(!yes)throw std::runtime_error(message);
}
}
int main() {
    try {
        using rloft::original_irq8::ElapsedScheduler;
        using rloft::original_irq8::Status;
        using namespace std::chrono;
        std::vector<code2::u8> data(0x300000u,0u);
        code2::Machine guest{};
        guest.data=data.data();
        guest.cpu.cs=0x31u;
        guest.cpu.esp=0x2D000u;
        guest.cpu.interrupt_enable=true;
        code2::store_le32(data.data()+0xF58Cu,40u);
        code2::store_le32(data.data()+0xF582u,0x12345678u);
        code2::store_le16(data.data()+0xF586u,0x4321u);
        code2::Application71Boundary handlers{};
        handlers.call_19c80=&on_source_tick;
        handlers.far_chain=&on_old_vector;
        ElapsedScheduler timer;
        const auto start=ElapsedScheduler::Time{};
        const auto fire=[&]() noexcept {
            code2::interrupt_30708(guest,handlers);
        };
        require(!timer.arm(start,0u,4u) && !timer.armed(),
                "zero/unverified interrupt frequency was accepted");
        require(timer.poll(start,fire).status==Status::NotArmed,
                "disarmed IRQ8 unexpectedly fired");
        require(!timer.arm(start,8193u,4u) && !timer.armed(),
                "out-of-bounds frequency was accepted");
        require(timer.arm(start,500u,4u),"test clock arm failed");
        auto a=timer.poll(start+milliseconds(2),fire);
        require(a.status==Status::Delivered && a.interrupts==1u &&
                code2::function_30700(guest)==41u,
                "first elapsed interval did not invoke source IRQ8");
        a=timer.poll(start+milliseconds(10),fire);
        require(a.status==Status::Delivered && a.interrupts==4u &&
                code2::function_30700(guest)==45u,
                "fractional elapsed-clock calculation diverged");
        a=timer.poll(start+milliseconds(9),fire);
        require(a.status==Status::ClockReversed && a.interrupts==0u &&
                code2::function_30700(guest)==45u,
                "reversed elapsed clock rewrote source time");
        a=timer.poll(start+milliseconds(16),fire);
        require(a.status==Status::Delivered && a.interrupts==3u &&
                code2::function_30700(guest)==48u,
                "elapsed-clock scheduler dropped legitimate IRQ8s");
        a=timer.poll(start+milliseconds(28),fire);
        require(a.status==Status::ExcessiveBacklog && a.interrupts==0u &&
                timer.delivered()==8u && code2::function_30700(guest)==48u,
                "excessive catch-up silently forged or dropped ticks");
        require(ordered && tick_callbacks==8u && chain_callbacks==8u &&
                guest.cpu.cs==0x31u && guest.cpu.esp==0x2D000u &&
                guest.cpu.interrupt_enable,
                "source IRQ8 callback order, chain or CPU restoration changed");
        timer.disarm();
        require(!timer.armed() && timer.delivered()==0u &&
                timer.poll(start+milliseconds(30),fire).status==Status::NotArmed,
                "disarm did not prevent later interrupt dispatch");
        std::cout<<"ORIGINAL_30708_HOST_ELAPSED_SOURCE_IRQ8_PASS"
                 <<" source_tick_callbacks=8 chained_vectors=8"
                 <<" reject_clock_reversal=YES reject_backlog=YES"
                 <<" test_frequency_only=YES live_IRQ8=NOT_CONNECTED"
                 <<" retail_DOS_timer_parity=NOT_CERTIFIED\n";
        return 0;
    }catch(const std::exception& e) {
        std::cerr<<"FAIL "<<e.what()<<"\n";return 1;
    }
}
