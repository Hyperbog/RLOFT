#include "rloft/RES12.hpp"
#include "rloft/RES6.hpp"
#include "rloft/resource_assets.hpp"

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
constexpr std::size_t kRasterPrefix = 13U, kVocPrefix = 9U;
constexpr int kTruncatedVoc = 430;
std::uint16_t u16(const std::vector<std::uint8_t>& b, std::size_t p) {
    return static_cast<std::uint16_t>(b.at(p) | (unsigned(b.at(p+1U))<<8U));
}
std::uint32_t u32(const std::vector<std::uint8_t>& b, std::size_t p) {
    return std::uint32_t(b.at(p)) | (std::uint32_t(b.at(p+1U))<<8U) |
           (std::uint32_t(b.at(p+2U))<<16U) | (std::uint32_t(b.at(p+3U))<<24U);
}
void put32(std::vector<std::uint8_t>& b, std::size_t p, std::uint32_t n) {
    for (unsigned i=0U;i<4U;++i) b.at(p+i)=static_cast<std::uint8_t>(n>>(8U*i));
}
Res9RasterInfo raster_metadata(const std::vector<std::uint8_t>& e) {
    if(e.size()<kRasterPrefix || e[0]!='E' || e[1]!='H' || e[2]!=1U ||
       u16(e,3U)!=8U) throw std::runtime_error("RES12 malformed type-1 descriptor");
    Res9RasterInfo d{};
    d.count=u16(e,5U); d.height=u16(e,7U);
    d.width=static_cast<std::uint16_t>(e[9U]*8U);
    d.descriptor_field5=e[10U]; d.compression=e[11U];
    d.descriptor_field7=e[12U];
    const auto size=std::uint64_t(d.count)*d.height*d.width;
    if(!d.count || d.count>32767U || !d.height || d.height>32767U ||
       !d.width || size>64ULL*1024ULL*1024ULL ||
       (d.compression!=0U && d.compression!=3U) ||
       (d.compression==0U && e.size()-kRasterPrefix!=size) ||
       (d.compression==3U && e.size()==kRasterPrefix))
        throw std::runtime_error("RES12 invalid type-1 raster dimensions/stream");
    d.decoded_bytes=static_cast<std::size_t>(size);
    return d;
}
Res9Program parse_program(const std::vector<std::uint8_t>& e) {
    if(e.size()<2U) throw std::runtime_error("RES12 wordcode count missing");
    const auto n=static_cast<std::size_t>(u16(e,0U));
    if(!n || e.size()!=2U+4U*n) throw std::runtime_error("RES12 wordcode banks mis-sized");
    Res9Program p;
    p.words.reserve(n); p.companion_words.reserve(n);
    for(std::size_t i=0;i<n;++i) p.words.push_back(u16(e,2U+2U*i));
    for(std::size_t i=0;i<n;++i) p.companion_words.push_back(u16(e,2U+2U*n+2U*i));
    return p;
}
bool is_damaged_voc(int index, const std::vector<std::uint8_t>& e) {
    return index==kTruncatedVoc && e.size()==5746U &&
           e[0]=='E' && e[1]=='H' && e[2]==4U && u16(e,3U)==0U &&
           u32(e,5U)==0x11669U && e[9U+26U]==1U &&
           e[9U+20U]==26U && u16(e,9U+22U)==0x010AU &&
           u16(e,9U+24U)==0x1129U &&
           (std::uint32_t(e[9U+27U]) | (std::uint32_t(e[9U+28U])<<8U) |
           (std::uint32_t(e[9U+29U])<<16U))==0x1164AU;
}
void validate_voice(int index, const std::vector<std::uint8_t>& e) {
    if(e.size()<kVocPrefix+26U || e[0]!='E' || e[1]!='H' ||
       e[2]!=4U || u16(e,3U)!=0U)
        throw std::runtime_error("RES12 invalid EH type-4 voice envelope");
    if(is_damaged_voc(index,e)) return; // Preserve, but never synthesize audio.
    if(u32(e,5U)!=e.size()-kVocPrefix)
        throw std::runtime_error("RES12 voice declared/physical length mismatch");
    (void)inspect_res6_voc({e.begin()+kVocPrefix,e.end()});
}
std::vector<std::uint8_t> single_entry(const std::vector<std::uint8_t>& e) {
    std::vector<std::uint8_t> bytes(8U,0U);
    put32(bytes,0U,1U);put32(bytes,4U,8U);
    bytes.insert(bytes.end(),e.begin(),e.end());
    return bytes;
}
} // namespace

