#include "ResourceCache.hpp"
#include <cstdint>
#include <limits>

namespace rloft::resource_cache {
namespace {
std::uint16_t le16(const Byte* b) noexcept {
    return static_cast<std::uint16_t>(b[0] | (std::uint16_t(b[1])<<8u));
}
std::uint32_t le32(const Byte* b) noexcept {
    return static_cast<std::uint32_t>(le16(b)) |
           (static_cast<std::uint32_t>(le16(b+2u))<<16u);
}
}
bool locate(const std::vector<Byte>& archive,
            std::uint32_t index,Record& out) noexcept {
    out={};
    if(archive.size()<8u ||
       archive.size()>std::numeric_limits<std::uint32_t>::max())return false;
    const auto count=le32(archive.data());
    const std::uint64_t table_end=4u+std::uint64_t(count)*4u;
    if(index>=count || table_end>archive.size() ||
       4u+std::uint64_t(index)*4u+4u>table_end)return false;
    const auto start=le32(archive.data()+4u+index*4u);
    if(start<table_end || std::uint64_t(start)+5u>archive.size())
        return false;
    const auto end=index+1u<count
        ?le32(archive.data()+4u+(index+1u)*4u)
        :static_cast<std::uint32_t>(archive.size());
    out={start,end,archive[start+2u]};
    return true;
}
bool authenticate(const std::vector<Byte>& archive,
                  const Record& record,unsigned archive_slot) noexcept {
    const auto entry=record.start, next=record.end;
    if(!tracked_type(record.type) ||
       std::uint64_t(entry)+5u>archive.size() ||
       next>archive.size() ||
       archive[entry]!='E' || archive[entry+1u]!='H')
        return false;
    const auto* b=archive.data();
    if(record.type==1u) {
        return next>std::uint64_t(entry)+13u &&
               le16(b+entry+3u)==8u &&
               (b[entry+11u]==0u || b[entry+11u]==3u) &&
               le16(b+entry+5u)!=0u &&
               le16(b+entry+7u)!=0u &&
               b[entry+9u]!=0u;
    }
    if(record.type==2u) {
        // Exact original 301A8 128-width table and eight-byte image
        // descriptor; font data belongs only to RES0.
        const auto descriptor=std::uint64_t(entry)+5u+128u;
        if(archive_slot!=0u ||
           next<=std::uint64_t(entry)+5u+136u ||
           le16(b+entry+3u)!=136u)return false;
        const auto count=static_cast<std::int16_t>(le16(b+descriptor));
        const auto height=static_cast<std::int16_t>(le16(b+descriptor+2u));
        return count>0 && count<=128 && height>0 &&
               b[descriptor+4u]!=0u && b[descriptor+6u]==3u;
    }
    // Original retained type 6/7 compressed texture registration.
    return next>std::uint64_t(entry)+7u;
}
} // namespace rloft::resource_cache
