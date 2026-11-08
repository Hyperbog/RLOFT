// Independent DOS 48h/49h/4Ah conventional-memory contract.
#include "dos_paragraph_memory.hpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>
using rloft_recomp::DosParagraphMemory;
using rloft_recomp::DosMemoryResult;
static void need(bool ok,const char* msg){if(!ok)throw std::runtime_error(msg);}
static void result(DosMemoryResult r,bool carry,unsigned ax,const char* msg) {
    need(r.carry==carry && r.ax==ax,msg);
}
int main() {
    try {
        DosParagraphMemory dos;
        result(dos.allocate(0),true,8u,"zero paragraph allocation");
        need(dos.allocation_count()==0u,"invalid allocation tracked");
        const auto a=dos.allocate(1u),b=dos.allocate(0x100u);
        need(!a.carry && !b.carry &&
             a.ax==0x1001u && b.ax==0x1003u &&
             dos.allocation_count()==2u,
             "DOS first-fit/MCB paragraph accounting");
        need(dos.physical_base(a.ax)==0x10010u &&
             dos.physical_base(b.ax)==0x10030u,
             "DOS paragraph physical-base address");
        auto* pa=dos.translate(dos.physical_base(a.ax),16u);
        auto* pb=dos.translate(dos.physical_base(b.ax),0x1000u);
        need(pa && pb && pa!=pb,"independent DOS guest regions");
        pa[15]=0x83u;pb[0]=0x55u;pb[0xFFFu]=0xFAu;
        need(dos.translate(dos.physical_base(a.ax)+15u,2u)==nullptr &&
             dos.translate(dos.physical_base(b.ax)+0x1000u,1u)==nullptr,
             "guest view crossed block boundary");
        result(dos.resize(a.ax,0),true,8u,"zero resize accepted");
        const auto denied=dos.resize(a.ax,2u);
        need(denied.carry && denied.ax==8u && denied.bx==1u &&
             pa[15]==0x83u && pb[0]==0x55u,
             "failed resize broke neighboring DOS region");
        result(dos.resize(0xFFFEu,2u),true,9u,"unknown resize segment");
        result(dos.release(0xFFFEu),true,9u,"unknown free segment");
        result(dos.release(a.ax),false,0u,"first free");
        result(dos.release(a.ax),true,9u,"double free accepted");
        need(dos.translate(0x10010u,1)==nullptr,"stale DOS mapping readable");
        const auto reused=dos.allocate(1u);
        need(!reused.carry && reused.ax==a.ax,"released gap not reused");
        need(dos.translate(0x10010u,1u)[0]==0u,
             "reused DOS block leaked prior bytes");
        result(dos.release(b.ax),false,0u,"second free");
        result(dos.resize(reused.ax,0x200u),false,0u,
               "DOS in-place 4Ah growth failed");
        auto* grown=dos.translate(dos.physical_base(reused.ax),0x2000u);
        need(grown!=nullptr && grown[15]==0u &&
             grown[0x1fffu]==0u,"in-place growth was not zero initialized");
        result(dos.resize(reused.ax,0x80u),false,0u,"DOS shrink failed");
        need(dos.translate(dos.physical_base(reused.ax)+0x800u,1u)==nullptr,
             "shrink left stale region");
        result(dos.release(reused.ax),false,0u,"final free");
        need(dos.allocation_count()==0u &&
             dos.largest_free()==0x8fffu,"paragraph arena not restored");
        // Separate original CODE.2 guest DGROUP from DOS conventional
        // physical memory. A checked host selector maps segment * 16 into
        // a distinct shared byte aperture; unowned segments stay invalid.
        constexpr std::uint32_t alias=0x300000u;
        std::vector<std::uint8_t> whole(alias+0xA0000u,0x69u);
        DosParagraphMemory mapped(whole.data()+alias,whole.size()-alias);
        const auto dos_a=mapped.allocate(0x100u);
        need(!dos_a.carry && dos_a.ax==0x1001u,
             "shared conventional memory allocation failed");
        const auto phys=mapped.physical_base(dos_a.ax);
        need(mapped.translate(phys,0x1000u)==whole.data()+alias+phys,
             "shared conventional bytes do not alias original guest image");
        need(whole[0x12624u]==0x69u,
             "shared DOS page allocation overwrote CODE.2 static DGROUP");
        whole[alias+phys+5u]=0x37u;
        need(mapped.translate(phys+5u,1u)[0]==0x37u,
             "guest write invisible through DOS segment provider");
        mapped.translate(phys+6u,1u)[0]=0xC8u;
        need(whole[alias+phys+6u]==0xC8u,
             "DOS segment provider write invisible through guest image");
        const auto too_big=mapped.resize(dos_a.ax,0x9000u);
        need(too_big.carry && too_big.ax==8u &&
             whole[alias+phys+5u]==0x37u,
             "failed shared DOS resize corrupted live guest data");
        need(!mapped.resize(dos_a.ax,0x200u).carry &&
             mapped.translate(phys,0x2000u)==whole.data()+alias+phys &&
             whole[alias+phys+0x1000u]==0u,
             "shared DOS growth failed to preserve or zero guest bytes");
        need(!mapped.resize(dos_a.ax,0x80u).carry &&
             mapped.translate(phys+0x800u,1u)==nullptr,
             "shared DOS shrink left stale translated pages");
        need(!mapped.release(dos_a.ax).carry &&
             mapped.translate(phys,1u)==nullptr &&
             mapped.allocation_count()==0u,
             "shared DOS release retained a guest segment");
        const auto dos_reuse=mapped.allocate(0x80u);
        need(!dos_reuse.carry && dos_reuse.ax==dos_a.ax &&
             mapped.translate(phys,16u)==whole.data()+alias+phys &&
             whole[alias+phys+5u]==0u,
             "shared DOS free/reallocate resurrected stale bytes");
        need(!mapped.release(dos_reuse.ax).carry,
             "shared DOS final release failed");
        std::cout<<"DOS_INT21_AH48_AH49_AH4A=PASS\n"
                    "DOS_MCB_GAPS_AND_RESIZE=CHECKED\n"
                    "DOS_MEM_SOURCE_PARITY=NOT_CLAIMED\n"
                    "R04_ACCEPTED=NO\n";
        return 0;
    }catch(const std::exception& e) {
        std::cerr<<"DOS_PARAGRAPH_MEMORY_FAIL: "<<e.what()<<'\n';
        return 1;
    }
}
