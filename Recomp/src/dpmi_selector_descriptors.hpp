#pragma once
// DPMI 0.9 INT 31h AX=0006h: descriptor selector -> linear base.
// This table models host-owned descriptor state, not physical LDT hardware.
// Memory ownership is separate: a selector cannot own a DPMI 0501 block.
#include <cstddef>
#include <cstdint>
#include <limits>
#include <map>

namespace rloft_recomp {
struct DpmiSelectorResult {
    bool carry;
    std::uint16_t ax; // 0006 failure code (invalid selector = 8022h).
    std::uint16_t cx; // high 16 bits of base on success.
    std::uint16_t dx; // low 16 bits of base on success.
    std::uint32_t base() const noexcept {
        return (static_cast<std::uint32_t>(cx)<<16u)|dx;
    }
};

class DpmiSelectorDescriptors {
public:
    struct Descriptor {
        std::uint32_t base;
        std::uint32_t length; // accessible byte count, NOT inclusive limit.
    };

    // Called only by the guest loader or a genuine host DPMI 0100 provider.
    // Reject overlapping selector identities and overflowing linear ranges.
    bool install(std::uint16_t selector,std::uint32_t base,
                 std::uint32_t length) {
        if(selector==0u || length==0u ||
           static_cast<std::uint64_t>(base)+length >
               static_cast<std::uint64_t>(
                   std::numeric_limits<std::uint32_t>::max())+1u)
            return false;
        return entries_.emplace(selector,Descriptor{base,length}).second;
    }
    bool retire(std::uint16_t selector) noexcept {
        return entries_.erase(selector)==1u;
    }
    // DPMI 0006: return base in CX:DX, never the numeric selector.
    DpmiSelectorResult get_base(std::uint16_t selector) const noexcept {
        const auto it=entries_.find(selector);
        if(it==entries_.end())return {true,0x8022u,0u,0u};
        const auto base=it->second.base;
        return {false,0u,
                static_cast<std::uint16_t>(base>>16u),
                static_cast<std::uint16_t>(base)};
    }
    const Descriptor* descriptor(std::uint16_t selector) const noexcept {
        const auto it=entries_.find(selector);
        return it==entries_.end()?nullptr:&it->second;
    }
    // Validates an offset/count in selector-local space. One-past-end is
    // only valid for a zero-length operation.
    bool check(std::uint16_t selector,std::uint32_t offset,
               std::size_t count) const noexcept {
        const auto* d=descriptor(selector);
        return d && offset<=d->length &&
               count<=static_cast<std::size_t>(d->length-offset);
    }
    std::size_t size() const noexcept {return entries_.size();}
private:
    std::map<std::uint16_t,Descriptor> entries_;
};
} // namespace rloft_recomp