Res12Archive::Res12Archive(std::vector<std::uint8_t> source)
    : archive_(std::make_unique<ResArchive>(std::move(source))) {
    stats_.archive_bytes=archive_->encode().size();
    stats_.entries=static_cast<std::size_t>(archive_->entry_count());
    int phase=0;
    for(int i=0;i<entry_count();++i) {
        const auto e=entry(i);
        const bool eh=e.size()>=3U && e[0]=='E' && e[1]=='H';
        const int family=eh ? (e[2]==1U ? 0 : e[2]==4U ? 2 : -1) : 1;
        if(family<phase || family>phase+1 || family<0)
            throw std::runtime_error("RES12 graphics/wordcode/voice bank order invalid");
        if(family!=phase) {
            phase=family;
            if(phase==1) programs_.first=i;
            if(phase==2) voices_.first=i;
        }
        if(phase==0) {
            ++graphics_.count;
            const auto m=raster_metadata(e);
            stats_.decoded_pixels+=m.decoded_bytes;
            if(m.compression==0U)++stats_.raw_rasters;
            else ++stats_.lzss_rasters;
        } else if(phase==1) {
            ++programs_.count;
            const auto p=parse_program(e);
            stats_.program_words+=p.words.size();
            const auto ins=disassemble_res9_wordcode(p.words,141U);
            stats_.instructions+=ins.size();
            stats_.inline_templates+=static_cast<std::size_t>(std::count_if(
                ins.begin(),ins.end(),[](const Res9Instruction& a){return a.opcode==5U;}));
            stats_.nonzero_companion_words+=static_cast<std::size_t>(std::count_if(
                p.companion_words.begin(),p.companion_words.end(),
                [](std::uint16_t word){return word!=0U;}));
        } else {
            ++voices_.count;
            validate_voice(i,e);
            if(is_damaged_voc(i,e))++stats_.truncated_voices;
            else ++stats_.valid_voices;
        }
    }
    if(phase!=2 || !graphics_.count || !programs_.count || !voices_.count)
        throw std::runtime_error("RES12 incomplete composite archive");
    stats_.rasters=static_cast<std::size_t>(graphics_.count);
    stats_.programs=static_cast<std::size_t>(programs_.count);
    stats_.voices=static_cast<std::size_t>(voices_.count);
}
Res12Archive::~Res12Archive()=default;
Res12Archive::Res12Archive(Res12Archive&&) noexcept=default;
Res12Archive& Res12Archive::operator=(Res12Archive&&) noexcept=default;
const Res12Stats& Res12Archive::stats() const noexcept{return stats_;}
int Res12Archive::entry_count() const noexcept{return archive_->entry_count();}
Res12Range Res12Archive::range(Res12Family f) const {
    switch(f) {
    case Res12Family::Raster: return graphics_;
    case Res12Family::Wordcode: return programs_;
    case Res12Family::Voc: return voices_;
    }
    throw std::invalid_argument("RES12 unknown family");
}
Res12Family Res12Archive::family(int i) const {
    if(i<0 || i>=entry_count())throw std::out_of_range("RES12 entry index");
    if(i<programs_.first)return Res12Family::Raster;
    if(i<voices_.first)return Res12Family::Wordcode;
    return Res12Family::Voc;
}
std::vector<std::uint8_t> Res12Archive::entry(int i) const{return archive_->entry(i);}
std::vector<std::uint8_t> Res12Archive::encode() const{return archive_->encode();}
Res9RasterInfo Res12Archive::raster_info(int i) const {
    if(family(i)!=Res12Family::Raster)throw std::invalid_argument("RES12 not a raster");
    return raster_metadata(entry(i));
}
Type1Raster Res12Archive::raster(int i) const {
    (void)raster_info(i);return archive_->decode_type1(i);
}
Res9Program Res12Archive::program(int id) const {
    if(id<0 || id>=programs_.count)throw std::out_of_range("RES12 program ID");
    return parse_program(entry(programs_.first+id));
}
std::vector<Res9Instruction> Res12Archive::instructions(int id) const {
    return disassemble_res9_wordcode(program(id).words,141U);
}
std::vector<std::uint8_t> Res12Archive::voc(int i) const {
    if(family(i)!=Res12Family::Voc)throw std::invalid_argument("RES12 not a VOC");
    const auto e=entry(i);return {e.begin()+kVocPrefix,e.end()};
}
bool Res12Archive::is_truncated_voc(int i) const {
    if(family(i)!=Res12Family::Voc)throw std::invalid_argument("RES12 not a VOC");
    return is_damaged_voc(i,entry(i));
}
PcmSound Res12Archive::pcm16(int i) const {
    if(is_truncated_voc(i))throw std::runtime_error("RES12:430 original VOC is truncated");
    return decode_creative_voice(voc(i));
}
void Res12Archive::apply(const Res12Edits& edits) {
    if(edits.rasters.empty()&&edits.programs.empty()&&edits.voices.empty())return;
    std::map<int,std::vector<std::uint8_t>> replacements;
    for(const auto& p:edits.rasters) {
        const auto old=raster_info(p.image_index);
        const auto src=entry(p.image_index);
        if(replacements.count(p.image_index))throw std::invalid_argument("RES12 duplicate edit");
        if(src!=p.expected_entry)throw std::runtime_error("RES12 stale raster");
        std::vector<std::uint8_t> next;
        if(p.replacement_encoded_entry) {
            if(!p.replacement_pixels.empty())throw std::invalid_argument("RES12 conflicting raster forms");
            next=*p.replacement_encoded_entry;
            const auto info=raster_metadata(next);
            if(info.count!=old.count || info.width!=old.width || info.height!=old.height ||
               info.descriptor_field5!=old.descriptor_field5 ||
               info.descriptor_field7!=old.descriptor_field7)
                throw std::invalid_argument("RES12 raster descriptor changes");
        } else {
            if(p.replacement_pixels.size()!=old.decoded_bytes)
                throw std::invalid_argument("RES12 pixel count changed");
            next.assign(src.begin(),src.begin()+static_cast<std::ptrdiff_t>(kRasterPrefix));
            next[11U]=0U; // Original method 0 is legal raw type-1 data.
            next.insert(next.end(),p.replacement_pixels.begin(),p.replacement_pixels.end());
        }
        ResArchive check(single_entry(next));(void)check.decode_type1(0);
        replacements.emplace(p.image_index,std::move(next));
    }
    for(const auto& p:edits.programs) {
        if(p.program_id<0||p.program_id>=programs_.count)
            throw std::out_of_range("RES12 program ID");
        const int i=programs_.first+p.program_id;
        if(replacements.count(i))throw std::invalid_argument("RES12 duplicate edit");
        const auto original=program(p.program_id);
        if(p.expected_words!=original.words)throw std::runtime_error("RES12 stale wordcode");
        if(p.replacement_words.size()!=original.words.size())
            throw std::invalid_argument("RES12 program size changes companion-bank alignment");
        (void)disassemble_res9_wordcode(p.replacement_words,141U);
        auto next=entry(i);
        for(std::size_t j=0;j<p.replacement_words.size();++j) {
            next[2U+2U*j]=static_cast<std::uint8_t>(p.replacement_words[j]);
            next[3U+2U*j]=static_cast<std::uint8_t>(p.replacement_words[j]>>8U);
        }
        replacements.emplace(i,std::move(next));
    }
    for(const auto& p:edits.voices) {
        if(family(p.index)!=Res12Family::Voc)throw std::invalid_argument("RES12 not a VOC");
        if(replacements.count(p.index))throw std::invalid_argument("RES12 duplicate edit");
        const auto src=entry(p.index);
        if(src!=p.expected_entry)throw std::runtime_error("RES12 stale VOC");
        if(is_damaged_voc(p.index,src))
            throw std::invalid_argument("RES12 truncated original VOC is preservation-only");
        (void)inspect_res6_voc(p.replacement_voc);
        if(p.replacement_voc.size()>std::numeric_limits<std::uint32_t>::max())
            throw std::overflow_error("RES12 oversized VOC");
        std::vector<std::uint8_t> next(src.begin(),src.begin()+9);
        put32(next,5U,static_cast<std::uint32_t>(p.replacement_voc.size()));
        next.insert(next.end(),p.replacement_voc.begin(),p.replacement_voc.end());
        replacements.emplace(p.index,std::move(next));
    }
    const auto src=encode();
    std::uint64_t total=4ULL+4ULL*static_cast<std::uint64_t>(entry_count());
    for(int i=0;i<entry_count();++i){
        const auto p=replacements.find(i);
        const auto a=static_cast<std::size_t>(u32(src,4U+4U*static_cast<std::size_t>(i)));
        const auto z=i+1<entry_count()
           ?static_cast<std::size_t>(u32(src,8U+4U*static_cast<std::size_t>(i))):src.size();
        total+=p==replacements.end()?z-a:p->second.size();
        if(total>std::numeric_limits<std::uint32_t>::max())
            throw std::overflow_error("RES12 archive offset overflow");
    }
    std::vector<std::uint8_t> built(static_cast<std::size_t>(total));
    put32(built,0U,static_cast<std::uint32_t>(entry_count()));
    std::size_t at=4U+4U*static_cast<std::size_t>(entry_count());
    for(int i=0;i<entry_count();++i) {
        put32(built,4U+4U*static_cast<std::size_t>(i),static_cast<std::uint32_t>(at));
        auto found=replacements.find(i);
        if(found!=replacements.end()){
            std::copy(found->second.begin(),found->second.end(),
                      built.begin()+static_cast<std::ptrdiff_t>(at));
            at+=found->second.size();
        } else {
            const auto a=static_cast<std::size_t>(u32(src,4U+4U*static_cast<std::size_t>(i)));
            const auto z=i+1<entry_count()
               ?static_cast<std::size_t>(u32(src,8U+4U*static_cast<std::size_t>(i))):src.size();
            std::copy(src.begin()+static_cast<std::ptrdiff_t>(a),
                      src.begin()+static_cast<std::ptrdiff_t>(z),
                      built.begin()+static_cast<std::ptrdiff_t>(at));
            at+=z-a;
        }
    }
    Res12Archive validated(std::move(built));
    *this=std::move(validated);
}
} // namespace rloft
