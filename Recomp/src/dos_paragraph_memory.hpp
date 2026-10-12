#pragma once
// Bounded host-owned DOS INT 21h AH=48h/49h/4Ah paragraph arena.
// One MCB paragraph is reserved per block; this is NOT a DOS/4GW
// extender's protected-mode selector or CODE.1's 4-KiB resize ABI.
// Errors are DOS AX=8 (insufficient memory), AX=9 (invalid block).
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <map>
#include <iterator>
#include <new>
#include <vector>

namespace rloft_recomp {
struct DosMemoryResult {
    bool carry;
    std::uint16_t ax; // allocation AX=segment, error AX=8/9
    std::uint16_t bx; // on failed resize/allocate: largest possible extent
};
class DosParagraphMemory {
public:
    static constexpr std::uint32_t kFirstMcb=0x1000u;
    static constexpr std::uint32_t kLimit=0xA000u; // exclusive

    struct Block {
        std::uint16_t segment;
        std::uint16_t paragraphs;
        std::vector<std::uint8_t> bytes;
    };

    DosMemoryResult allocate(std::uint16_t paragraphs) {
        if(paragraphs==0u)return {true,8u,largest_free()};
        std::uint32_t candidate=kFirstMcb+1u;
        for(const auto& item:blocks_) {
            const auto& block=item.second;
            const auto next_mcb=static_cast<std::uint32_t>(block.segment)-1u;
            if(candidate+paragraphs<=next_mcb)break;
            candidate=static_cast<std::uint32_t>(block.segment)+
                      block.paragraphs+1u;
        }
        if(candidate+paragraphs>kLimit)
            return {true,8u,largest_free()};
        Block incoming{static_cast<std::uint16_t>(candidate),paragraphs,{}};
        try {
            incoming.bytes.resize(static_cast<std::size_t>(paragraphs)*16u,0u);
            blocks_.emplace(incoming.segment,std::move(incoming));
        } catch(const std::bad_alloc&) {
            return {true,8u,largest_free()};
        }
        return {false,static_cast<std::uint16_t>(candidate),0u};
    }

    DosMemoryResult release(std::uint16_t segment) {
        auto it=blocks_.find(segment);
        if(it==blocks_.end())return {true,9u,0u};
        blocks_.erase(it);
        return {false,0u,0u};
    }

    DosMemoryResult resize(std::uint16_t segment,std::uint16_t paragraphs) {
        auto it=blocks_.find(segment);
        if(it==blocks_.end())return {true,9u,0u};
        if(paragraphs==0u)return {true,8u,it->second.paragraphs};
        const auto next=std::next(it);
        const std::uint32_t boundary=next==blocks_.end()?
            kLimit:static_cast<std::uint32_t>(next->second.segment)-1u;
        const auto available=boundary-static_cast<std::uint32_t>(segment);
        if(paragraphs>available)
            return {true,8u,static_cast<std::uint16_t>(available)};
        try {
            it->second.bytes.resize(static_cast<std::size_t>(paragraphs)*16u,0u);
        } catch(const std::bad_alloc&) {
            return {true,8u,static_cast<std::uint16_t>(available)};
        }
        it->second.paragraphs=paragraphs;
        return {false,0u,0u};
    }

    // This returns the conventional physical address of a DOS paragraph
    // allocation, not a 0006h protected-mode LDT descriptor lookup.
    std::uint32_t physical_base(std::uint16_t segment) const noexcept {
        return blocks_.count(segment)?static_cast<std::uint32_t>(segment)*16u:0u;
    }
    std::uint8_t* translate(std::uint32_t linear,std::size_t count) noexcept {
        for(auto& item:blocks_) {
            auto& b=item.second;
            const auto base=static_cast<std::uint32_t>(b.segment)*16u;
            if(linear<base)continue;
            const auto displacement=static_cast<std::uint64_t>(linear)-base;
            if(displacement<=b.bytes.size() &&
               count<=b.bytes.size()-displacement)
                return b.bytes.data()+static_cast<std::size_t>(displacement);
        }
        return nullptr;
    }
    const Block* block(std::uint16_t segment) const noexcept {
        const auto it=blocks_.find(segment);
        return it==blocks_.end()?nullptr:&it->second;
    }
    std::uint16_t largest_free() const noexcept {
        std::uint32_t candidate=kFirstMcb+1u,best=0u;
        for(const auto& item:blocks_) {
            const auto& b=item.second;
            if(b.segment>candidate+1u)
                best=std::max(best,static_cast<std::uint32_t>(b.segment)-1u-candidate);
            candidate=static_cast<std::uint32_t>(b.segment)+b.paragraphs+1u;
        }
        if(candidate<kLimit)best=std::max(best,kLimit-candidate);
        return static_cast<std::uint16_t>(best);
    }
    std::size_t allocation_count() const noexcept{return blocks_.size();}
private:
    std::map<std::uint16_t,Block> blocks_;
};
} // namespace rloft_recomp
