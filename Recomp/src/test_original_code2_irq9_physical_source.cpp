// Original CODE.2 308F8 -> 30B40 PC keyboard path, with bounded
// synthetic 8042/PIC callbacks. Verifies SOURCE IRQ9 logic, not DOS timing.
#include "../../Code2/Code2.cpp"
#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
using code2::u8;using code2::u16;using code2::u32;
struct Vector {u32 offset=0;u16 selector=0;};
std::array<Vector,256> ivt{};
u8 host_scan=0u,port61=0u;
unsigned eois=0u,ack_writes=0u;
void require(bool b,const char* error) {
    if(!b)throw std::runtime_error(error);
}
void get_vector(u8 n,u32* offset,u16* selector) noexcept {
    if(offset)*offset=ivt[n].offset;
    if(selector)*selector=ivt[n].selector;
}
void set_vector(u8 n,u32 offset,u16 selector) noexcept {
    ivt[n]={offset,selector};
}
u8 in8(u16 port) noexcept {
    if(port==0x60u)return host_scan;
    if(port==0x61u)return port61;
    std::terminate();
}
void out8(u16 port,u8 value) noexcept {
    if(port==0x61u) {
        ++ack_writes;
        port61=value;
    } else if(port==0x20u && value==0x20u) {
        ++eois;
    } else std::terminate();
}
void source_event(code2::Machine& m,code2::Application71Boundary& irq,u8 scan) {
    host_scan=scan;
    port61=0u;
    const unsigned was=eois;
    code2::interrupt_308f8(m,irq);
    require(eois==was+1u && ack_writes==2u*eois &&
            port61==0u,"Original IRQ9 PIC/8042 acknowledgment changed");
}
}
int main() {
    try {
        std::vector<u8> memory(0x300000u,0u);
        code2::Machine m{};
        m.data=memory.data();
        m.cpu.cs=0x17u;
        code2::Application71Boundary irq{};
        irq.get_vector=&get_vector;
        irq.set_vector=&set_vector;
        irq.in8=&in8;
        irq.out8=&out8;
        code2::function_30bb4(m,irq);
        require(memory[0xF575u]==1u &&
                ivt[9u].offset==0x308F8u &&
                ivt[9u].selector==0x17u,
                "Original 30BB4 did not install CODE.2 IRQ9");

        // R makes a one-shot command. Translate using authentic compiled
        // 30B40's guest F590[0x13], not BIOS ASCII+scan assumptions.
        code2::store_le32(memory.data()+0xF590u+0x13u*4u,0x52u);
        source_event(m,irq,0x13u);
        require(memory[0xF572u]==0x13u,
                "Original IRQ9 did not publish R make scan");
        const u32 mapped_r=code2::function_30b40(m,irq);
        require(mapped_r==0x52u,"Original F590 command translation changed");
        source_event(m,irq,0x93u);
        require(memory[0xF572u]==0u &&
                memory[0xF571u]==0u &&
                memory[0xF588u]==0u,
                "Original R break did not release keyboard state");

        // The same ISR also owns cursor/navigation held key records;
        // publishing F572 directly would skip original 307F0/30748.
        code2::store_le32(memory.data()+0xF590u+0x48u*4u,0x148u);
        source_event(m,irq,0x48u);
        require(memory[0xF572u]==0x48u &&
                memory[0xF571u]==0x48u &&
                memory[0xF588u]==1u &&
                memory[0x2B6C4u]==0x48u &&
                code2::load_le16(memory.data()+0x258FCu)==3u &&
                code2::load_le16(memory.data()+0x258FEu)==0xFFFFu &&
                code2::function_308bc(m,0x48u)==1u,
                "Original 307F0 held-navigation make side effects missing");
        require(code2::function_30b40(m,irq)==0x148u,
                "Original held navigation key F590 mapping changed");
        source_event(m,irq,0xC8u);
        require(memory[0xF572u]==0u &&
                memory[0xF571u]==0u &&
                memory[0xF588u]==0u &&
                code2::function_308bc(m,0x48u)==0u,
                "Original 30748 held-navigation break did not drain source key");

        // Genuine source modifier lifetime; no invented host F573 flags.
        source_event(m,irq,0x1Du);
        require(code2::function_30abc(m,8u)==8u,
                "Original CTRL make flag absent");
        source_event(m,irq,0x9Du);
        require(code2::function_30abc(m,8u)==0u,
                "Original CTRL break flag stuck");
        require(eois==6u && ack_writes==12u,
                "Original IRQ9 did not acknowledge six source events");
        std::cout<<"ORIGINAL_CODE2_308F8_30B40_PHYSICAL_SOURCE_PASS"
                 <<" original_make_break_events="<<eois
                 <<" original_8042_ack_writes="<<ack_writes
                 <<" R_F590="<<mapped_r
                 <<" held_navigation_queue=ORIGINAL_307F0_30748"
                 <<" CTRL_modifiers=ORIGINAL_30ABC"
                 <<" original_vectors=30BB4"
                 <<" DOS_IRQ_TIMING_PARITY=OPEN\n";
        return 0;
    }catch(const std::exception& e) {
        std::cerr<<e.what()<<"\n";
        return 1;
    }
}
