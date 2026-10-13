#include "rloft_native/native_party_inventory.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace rloft_native {
namespace {
constexpr std::size_t kObjectStride=0x41U;
constexpr std::size_t kActorStride=0x248U;
constexpr std::size_t kInventoryOffset=0x40U;
constexpr std::size_t kKnownBitsOffset=0x12BU;
void require(bool good,const char* reason) {
    if(!good)throw std::runtime_error(reason);
}
std::uint16_t u16(const NativeCharsImage& b,std::size_t at) {
    return std::uint16_t(b.at(at)) |
           (std::uint16_t(b.at(at+1U))<<8U);
}
std::uint32_t u32(const NativeCharsImage& b,std::size_t at) {
    return std::uint32_t(b.at(at)) |
           (std::uint32_t(b.at(at+1U))<<8U) |
           (std::uint32_t(b.at(at+2U))<<16U) |
           (std::uint32_t(b.at(at+3U))<<24U);
}
std::uint32_t object_count(const std::vector<std::uint8_t>& b) {
    require(b.size()>=4U,"original OBJECTS.DAT header missing");
    return std::uint32_t(b[0]) | (std::uint32_t(b[1])<<8U) |
           (std::uint32_t(b[2])<<16U) | (std::uint32_t(b[3])<<24U);
}
std::string name(const std::vector<std::uint8_t>& b,std::size_t offset) {
    std::string out;
    for(std::size_t n=0;n<40U;++n) {
        const auto ch=b.at(offset+n);
        if(ch==0)return out; // Genuine blank names remain blank.
        require(ch>=0x20U && ch<=0x7EU,
                "original object display name is not ASCII");
        out.push_back(static_cast<char>(ch));
    }
    throw std::runtime_error("original object name has no NUL terminator");
}
} // namespace
const char* native_inventory_slot_label(unsigned slot) {
    static constexpr const char* kNames[]{
        "Backpack 01","Backpack 02","Backpack 03","Backpack 04",
        "Backpack 05","Backpack 06","Backpack 07","Backpack 08",
        "Backpack 09","Backpack 10","Backpack 11","Backpack 12",
        "Ring A","Ring B","Hand A","Hand B","Body","Head",
        "Accessory","Quiver","Bracers"
    };
    if(slot>=21U)throw std::out_of_range("CODE.2 inventory slot outside 0..20");
    return kNames[slot];
}
NativePartyInventory inspect_native_party_inventory(
    const NativePartyStart& party,
    const std::vector<std::uint8_t>& objects) {
    // This also checks the source 0x920 transfer actor status, attributes
    // and exact byte identity; no fabricated native actors are accepted.
    const auto decoded=decode_native_party_start(party.chars);
    for(unsigned i=0;i<2U;++i)
        require(decoded.actors[i].name==party.actors[i].name,
                "party header does not match authentic CODE.1 transfer");
    const auto count=object_count(objects);
    require(count==336U && objects.size()==4U+std::size_t(count)*kObjectStride,
            "source object table is not exact original 336x65 corpus");
    // Fail if ANY source definition is malformed, not only those
    // actually equipped by the current original party.
    std::vector<std::string> names;
    names.reserve(count);
    for(unsigned id=0;id<count;++id) {
        const auto at=4U+std::size_t(id)*kObjectStride;
        names.push_back(name(objects,at));
        require(objects[at+0x2FU]<=15U,
                "original object category outside source 0..15");
        const auto icon=std::uint16_t(objects[at+0x2DU]) |
                        (std::uint16_t(objects[at+0x2EU])<<8U);
        require(std::size_t(icon)+168U<=335U,
                "original OBJECTS RES1 inventory icon outside owned bank");
    }
    NativePartyInventory result;
    result.object_definitions=count;
    for(unsigned actor_index=0;actor_index<2U;++actor_index) {
        auto& actor=result.actors[actor_index];
        actor.actor_name=decoded.actors[actor_index].name;
        const auto base=actor_index*kActorStride;
        actor.original_known_mask=u32(party.chars,base+kKnownBitsOffset);
        for(unsigned slot=0;slot<21U;++slot) {
            auto& item=actor.slots[slot];
            item.slot=slot;
            const auto id=u16(party.chars,base+kInventoryOffset+2U*slot);
            if(id==0xFFFFU)continue;
            require(id<count,
                    "CODE.1 original party inventory references absent OBJECTS record");
            const auto offset=4U+std::size_t(id)*kObjectStride;
            item.occupied=true;
            item.object_id=static_cast<std::int16_t>(id);
            item.source_name=names[id];
            item.source_category=objects[offset+0x2FU];
            item.absolute_res1_ui_id=static_cast<std::uint16_t>(
                168U+std::uint16_t(objects[offset+0x2DU])+
                (std::uint16_t(objects[offset+0x2EU])<<8U));
            item.identified=((actor.original_known_mask>>slot)&1U)!=0U;
            ++actor.occupied_count;
            ++result.occupied_total;
        }
    }
    return result;
}
} // namespace rloft_native
