#include "rloft/RES11.hpp"

#include "rloft/RES4.hpp"
#include "rloft/RES6.hpp"
#include "rloft/resource_assets.hpp"
#include "rloft/voc_decoder.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {
constexpr int kAnomalousIndex = 4877;
constexpr std::size_t kAnomalousEntryBytes = 5746U;
constexpr std::uint32_t kAnomalousOuterLength = 0x11669U;
constexpr std::uint32_t kAnomalousInnerLength = 0x1164AU;
constexpr std::array<Res11Family,4> kOrder{{
    Res11Family::SceneShape, Res11Family::Texture,
    Res11Family::Xmidi, Res11Family::Voice}};

std::uint16_t u16(const std::vector<std::uint8_t>& b, std::size_t at) {
    return static_cast<std::uint16_t>(b.at(at) |
        (static_cast<unsigned>(b.at(at + 1U)) << 8U));
}
std::uint32_t u32(const std::vector<std::uint8_t>& b, std::size_t at) {
    return static_cast<std::uint32_t>(b.at(at)) |
        (static_cast<std::uint32_t>(b.at(at+1U)) << 8U) |
        (static_cast<std::uint32_t>(b.at(at+2U)) << 16U) |
        (static_cast<std::uint32_t>(b.at(at+3U)) << 24U);
}
void put32(std::vector<std::uint8_t>& b, std::size_t at, std::uint32_t v) {
    for (unsigned i=0U;i<4U;++i)
        b.at(at+i) = static_cast<std::uint8_t>(v >> (8U*i));
}
std::size_t family_slot(Res11Family family) {
    for (std::size_t i=0;i<kOrder.size();++i)
        if (kOrder[i]==family) return i;
    throw std::invalid_argument("RES11 unknown resource family");
}
Res11Family family_of(const std::vector<std::uint8_t>& e) {
    if (e.size()<7U || e[0]!='E' || e[1]!='H' || u16(e,3U)!=0U)
        throw std::runtime_error("RES11 malformed EH resource envelope");
    switch (e[2U]) {
    case 7U: return Res11Family::SceneShape;
    case 6U: return Res11Family::Texture;
    case 3U: return Res11Family::Xmidi;
    case 4U: return Res11Family::Voice;
    default: throw std::runtime_error("RES11 unsupported EH resource family");
    }
}
std::vector<std::uint8_t> one_entry(const std::vector<std::uint8_t>& entry) {
    std::vector<std::uint8_t> bytes(8U,0U);
    put32(bytes,0U,1U); put32(bytes,4U,8U);
    bytes.insert(bytes.end(),entry.begin(),entry.end());
    return bytes;
}
std::vector<std::uint8_t> literal_lzss(const std::vector<std::uint8_t>& src) {
    std::vector<std::uint8_t> out;
    out.reserve(src.size()+(src.size()+7U)/8U);
    for (std::size_t at=0U;at<src.size();at+=8U) {
        const auto count=std::min<std::size_t>(8U,src.size()-at);
        out.push_back(static_cast<std::uint8_t>((1U<<count)-1U));
        out.insert(out.end(),src.begin()+static_cast<std::ptrdiff_t>(at),
                   src.begin()+static_cast<std::ptrdiff_t>(at+count));
    }
    return out;
}
bool legacy_anomaly(int index, const std::vector<std::uint8_t>& e) {
    if (index!=kAnomalousIndex || e.size()!=kAnomalousEntryBytes ||
        e[2U]!=4U || u32(e,5U)!=kAnomalousOuterLength ||
        u16(e,9U+20U)!=26U || u16(e,9U+22U)!=0x010AU ||
        e[9U+26U]!=1U || u16(e,9U+24U)!=0x1129U) {
        // VOC block size is little-endian u24; preserve the shipped overflow.
        return false;
    }
    const auto inner=static_cast<std::uint32_t>(e[9U+27U]) |
        (static_cast<std::uint32_t>(e[9U+28U])<<8U) |
        (static_cast<std::uint32_t>(e[9U+29U])<<16U);
    return inner==kAnomalousInnerLength;
}
// Validate the physical representation without silently normalizing any
// envelope. In particular the original malformed voice is a preservation
// exception, NOT a playable or valid-length Creative VOC sound.
Res11EntryInfo inspect_entry(int index, const std::vector<std::uint8_t>& e,
                             bool deep) {
    Res11EntryInfo info{};
    info.family=family_of(e);
    info.stored_bytes=e.size();
    const auto kind=info.family;
    if (kind==Res11Family::SceneShape || kind==Res11Family::Texture) {
        if (e.size()<=7U) throw std::runtime_error("RES11 truncated LZSS payload");
        info.declared_bytes=u16(e,5U);
        if (kind==Res11Family::Texture &&
            info.declared_bytes!=4096U && info.declared_bytes!=8192U)
            throw std::runtime_error("RES11 texture is not 32x128 or 64x128");
        if (kind==Res11Family::SceneShape && info.declared_bytes<14U)
            throw std::runtime_error("RES11 type-7 display list too small");
    } else {
        if (e.size()<9U) throw std::runtime_error("RES11 EH3/4 header truncated");
        info.declared_bytes=u32(e,5U);
        if (kind==Res11Family::Xmidi) {
            if (e.size()-9U!=info.declared_bytes || e.size()<21U ||
                e[9U]!='F' || e[10U]!='O' || e[11U]!='R' || e[12U]!='M' ||
                e[17U]!='X' || e[18U]!='D' || e[19U]!='I' || e[20U]!='R')
                throw std::runtime_error("RES11 XMIDI/XDIR envelope invalid");
        } else {
            if (e.size()<9U+26U)
                throw std::runtime_error("RES11 VOC header truncated");
            info.legacy_length_anomaly=legacy_anomaly(index,e);
            if (!info.legacy_length_anomaly && info.declared_bytes!=e.size()-9U)
                throw std::runtime_error("RES11 VOC declared/physical length mismatch");
            if (!info.legacy_length_anomaly) {
                const std::vector<std::uint8_t> physical(e.begin()+9,e.end());
                (void)inspect_res6_voc(physical);
            }
        }
    }
    if (deep) {
        ResArchive individual(one_entry(e));
        switch (kind) {
        case Res11Family::SceneShape: (void)individual.decode_type7(0); break;
        case Res11Family::Texture: (void)decode_res4_texture(individual,0); break;
        case Res11Family::Xmidi: (void)individual.decode_type3(0); break;
        case Res11Family::Voice:
            if (!info.legacy_length_anomaly) (void)individual.decode_type4(0);
            break;
        }
    }
    return info;
}
void require_family(Res11Family actual, Res11Family requested) {
    if (actual!=requested) throw std::invalid_argument("RES11 resource family mismatch");
}
} // namespace

