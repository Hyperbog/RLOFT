// Executes reconstructed CODE.2 27894 -> 19D18, not a substitute menu.
#include "../../Code2/Code2.cpp"
#include "game/input/MouseDispatch.hpp"
#include "game/ui/TitleInteraction.hpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>
namespace {
using u8=std::uint8_t;
using u32=std::uint32_t;
void require(bool ok,const char* why) {
    if(!ok)throw std::runtime_error(why);
}
struct Gate {
    using Sample=rloft::input::mouse_dispatch::Sample;
    std::vector<u8> data=std::vector<u8>(0x40000u,0u);
    code2::Machine machine{};
    code2::Application54Boundary isr{};
    code2::Application34Boundary source_mouse{};
    rloft::input::mouse_dispatch::Mailbox inbox{};
    rloft::ui::title_interaction::CallbackDispatch classify{};
    unsigned calls=0u,reads=0u,writes=0u;
    u32 last_flags=0u;
    static Gate* active;
    static Gate& self() noexcept {return *active;}
    Gate() {
        require(!active,"one original guest per ISR fixture");
        active=this;machine.data=data.data();
        machine.cpu.ds=0x20u;machine.cpu.ss=0x30u;
        machine.cpu.cs=0x40u;machine.cpu.esp=0xC000u;
        code2::store_le16(data.data()+0xCF48u,1u);
        code2::store_le16(data.data()+0x9E37u,1u);
        source_mouse.call_2e34c=[]() noexcept -> u32 {return 0u;};
        isr.in8=[](code2::u16) noexcept -> u8 {
            ++self().reads;return 0u;
        };
        isr.out8=[](code2::u16,u8) noexcept {++self().writes;};
        isr.out16=[](code2::u16,code2::u16) noexcept {
            ++self().writes;
        };
        isr.call_19d18=[](u32 flags,u32 buttons,u32 x,u32 y,
                            u32 sample_ptr) noexcept -> u32 {
            auto& g=self();
            ++g.calls;g.last_flags=flags;
            code2::function_19d18(g.machine,g.source_mouse,flags,buttons,
                                   x,y,sample_ptr);
            return 0u;
        };
    }
    ~Gate(){active=nullptr;}
    void deliver(const Sample& sample) {
        const rloft::ui::title_interaction::MouseEvent event{
            1u,sample.buttons,sample.x,sample.y,0u};
        const auto flags=classify.event_mask(event);
        if(!flags)return;
        const auto prior=machine.cpu;
        const auto original_nested_guard=data[0xD910u];
        machine.cpu.eax=flags;machine.cpu.ebx=sample.buttons;
        machine.cpu.ecx=sample.x;machine.cpu.edx=sample.y;machine.cpu.esi=0u;
        code2::function_27894(machine,isr);
        require(machine.cpu.eax==flags &&
                machine.cpu.ebx==sample.buttons &&
                machine.cpu.ecx==sample.x &&
                machine.cpu.edx==sample.y &&
                machine.cpu.ss==prior.ss &&
                machine.cpu.esp==prior.esp &&
                machine.cpu.ds==prior.ds &&
                data[0xD910u]==original_nested_guard,
                "original 27894 register/stack/guard restoration");
        machine.cpu=prior;
    }
    void run() {
        Sample sample{};
        inbox.publish({0u,0x140u,0xC8u});
        require(inbox.consume(sample),"initial fn03 sample");
        deliver(sample);
        require(calls==0u,"initial center cannot create a fake IRQ");
        inbox.publish({0u,0x14Au,0xDCu});
        require(inbox.consume(sample),"mouse move from physical mailbox");
        deliver(sample);
        require(calls==1u && last_flags==1u &&
                code2::load_le16(data.data()+0xA6B4u)==0xDCu &&
                code2::load_le16(data.data()+0xA6B6u)==0xA5u,
                "source 19D18 original movement state through 27894");
        inbox.publish({1u,0x14Au,0xDCu});
        inbox.publish({0u,0x14Au,0xDCu});
        require(inbox.consume(sample) && sample.buttons==1u,
                "physical down retained");
        deliver(sample);
        require(calls==2u && last_flags==2u,
                "source 27894 left-down event reason");
        require(inbox.consume(sample) && sample.buttons==0u,
                "physical up retained");
        deliver(sample);
        require(calls==3u && last_flags==4u,
                "source 27894 left-up event reason");
        require(reads>=12u && writes>=12u,
                "original 277F0/27844 VGA port register lifecycle");
        // Source 27894 has its own nested-interrupt guard.
        data[0xD910u]=1u;
        const auto before=calls;
        inbox.publish({0u,0x150u,0xDCu});
        require(inbox.consume(sample),"guard test input");
        deliver(sample);
        require(calls==before && data[0xD910u]==1u,
                "original 27894 nested callback must be ignored");
        data[0xD910u]=0u;
        std::cout<<"CODE2_ORIGINAL_27894_19D18_MOUSE_SOURCE_PASS"
                 <<" original_isr=EXECUTED"
                 <<" cpu_stack=RESTORED"
                 <<" vga_ports=SAVE_RESTORE"
                 <<" physical_edges=RETAINED"
                 <<" original_title_selection=NOT_PROVEN"
                 <<" DOS_IRQ_TIMING=NOT_PROVEN\n";
    }
};
Gate* Gate::active=nullptr;
}
int main() {
    try {Gate gate;gate.run();return 0;}
    catch(const std::exception& e) {
        std::cerr<<"CODE2_ORIGINAL_27894_MOUSE_SOURCE_FAIL: "
                 <<e.what()<<'\n';return 1;
    }
}
