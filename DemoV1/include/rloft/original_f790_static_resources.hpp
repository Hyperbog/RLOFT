#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace rloft {

// Original CODE.2 LE object 2's first disk page is at 0x75F24.
// Object 1 occupies 59 full pages from 0x3AF24, with disk padding
// after its 0x3A852 live bytes. Never concatenate at that live end.
// This class is source-only; runtime-mutated inventories are separate.
class OriginalF790StaticResources final {
public:
    static constexpr std::size_t kObject2DiskBase=0x75F24;
    static constexpr std::size_t kObject2DiskBytes=0x1273D;
    static constexpr std::size_t kCode2FileBytes=558689;
    static constexpr std::size_t kContainerTable=0x7330;
    static constexpr std::size_t kDescriptorStride=10;

    explicit OriginalF790StaticResources(const std::vector<std::uint8_t>& code2) {
        if(code2.size()!=kCode2FileBytes || code2[0x27BDC]!='L' ||
           code2[0x27BDD]!='E' ||
           read32(code2,0x27BDC+0xC4+0x0C)!=1U ||
           read32(code2,0x27BDC+0xC4+0x10)!=59U ||
           read32(code2,0x27BDC+0xDC+0x0C)!=60U ||
           read32(code2,0x27BDC+0xDC+0x10)!=19U)
            throw std::invalid_argument("not original CODE.2 LE two-object page map");
        data_.assign(code2.begin()+kObject2DiskBase,code2.end());
        if(data_.size()!=kObject2DiskBytes ||
           read32(data_,0x6D98)!=0x598U ||
           data_[0x7330]!=4U || data_[0x7332]!=8U ||
           cstring(0x610)!=" +" || cstring(0x614)!="; " ||
           cstring(0x618)!=" " || cstring(0x61C)!="-" ||
           cstring(0x620)!="/" || cstring(0x624)!=".5" ||
           cstring(0x598)!="; CHARGES: ")
            throw std::invalid_argument("original CODE.2 description data mismatch");
    }

    [[nodiscard]] std::string ds_text(std::uint32_t reference) const {
        switch(reference) {
        case 0x610:case 0x614:case 0x618:
        case 0x61C:case 0x620:case 0x624:return cstring(reference);
        case 0x6D98:return cstring(read32(data_,0x6D98));
        default:throw std::out_of_range("unknown F790 DS text reference");
        }
    }

    struct Container {
        std::uint8_t count{};
        std::uint8_t columns{};
        std::uint8_t suffix_code{};
        std::uint8_t style{};
        std::uint8_t kind{};
        std::uint32_t contents_offset{};
    };
    [[nodiscard]] Container descriptor(int subtype) const {
        if(subtype<0 || subtype>62)
            throw std::out_of_range("unsupported original container subtype");
        const auto off=kContainerTable+static_cast<std::size_t>(subtype)*kDescriptorStride;
        if(off+10>data_.size())throw std::out_of_range("truncated container descriptor");
        return {data_[off],data_[off+1],data_[off+2],data_[off+3],
                data_[off+4],read32(data_,off+5)};
    }
    [[nodiscard]] std::uint8_t suffix_7332(int subtype) const {
        return descriptor(subtype).suffix_code;
    }
    [[nodiscard]] std::uint8_t charge_67ec(int subtype) const {
        const auto off=static_cast<std::int64_t>(0x67EC)+subtype;
        if(off<0 || static_cast<std::uint64_t>(off)>=data_.size())
            throw std::out_of_range("F790 signed variant index beyond object 2");
        return data_[static_cast<std::size_t>(off)];
    }
    [[nodiscard]] std::int16_t initial_item(int subtype,std::size_t slot) const {
        const auto d=descriptor(subtype);
        if(slot>=d.count || d.contents_offset==0 ||
           static_cast<std::uint64_t>(d.contents_offset)+2U*(slot+1U)>data_.size())
            throw std::out_of_range("original container slot pointer/extent");
        return static_cast<std::int16_t>(read16(data_,d.contents_offset+2U*slot));
    }

    // Archival INITIAL load-image view only. Runtime mutations replace
    // container words; never use this initial count for live inventory.
    [[nodiscard]] std::uint32_t initial_nested_half_units(
        int subtype,const std::vector<std::uint8_t>& original_objects) const {
        if(original_objects.size()<4 || read32(original_objects,0)!=336U ||
           original_objects.size()!=4U+336U*0x41U)
            throw std::invalid_argument("wrong original OBJECTS.DAT record set");
        std::vector<bool> visited(63,false);
        return nested(subtype,original_objects,visited);
    }
private:
    static std::uint16_t read16(const std::vector<std::uint8_t>& v,
                                std::size_t off) {
        if(off+2>v.size())throw std::out_of_range("original 16-bit read");
        return static_cast<std::uint16_t>(v[off] | (v[off+1]<<8U));
    }
    static std::uint32_t read32(const std::vector<std::uint8_t>& v,
                                std::size_t off) {
        if(off+4>v.size())throw std::out_of_range("original 32-bit read");
        return static_cast<std::uint32_t>(v[off]) |
               (static_cast<std::uint32_t>(v[off+1])<<8U) |
               (static_cast<std::uint32_t>(v[off+2])<<16U) |
               (static_cast<std::uint32_t>(v[off+3])<<24U);
    }
    [[nodiscard]] std::string cstring(std::size_t off) const {
        if(off>=data_.size())throw std::out_of_range("F790 DS string address");
        const auto start=off;
        while(off<data_.size() && data_[off]!=0U)++off;
        if(off==data_.size())throw std::out_of_range("F790 unterminated DS string");
        return std::string(reinterpret_cast<const char*>(data_.data()+start),off-start);
    }
    [[nodiscard]] std::uint32_t nested(
        int subtype,const std::vector<std::uint8_t>& objs,
        std::vector<bool>& visited) const {
        // Original CODE.2 F603 special-cases exact signed subtype 0x14.
        if(subtype==0x14)return objs[4U+80U*0x41U+0x30U];
        if(subtype<0 || subtype>=static_cast<int>(visited.size()) || visited[subtype])
            throw std::out_of_range("invalid/cyclic F5F8 initial container graph");
        visited[subtype]=true;
        const auto d=descriptor(subtype);
        std::uint32_t total=0U;
        for(std::size_t slot=0;slot<d.count;++slot) {
            const auto id=initial_item(subtype,slot);
            if(id==-1)continue;
            if(id<0 || id>=336)throw std::out_of_range("invalid original nested item ID");
            const std::size_t at=4U+static_cast<std::size_t>(id)*0x41U;
            total+=objs[at+0x30U];
            if(objs[at+0x2FU]==1U)
                total+=nested(static_cast<std::int8_t>(objs[at+0x31U]),objs,visited);
        }
        visited[subtype]=false;
        return total;
    }
    std::vector<std::uint8_t> data_;
};
} // namespace rloft