Res11Archive::Res11Archive(std::vector<std::uint8_t> source)
    : archive_(std::make_unique<ResArchive>(std::move(source))) {
    const auto n=archive_->entry_count();
    if (n<4) throw std::runtime_error("RES11 missing composite resource banks");
    stats_.archive_bytes=archive_->encode().size();
    stats_.entries=static_cast<std::size_t>(n);
    int current=-1;
    for (int i=0;i<n;++i) {
        const auto info=inspect_entry(i,archive_->entry(i),false);
        const auto slot=static_cast<int>(family_slot(info.family));
        if (slot<current || slot>current+1)
            throw std::runtime_error("RES11 composite resource bank order changed");
        if (slot!=current) {
            current=slot;
            ranges_[static_cast<std::size_t>(slot)].first=i;
        }
        ++ranges_[static_cast<std::size_t>(slot)].count;
        if (info.legacy_length_anomaly) ++stats_.anomalous_voices;
    }
    if (current!=3) throw std::runtime_error("RES11 missing final VOC bank");
    stats_.shapes=static_cast<std::size_t>(ranges_[0].count);
    stats_.textures=static_cast<std::size_t>(ranges_[1].count);
    stats_.xmidi=static_cast<std::size_t>(ranges_[2].count);
    stats_.voices=static_cast<std::size_t>(ranges_[3].count);
}
Res11Archive::~Res11Archive() = default;
Res11Archive::Res11Archive(Res11Archive&&) noexcept = default;
Res11Archive& Res11Archive::operator=(Res11Archive&&) noexcept = default;
int Res11Archive::entry_count() const noexcept {return archive_->entry_count();}
const Res11Stats& Res11Archive::stats() const noexcept {return stats_;}
Res11Range Res11Archive::range(Res11Family family) const {
    return ranges_[family_slot(family)];
}
Res11EntryInfo Res11Archive::entry_info(int index) const {
    return inspect_entry(index,archive_->entry(index),false);
}
std::vector<std::uint8_t> Res11Archive::entry(int index) const {
    return archive_->entry(index);
}
std::vector<std::uint8_t> Res11Archive::encode() const {return archive_->encode();}
Type7SceneShape Res11Archive::shape(int index) const {
    require_family(entry_info(index).family,Res11Family::SceneShape);
    return archive_->decode_type7(index);
}
std::vector<std::uint8_t> Res11Archive::shape_body(int index) const {
    require_family(entry_info(index).family,Res11Family::SceneShape);
    return archive_->decode_type7_body(index);
}
SurfaceTexture Res11Archive::texture(int index) const {
    require_family(entry_info(index).family,Res11Family::Texture);
    return decode_res4_texture(*archive_,index);
}
std::vector<std::uint8_t> Res11Archive::xmidi(int index) const {
    require_family(entry_info(index).family,Res11Family::Xmidi);
    return archive_->decode_type3(index);
}
std::vector<std::uint8_t> Res11Archive::voc(int index) const {
    require_family(entry_info(index).family,Res11Family::Voice);
    const auto e=entry(index);
    return {e.begin()+9,e.end()}; // Use physical size, even for 4877.
}
PcmSound Res11Archive::pcm16(int index) const {
    if (entry_info(index).legacy_length_anomaly)
        throw std::runtime_error("RES11:4877 is a truncated shipped VOC; PCM is unavailable");
    return decode_creative_voice(voc(index));
}
Res11Verification Res11Archive::verify_all() const {
    Res11Verification v{};
    for (int i=0;i<entry_count();++i) {
        const auto info=entry_info(i);
        switch(info.family) {
        case Res11Family::SceneShape:
            (void)shape(i); v.shape_body_bytes+=info.declared_bytes;
            ++v.decoded_shapes; break;
        case Res11Family::Texture:
            v.texture_texel_bytes+=texture(i).texels_column_major.size();
            ++v.decoded_textures; break;
        case Res11Family::Xmidi: (void)xmidi(i); ++v.decoded_xmidi; break;
        case Res11Family::Voice:
            if (info.legacy_length_anomaly) ++v.preserved_anomalies;
            else { (void)inspect_res6_voc(voc(i)); ++v.validated_voices; }
            break;
        }
    }
    return v;
}
Res11EntryPatch Res11Archive::texture_patch(
    int index, const SurfaceTexture& replacement) const {
    require_family(entry_info(index).family,Res11Family::Texture);
    auto expected=entry(index);
    ResArchive single(one_entry(expected));
    replace_res4_texture(single,0,replacement);
    return {index,std::move(expected),single.entry(0)};
}
Res11EntryPatch Res11Archive::shape_body_patch(
    int index, const std::vector<std::uint8_t>& replacement) const {
    require_family(entry_info(index).family,Res11Family::SceneShape);
    const auto original=shape_body(index);
    if (replacement.size()!=original.size() ||
        !std::equal(original.begin(),original.begin()+10,replacement.begin()))
        throw std::invalid_argument("RES11 shape body changes size or source geometry");
    auto raw=entry(index);
    raw.resize(7U);
    const auto packed=literal_lzss(replacement);
    raw.insert(raw.end(),packed.begin(),packed.end());
    return {index,entry(index),std::move(raw)};
}
Res11EntryPatch Res11Archive::xmidi_patch(
    int index, const std::vector<std::uint8_t>& replacement) const {
    require_family(entry_info(index).family,Res11Family::Xmidi);
    if (replacement.size()<12U || replacement.size()>
        static_cast<std::size_t>(std::numeric_limits<std::uint32_t>::max()))
        throw std::invalid_argument("RES11 replacement XMIDI is not a legal payload");
    auto raw=entry(index);
    raw.resize(9U);
    put32(raw,5U,static_cast<std::uint32_t>(replacement.size()));
    raw.insert(raw.end(),replacement.begin(),replacement.end());
    return {index,entry(index),std::move(raw)};
}
Res11EntryPatch Res11Archive::voc_patch(
    int index, const std::vector<std::uint8_t>& replacement) const {
    require_family(entry_info(index).family,Res11Family::Voice);
    (void)inspect_res6_voc(replacement);
    if (replacement.size()>
        static_cast<std::size_t>(std::numeric_limits<std::uint32_t>::max()))
        throw std::overflow_error("RES11 replacement VOC exceeds 32-bit length");
    auto raw=entry(index);
    raw.resize(9U);
    put32(raw,5U,static_cast<std::uint32_t>(replacement.size()));
    raw.insert(raw.end(),replacement.begin(),replacement.end());
    return {index,entry(index),std::move(raw)};
}
void Res11Archive::apply(const std::vector<Res11EntryPatch>& patches) {
    if (patches.empty()) return;
    std::map<int,std::vector<std::uint8_t>> replacements;
    for (const auto& p:patches) {
        if (p.index<0 || p.index>=entry_count())
            throw std::out_of_range("RES11 replacement archive index");
        if (replacements.count(p.index))
            throw std::invalid_argument("RES11 duplicate archive edit");
        const auto original=entry(p.index);
        if (original!=p.expected_entry)
            throw std::runtime_error("RES11 stale expected resource bytes");
        const auto old_info=entry_info(p.index);
        const auto new_info=inspect_entry(p.index,p.replacement_entry,true);
        require_family(new_info.family,old_info.family);
        if (new_info.family==Res11Family::SceneShape ||
            new_info.family==Res11Family::Texture) {
            if (old_info.declared_bytes!=new_info.declared_bytes)
                throw std::invalid_argument("RES11 replacement changes decoded dimensions");
        }
        replacements.emplace(p.index,p.replacement_entry);
    }
    const auto original=archive_->encode();
    const auto count=static_cast<std::size_t>(entry_count());
    const auto limit=static_cast<std::uint64_t>(
        std::numeric_limits<std::uint32_t>::max());
    std::uint64_t total=4ULL+4ULL*count;
    for (std::size_t i=0;i<count;++i) {
        const auto from=static_cast<std::size_t>(u32(original,4U+4U*i));
        const auto to=i+1U<count
            ? static_cast<std::size_t>(u32(original,8U+4U*i))
            : original.size();
        const auto found=replacements.find(static_cast<int>(i));
        total+=found==replacements.end()?to-from:found->second.size();
        if (total>limit) throw std::overflow_error("RES11 rebuilt archive exceeds 32-bit offsets");
    }
    std::vector<std::uint8_t> result(static_cast<std::size_t>(total));
    put32(result,0U,static_cast<std::uint32_t>(count));
    std::size_t at=4U+4U*count;
    for (std::size_t i=0;i<count;++i) {
        put32(result,4U+4U*i,static_cast<std::uint32_t>(at));
        const auto found=replacements.find(static_cast<int>(i));
        if (found!=replacements.end()) {
            std::copy(found->second.begin(),found->second.end(),
                      result.begin()+static_cast<std::ptrdiff_t>(at));
            at+=found->second.size();
        } else {
            const auto from=static_cast<std::size_t>(u32(original,4U+4U*i));
            const auto to=i+1U<count
                ? static_cast<std::size_t>(u32(original,8U+4U*i))
                : original.size();
            std::copy(original.begin()+static_cast<std::ptrdiff_t>(from),
                      original.begin()+static_cast<std::ptrdiff_t>(to),
                      result.begin()+static_cast<std::ptrdiff_t>(at));
            at+=to-from;
        }
    }
    Res11Archive verified(std::move(result));
    *this=std::move(verified);
}
} // namespace rloft
