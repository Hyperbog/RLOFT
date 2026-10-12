// DPMI 0.9 linear-memory service regressions. No original game assets.
#include "dpmi_linear_memory.hpp"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <stdexcept>
using rloft_recomp::DpmiLinearMemory;
using rloft_recomp::DpmiLinearResult;
static void need(bool ok,const char* reason) {
    if(!ok)throw std::runtime_error(reason);
}
static void failure(DpmiLinearResult r,std::uint16_t error,
                    const char* reason) {
    need(r.carry && r.ax==error && r.linear==0u && r.handle==0u,reason);
}
int main() {
    try {
        DpmiLinearMemory service;
        failure(service.allocate(0),0x8021u,"0501 zero size");
        failure(service.allocate(DpmiLinearMemory::kBudget+1u),
                0x8012u,"0501 impossible request");
        need(service.allocation_count()==0u,"rejection allocated a block");
        const auto a=service.allocate(0x1000u);
        const auto b=service.allocate(0x1001u);
        need(!a.carry && !b.carry && a.ax==0u && b.ax==0u,
             "0501 must allocate both blocks");
        need(a.linear==DpmiLinearMemory::kFirstLinear &&
             (a.linear&0xFu)==0u && (b.linear&0xFu)==0u,
             "linear addressing is not paragraph aligned");
        need(a.handle!=a.linear && b.handle!=b.linear &&
             a.handle!=b.handle && a.linear!=b.linear,
             "handle/linear address alias or collision");
        need(service.block(a.handle)->requested==0x1000u &&
             service.block(a.handle)->committed==0x1000u &&
             service.block(b.handle)->requested==0x1001u &&
             service.block(b.handle)->committed==0x2000u,
             "DPMI page accounting differs");
        need(service.committed_bytes()==0x3000u,"page commitment total");
        auto* pa=service.translate(a.linear,0x1000u);
        auto* pb=service.translate(b.linear,0x2000u);
        need(pa && pb && pa!=pb,"guest pages not mapped separately");
        need(service.translate(a.handle,1u)==nullptr,
             "memory-block handle incorrectly interpreted as linear address");
        need(service.translate(a.linear+0x1000u,1u)==nullptr,
             "out of range access was accepted");
        need(service.translate(b.linear+0x2000u,1u)==nullptr,
             "range crossed into unowned page");
        for(unsigned i=0;i<0x1000u;++i)
            pa[i]=static_cast<std::uint8_t>(i);
        pb[0]=0xA5u;pb[0x1fffu]=0x5Au;
        failure(service.resize(a.handle,0u),0x8021u,"0503 zero size");
        failure(service.resize(0xdeadbeefu,1024u),0x8023u,
                "0503 unknown handle");
        failure(service.resize(a.handle,DpmiLinearMemory::kBudget),
                0x8012u,"0503 oversubscription");
        need(service.block(a.handle) && service.committed_bytes()==0x3000u,
             "failed 0503 destroyed original allocation");
        const auto grown=service.resize(a.handle,0x3001u);
        need(!grown.carry && grown.handle!=a.handle && grown.linear!=a.linear,
             "0503 did not replace both address and handle");
        need(service.block(a.handle)==nullptr &&
             service.translate(a.linear,1u)==nullptr,
             "retired 0503 identity remained valid");
        auto* pg=service.translate(grown.linear,0x4000u);
        need(pg!=nullptr && service.committed_bytes()==0x6000u,
             "0503 page growth accounting");
        for(unsigned i=0;i<0x1000u;++i)
            need(pg[i]==static_cast<std::uint8_t>(i),
                 "0503 growth lost previous content");
        for(unsigned i=0x1000u;i<0x4000u;++i)
            need(pg[i]==0u,"0503 growth did not zero new pages");
        need(pb[0]==0xA5u && pb[0x1fffu]==0x5Au,
             "0503 growth changed independent block");
        const auto shrunk=service.resize(grown.handle,0x800u);
        need(!shrunk.carry && shrunk.handle!=grown.handle &&
             shrunk.linear!=grown.linear && service.block(grown.handle)==nullptr,
             "0503 shrink failed to retire old handle");
        auto* ps=service.translate(shrunk.linear,0x1000u);
        need(ps!=nullptr && service.committed_bytes()==0x3000u,
             "0503 shrink commitment accounting");
        for(unsigned i=0;i<0x800u;++i)
            need(ps[i]==static_cast<std::uint8_t>(i),
                 "0503 shrink lost retained contents");
        failure(service.release(grown.handle),0x8023u,
                "0502 permitted already-resized handle");
        failure(service.release(a.handle),0x8023u,
                "0502 permitted original stale handle");
        need(!service.release(shrunk.handle).carry &&
             !service.release(b.handle).carry,
             "0502 live memory release failed");
        failure(service.release(shrunk.handle),0x8023u,
                "0502 double release accepted");
        need(service.allocation_count()==0u &&
             service.committed_bytes()==0u &&
             service.translate(shrunk.linear,1u)==nullptr &&
             service.translate(b.linear,1u)==nullptr,
             "0502 release left live address space");
        const auto fresh=service.allocate(0x1000u);
        need(!fresh.carry && fresh.handle!=a.handle &&
             fresh.handle!=grown.handle && fresh.handle!=shrunk.handle &&
             fresh.linear!=a.linear && fresh.linear!=shrunk.linear,
             "new allocation revived an invalidated address or handle");
        need(!service.release(fresh.handle).carry,"final 0502 failed");
        std::cout<<"DPMI_LINEAR_0501_0502_0503=PASS\n"
                   "DPMI_0503_RELOCATES_HANDLE_AND_LINEAR=PASS\n"
                   "DPMI_INVALID_HANDLES_FAIL_CLOSED=PASS\n"
                   "ORIGINAL_HEAP_GROWTH_INTEGRATED=NO\n"
                   "ORIGINAL_DOS_PARITY=NO\n";
    }catch(const std::exception& e) {
        std::cerr<<"DPMI_LINEAR_MEMORY_FAIL: "<<e.what()<<'\n';
        return 1;
    }
    return 0;
}
