#pragma once
// DPMI 0.9 INT 31h AX=0501h/0502h/0503h linear-memory transport.
// Linear addresses and opaque block handles are NOT DOS segments/selectors.
// Stateful host allocations; deterministic guest addresses, not DOS/4GW parity.
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <map>
#include <new>
#include <utility>
#include <vector>

namespace rloft_recomp {
struct DpmiLinearResult {
    bool carry;
    std::uint16_t ax;          // nonzero error only when carry is set
    std::uint32_t linear;      // BX:CX on successful 0501/0503
    std::uint32_t handle;      // SI:DI on successful 0501/0503
};
class DpmiLinearMemory {
public:
    static constexpr std::uint32_t kFirstLinear=0x01000000u;
    static constexpr std::uint32_t kPage=0x1000u;
    static constexpr std::uint32_t kBudget=16u*1024u*1024u;
    struct Block {
        std::uint32_t linear,requested,committed;
        std::vector<std::uint8_t> bytes;
    };

    // Optional caller-owned flat guest aperture. When present, the original
    // reconstructed source and DPMI 0501 backing share the SAME host bytes.
    // Addresses are guest offsets; no invalid host pointer casts are used.
    explicit DpmiLinearMemory(std::uint8_t* guest=nullptr,
                              std::size_t guest_size=0u)
        : guest_(guest),guest_size_(guest_size) {}

    DpmiLinearResult allocate(std::uint32_t bytes) {
        return replace(0u,bytes,false);
    }
    // Successful DPMI 0503 invalidates the old handle. Both the linear base
    // and the handle may change; this implementation deliberately relocates.
    DpmiLinearResult resize(std::uint32_t handle,std::uint32_t bytes) {
        if(blocks_.find(handle)==blocks_.end())return error(0x8023u);
        return replace(handle,bytes,true);
    }
    DpmiLinearResult release(std::uint32_t handle) {
        const auto it=blocks_.find(handle);
        if(it==blocks_.end())return error(0x8023u);
        committed_-=it->second.committed;
        blocks_.erase(it);
        return {false,0u,0u,0u};
    }
    const Block* block(std::uint32_t handle) const noexcept {
        const auto it=blocks_.find(handle);
        return it==blocks_.end()?nullptr:&it->second;
    }
    // Linear-address reads are bounds checked against committed guest pages.
    // They never accept a DPMI handle in place of an address.
    std::uint8_t* translate(std::uint32_t linear,std::size_t count) noexcept {
        for(auto& entry:blocks_) {
            Block& b=entry.second;
            if(linear<b.linear)continue;
            const auto offset=static_cast<std::uint64_t>(linear)-b.linear;
            if(offset<=b.committed && count<=b.committed-offset) {
                if(guest_) {
                    if(linear>guest_size_ || count>guest_size_-linear)
                        return nullptr;
                    return guest_+linear;
                }
                return b.bytes.data()+static_cast<std::size_t>(offset);
            }
        }
        return nullptr;
    }
    std::size_t allocation_count() const noexcept{return blocks_.size();}
    std::uint32_t committed_bytes() const noexcept{return committed_;}
private:
    static DpmiLinearResult error(std::uint16_t code) noexcept {
        return {true,code,0u,0u};
    }
    static std::uint64_t pages(std::uint32_t bytes) noexcept {
        return (static_cast<std::uint64_t>(bytes)+kPage-1u)&
               ~static_cast<std::uint64_t>(kPage-1u);
    }
    DpmiLinearResult replace(std::uint32_t old,std::uint32_t requested,
                             bool resizing) {
        if(requested==0u)return error(0x8021u);
        const auto prior=resizing?blocks_.find(old):blocks_.end();
        const auto prior_bytes=resizing?prior->second.committed:0u;
        const auto committed=pages(requested);
        if(committed>kBudget ||
           static_cast<std::uint64_t>(committed_)-prior_bytes+committed>kBudget ||
           next_linear_+committed>std::numeric_limits<std::uint32_t>::max() ||
           next_handle_>std::numeric_limits<std::uint32_t>::max() ||
           (guest_ && next_linear_+committed>guest_size_))
            return error(0x8012u);
        Block incoming{};
        incoming.linear=static_cast<std::uint32_t>(next_linear_);
        incoming.requested=requested;
        incoming.committed=static_cast<std::uint32_t>(committed);
        try {
            if(!guest_)
                incoming.bytes.resize(static_cast<std::size_t>(committed),0u);
        } catch(const std::bad_alloc&) {
            return error(0x8013u);
        }
        if(resizing) {
            const auto& original=prior->second;
            const auto keep=std::min(original.committed,incoming.committed);
            if(!guest_)
                std::copy_n(original.bytes.begin(),keep,incoming.bytes.begin());
        }
        // All expected rejections happen before retiring the old identity.
        const auto handle=static_cast<std::uint32_t>(next_handle_);
        blocks_.emplace(handle,std::move(incoming));
        if(guest_) {
            const auto newbase=static_cast<std::size_t>(next_linear_);
            if(resizing) {
                std::copy_n(guest_+prior->second.linear,
                            std::min(prior->second.committed,
                                     static_cast<std::uint32_t>(committed)),
                            guest_+newbase);
            }
            const auto keep=resizing?
                std::min(prior->second.committed,
                         static_cast<std::uint32_t>(committed)):0u;
            std::fill_n(guest_+newbase+keep,
                        static_cast<std::size_t>(committed)-keep,0u);
        }
        if(resizing)blocks_.erase(old);
        committed_=static_cast<std::uint32_t>(
            static_cast<std::uint64_t>(committed_)-prior_bytes+committed);
        next_linear_+=committed+kPage;  // guard-page-sized address gap
        ++next_handle_;                 // never alias a retired handle
        return {false,0u,static_cast<std::uint32_t>(
            next_linear_-committed-kPage),handle};
    }
    std::uint8_t* guest_=nullptr;
    std::size_t guest_size_=0u;
    std::map<std::uint32_t,Block> blocks_;
    std::uint32_t committed_=0u;
    std::uint64_t next_linear_=kFirstLinear;
    std::uint64_t next_handle_=0x00010001u;
};
} // namespace rloft_recomp
