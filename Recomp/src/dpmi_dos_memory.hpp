#pragma once
// Narrow, stateful DPMI 0.9 INT 31h AX=0100h/0101h DOS-memory service.
// This does not implement arbitrary DPMI selectors, protected-mode paging,
// real-mode INT simulation, mouse hardware or DOS/4GW heap growth.
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <map>
#include <vector>
#include "dpmi_selector_descriptors.hpp"

namespace rloft_recomp {
struct DpmiDosResult {
    bool carry;
    std::uint16_t ax; // success: real-mode segment; failure: error
    std::uint16_t dx; // success: selector
    std::uint16_t bx; // on allocation failure: largest free paragraphs
};
class DpmiDosMemory {
public:
    struct Block {
        std::uint16_t segment;
        std::uint16_t selector;
        std::uint16_t paragraphs;
    };
    // Deterministic guest-DOS arena; not an observed original DOS memory map.
    static constexpr std::uint32_t kFirstParagraph=0x2D60u;
    static constexpr std::uint32_t kLastParagraph=0xA000u;
    static constexpr std::uint16_t kFirstSelector=0x0060u;

    DpmiDosMemory(): real_mode_(kLastParagraph*16u,0u) {}

    // DPMI 0100: BX requested paragraphs, returned AX segment / DX selector.
    DpmiDosResult allocate(std::uint16_t paragraphs) {
        if(paragraphs==0u)return {true,0x0008u,0u,largest_free()};
        std::uint32_t candidate=kFirstParagraph;
        // Find the first free interval, including holes left by release.
        std::vector<Block> occupied;
        occupied.reserve(blocks_.size());
        for(const auto& pair:blocks_)occupied.push_back(pair.second);
        std::sort(occupied.begin(),occupied.end(),[](const Block& a,const Block& b){
            return a.segment<b.segment;
        });
        for(const auto& b:occupied) {
            if(candidate+paragraphs<=b.segment)break;
            const auto next=static_cast<std::uint32_t>(b.segment)+b.paragraphs;
            if(candidate<next)candidate=next;
        }
        if(candidate+paragraphs>kLastParagraph)
            return {true,0x0008u,0u,largest_free()};
        if(next_selector_>0xFFF8u)
            return {true,0x8011u,0u,largest_free()};
        const auto selector=static_cast<std::uint16_t>(next_selector_);
        next_selector_+=8u; // Unique, descriptor-aligned guest selectors.
        Block b{static_cast<std::uint16_t>(candidate),selector,paragraphs};
        // The descriptor is installed only for this live 0100 allocation.
        // The 0006 base is a linear address; the segment remains separate.
        if(!selectors_.install(selector,candidate*16u,
                               static_cast<std::uint32_t>(paragraphs)*16u))
            return {true,0x8011u,0u,largest_free()};
        blocks_.emplace(selector,b);
        // Fresh blocks are initialized for test determinism, not DOS parity.
        std::fill(real_mode_.begin()+candidate*16u,
                  real_mode_.begin()+(candidate+paragraphs)*16u,0u);
        return {false,b.segment,selector,0u};
    }

    // DPMI 0101: DX must identify a live DOS allocation, not a segment.
    DpmiDosResult release(std::uint16_t selector) {
        const auto it=blocks_.find(selector);
        if(it==blocks_.end())return {true,0x8022u,0u,0u};
        const auto& b=it->second;
        std::fill(real_mode_.begin()+static_cast<std::uint32_t>(b.segment)*16u,
                  real_mode_.begin()+
                    (static_cast<std::uint32_t>(b.segment)+b.paragraphs)*16u,0u);
        selectors_.retire(selector);
        blocks_.erase(it);
        return {false,0u,0u,0u};
    }

    // DPMI INT 31h AX=0006h: selector in BX, base in CX:DX.
    DpmiSelectorResult selector_base(std::uint16_t selector) const noexcept {
        return selectors_.get_base(selector);
    }
    const Block* block(std::uint16_t selector) const noexcept {
        const auto it=blocks_.find(selector);
        return it==blocks_.end()?nullptr:&it->second;
    }
    // Checked protected-mode descriptor view of the conventional memory.
    std::uint8_t* translate(std::uint16_t selector,std::uint32_t offset,
                            std::size_t count) noexcept {
        const auto* b=block(selector);
        if(!b || !selectors_.check(selector,offset,count))return nullptr;
        return real_mode_.data()+static_cast<std::uint32_t>(b->segment)*16u+offset;
    }
    std::uint16_t largest_free() const noexcept {
        std::vector<Block> occupied;
        occupied.reserve(blocks_.size());
        for(const auto& pair:blocks_)occupied.push_back(pair.second);
        std::sort(occupied.begin(),occupied.end(),[](const Block& a,const Block& b){
            return a.segment<b.segment;
        });
        std::uint32_t start=kFirstParagraph,max_run=0u;
        for(const auto& b:occupied) {
            if(b.segment>start)max_run=std::max(max_run,
                 static_cast<std::uint32_t>(b.segment)-start);
            start=std::max(start,static_cast<std::uint32_t>(b.segment)+b.paragraphs);
        }
        max_run=std::max(max_run,kLastParagraph-start);
        return static_cast<std::uint16_t>(max_run);
    }
    std::size_t allocation_count() const noexcept {return blocks_.size();}
private:
    std::map<std::uint16_t,Block> blocks_;
    DpmiSelectorDescriptors selectors_;
    std::vector<std::uint8_t> real_mode_;
    std::uint32_t next_selector_=kFirstSelector;
};
} // namespace rloft_recomp
