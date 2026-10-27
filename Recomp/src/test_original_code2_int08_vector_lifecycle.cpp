// Original CODE.2 IRQ0/INT 08h install/handler/restore contract.
// Instrumented old-vector chain: not real BIOS interrupt execution.
#include "../../Code2/Code2.cpp"
#include "original_code2_int08_vector_preflight.hpp"
#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>
namespace {
struct Vector { std::uint32_t offset; std::uint16_t selector; };
std::array<Vector,256> ivt{};
std::uint32_t installs=0,reads=0,notice=0,chain=0;
bool correct=true;
void check(bool yes,const char* message) {
    if(!yes)throw std::runtime_error(message);
}
void get(std::uint8_t n,std::uint32_t* off,std::uint16_t* sel) noexcept {
    ++reads;if(off)*off=ivt[n].offset;if(sel)*sel=ivt[n].selector;
}
void set(std::uint8_t n,std::uint32_t off,std::uint16_t sel) noexcept {
    ++installs;ivt[n]={off,sel};
}
void notify(std::uint32_t old_tick) noexcept {
    if(old_tick!=55u+notice)correct=false;
    ++notice;
}
void previous(std::uint32_t off,std::uint16_t sel) noexcept {
    if(off!=0x1200u || sel!=0x300u || chain!=notice-1u)
        correct=false;
    ++chain;
}
}
int main() {
    try {
        std::vector<code2::u8> memory(0x300000u,0u);
        code2::Machine guest{};guest.data=memory.data();
        guest.cpu.cs=0x17u;guest.cpu.esp=0x4321u;
        ivt[8]={0x1200u,0x300u};
        ivt[9]={0x1300u,0x300u};
        ivt[0]={0x1400u,0x300u};
        code2::Application71Boundary handlers{};
        handlers.get_vector=&get;handlers.set_vector=&set;
        handlers.call_19c80=&notify;handlers.far_chain=&previous;
        auto validation=[&] {
            return rloft::source_int08::validate(
                guest,memory.size(),ivt,handlers);
        };
        check(!validation().ready,
              "uninstalled source vector admitted");
        code2::function_30bb4(guest,handlers);
        check(validation().ready && reads==3u && installs==3u &&
              memory[0xF575u]==1u &&
              code2::load_le32(memory.data()+0xF582u)==0x1200u &&
              code2::load_le16(memory.data()+0xF586u)==0x300u,
              "original 30BB4 vector installation/storage diverged");
        code2::function_30bb4(guest,handlers);
        check(reads==3u && installs==3u,
              "original 30BB4 installation was not idempotent");
        ivt[8].offset=0x4444u;
        check(!validation().ready &&
              std::string(validation().reason)==
                  "ORIGINAL_INT08_RETAINED_IVT_MISMATCH",
              "unexpected live IVT replacement admitted");
        ivt[8].offset=0x30708u;
        auto missing=handlers;missing.far_chain=nullptr;
        check(!rloft::source_int08::validate(
                  guest,memory.size(),ivt,missing).ready,
              "unimplemented previous-vector execution admitted");
        missing=handlers;missing.call_19c80=nullptr;
        check(!rloft::source_int08::validate(
                  guest,memory.size(),ivt,missing).ready,
              "missing original timer hook admitted");
        code2::store_le32(memory.data()+0xF58Cu,55u);
        const auto prior=guest.cpu;
        code2::interrupt_30708(guest,handlers);
        code2::interrupt_30708(guest,handlers);
        check(validation().ready &&
              code2::function_30700(guest)==57u &&
              notice==2u && chain==2u && correct &&
              guest.cpu.cs==prior.cs && guest.cpu.esp==prior.esp,
              "original 30708 increment/chain/CPU restore diverged");
        code2::function_30c38(guest,handlers);
        check(memory[0xF575u]==0u && installs==6u &&
              ivt[8].offset==0x1200u && ivt[8].selector==0x300u &&
              ivt[9].offset==0x1300u && ivt[0].offset==0x1400u &&
              !validation().ready,
              "original 30C38 did not restore old vectors or revoke readiness");
        std::cout<<"ORIGINAL_30BB4_30708_30C38_INT08_LIFECYCLE_PASS"
                 <<" source_irq_callbacks=2 saved_vector_chains=2"
                 <<" original_restore=YES live_host=NOT_CONNECTED"
                 <<" BIOS_IRQ_PARITY=NOT_CERTIFIED\n";
        return 0;
    }catch(const std::exception& e) {
        std::cerr<<"FAIL "<<e.what()<<"\n";return 1;
    }
}
