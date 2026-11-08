// Independent stateful DPMI DOS allocator tests, no original assets.
#include "dpmi_dos_memory.hpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>
using rloft_recomp::DpmiDosMemory;
using rloft_recomp::DpmiDosResult;
void need(bool yes,const char* why) {if(!yes)throw std::runtime_error(why);}
void expect(DpmiDosResult r,bool carry,std::uint16_t ax,
            std::uint16_t dx,const char* why) {
    need(r.carry==carry && r.ax==ax && r.dx==dx,why);
}
int main() {
    try {
        DpmiDosMemory dpmi;
        need(dpmi.allocation_count()==0u,"initial allocations");
        const auto first=dpmi.allocate(5);
        expect(first,false,0x2D60u,0x0060u,"first real-mode segment/selector");
        need(dpmi.allocation_count()==1u,"first allocation not tracked");
        const auto first_base=dpmi.selector_base(first.dx);
        need(!first_base.carry && first_base.base()==
             static_cast<std::uint32_t>(first.ax)*16u &&
             first_base.cx==0x0002u && first_base.dx==0xD600u,
             "0100 selector descriptor did not return expected 0006 base");
        need(dpmi.selector_base(first.ax).carry,
             "DOS real-mode segment incorrectly accepted as DPMI selector");
        const auto* block=dpmi.block(first.dx);
        need(block && block->segment==first.ax && block->paragraphs==5u,
             "selector descriptor does not describe 5 paragraphs");
        need(dpmi.translate(first.dx,0,80)!=nullptr,"full block view rejected");
        need(dpmi.translate(first.dx,79,1)!=nullptr,"last byte rejected");
        need(dpmi.translate(first.dx,80,1)==nullptr,"out-of-bounds accepted");
        need(dpmi.translate(first.dx,0,81)==nullptr,"overlong mapping accepted");
        need(dpmi.translate(0x30u,0,1)==nullptr,"foreign selector accepted");
        auto* p=dpmi.translate(first.dx,0,80);
        for(unsigned i=0;i<80;++i)p[i]=static_cast<std::uint8_t>(i);
        const auto second=dpmi.allocate(6);
        expect(second,false,0x2D65u,0x0068u,"second allocation overlaps");
        const auto* second_block=dpmi.block(second.dx);
        need(second_block && second_block->paragraphs==6u,
             "second allocation metadata");
        auto* q=dpmi.translate(second.dx,0,96);
        need(q && q!=p,"independent DOS block pointers");
        for(unsigned i=0;i<96;++i)q[i]=0xA5u;
        for(unsigned i=0;i<80;++i)
            need(p[i]==i,"second write corrupted first DOS block");
        expect(dpmi.release(first.ax),true,0x8022u,0u,
               "real-mode segment incorrectly accepted as selector");
        expect(dpmi.release(first.dx),false,0u,0u,"first release failed");
        need(dpmi.translate(first.dx,0,1)==nullptr,"stale selector translated");
        need(dpmi.selector_base(first.dx).carry &&
             dpmi.selector_base(second.dx).base()==
                 static_cast<std::uint32_t>(second.ax)*16u,
             "0101 selector retired incorrectly or corrupted live neighbor");
        expect(dpmi.release(first.dx),true,0x8022u,0u,"double release accepted");
        const auto reused=dpmi.allocate(5);
        expect(reused,false,0x2D60u,0x0070u,"hole reuse or descriptor reuse");
        need(dpmi.selector_base(reused.dx).base()==
                 static_cast<std::uint32_t>(reused.ax)*16u &&
             dpmi.selector_base(first.dx).carry,
             "0006 descriptor was not rebound with a fresh selector");
        auto* z=dpmi.translate(reused.dx,0,80);
        need(z!=nullptr,"reused block not translated");
        for(unsigned i=0;i<80;++i)
            need(z[i]==0u,"reused block contained stale guest data");
        need(q[0]==0xA5u && q[95]==0xA5u,"hole reuse corrupted live neighbor");
        expect(dpmi.allocate(0),true,8u,0u,"zero size allocation accepted");
        expect(dpmi.allocate(0xFFFF),true,8u,0u,"oversized allocation accepted");
        need(dpmi.allocation_count()==2u,"rejections mutated live blocks");
        need(dpmi.largest_free()>=0x1000u,"free space accounting");
        expect(dpmi.release(second.dx),false,0u,0u,"second release");
        expect(dpmi.release(reused.dx),false,0u,0u,"reused release");
        need(dpmi.allocation_count()==0u,"descriptor leak");
        need(dpmi.selector_base(second.dx).carry &&
             dpmi.selector_base(reused.dx).carry,
             "0006 descriptors remained live after 0101 release");
        need(dpmi.largest_free()==
             DpmiDosMemory::kLastParagraph-DpmiDosMemory::kFirstParagraph,
             "arena not fully restored after release");
        // Unified loader / DPMI table: the original guest DS selector,
        // two independently allocated DPMI 0100 descriptors and a third
        // unrelated DOS-fallback descriptor must coexist. Releasing a
        // DOS allocation must not retire loader or other owner identities.
        rloft_recomp::DpmiSelectorDescriptors loader;
        need(loader.install(0x30u,0x50000u,0x2DD10u) &&
             loader.install(0x1003u,0x310030u,0x1000u),
             "shared selector loader initial descriptors failed");
        DpmiDosMemory shared(&loader);
        need(shared.allocation_count()==0u && loader.size()==2u,
             "shared 0100 provider created test-only DOS allocations");
        const auto a=shared.allocate(0x12u);
        const auto b=shared.allocate(0x07u);
        need(!a.carry && !b.carry && a.dx==0x60u && b.dx==0x68u &&
             loader.size()==4u &&
             loader.get_base(a.dx).base()==
                 static_cast<std::uint32_t>(a.ax)*16u &&
             loader.get_base(b.dx).base()==
                 static_cast<std::uint32_t>(b.ax)*16u &&
             loader.check(a.dx,0u,0x120u) &&
             !loader.check(a.dx,0x120u,1u),
             "shared DPMI 0006, 0100 selector and limit not unified");
        need(!shared.release(a.dx).carry && loader.size()==3u &&
             loader.get_base(a.dx).carry && !loader.get_base(b.dx).carry &&
             !loader.get_base(0x30u).carry &&
             !loader.get_base(0x1003u).carry,
             "shared descriptor release invalidated unrelated selector");
        need(!shared.release(b.dx).carry && loader.size()==2u &&
             loader.get_base(b.dx).carry &&
             loader.get_base(0x30u).base()==0x50000u &&
             loader.check(0x1003u,0xFFFu,1u) &&
             !loader.check(0x1003u,0x1000u,1u) &&
             loader.retire(0x1003u) &&
             loader.get_base(0x1003u).carry &&
             loader.size()==1u,
             "shared DPMI selector lifetime leaked or wrong owner retired");
        need(loader.retire(0x30u) && loader.size()==0u,
             "shared loader teardown leaked final descriptor");
        std::cout<<"DPMI_DOS_MEMORY_AX0100_AX0101=PASS\n"
                    "DPMI_LIVE_SELECTORS_AFTER_FREE=0\n"
                    "DPMI_GUEST_TRANSLATION_CHECKED=YES\n"
                    "SOURCE_GAME_BOOT=NO\n"
                    "ORIGINAL_DOS_PARITY=NO\n";
    }catch(const std::exception& e) {
        std::cerr<<"DPMI_DOS_MEMORY_FAIL: "<<e.what()<<'\n';
        return 1;
    }
    return 0;
}
