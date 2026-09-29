#include "games/ravenloft/data/AnmMovie.h"
#include "games/ravenloft/data/Binary.h"
#include "engine/core/FileSystem.h"
#include <algorithm>
#include <span>
#include <stdexcept>

namespace ravenloft::data {
AnmMovie::AnmMovie(const std::filesystem::path& path){
    auto bytes=ssi::core::readFile(path); fileSize_=bytes.size();
    std::span<const std::uint8_t> b(bytes.data(),bytes.size());
    if(b.size()<4) throw std::runtime_error("ANM header truncated");
    frameCount_=u16(b,0); timing_=u16(b,2); std::size_t pos=4;
    frames_.reserve(frameCount_);
    for(std::uint16_t fi=0;fi<frameCount_;++fi){
        requireRange(b,pos,2,"ANM frame chunk count"); const auto n=u16(b,pos); pos+=2;
        AnmFrame f; f.chunks.reserve(n);
        for(std::uint16_t ci=0;ci<n;++ci){
            requireRange(b,pos,6,"ANM chunk header"); AnmChunk c; c.type=u16(b,pos); const auto len=u32(b,pos+2); pos+=6;
            requireRange(b,pos,len,"ANM chunk payload"); c.payload.assign(b.begin()+pos,b.begin()+pos+len); pos+=len; f.chunks.push_back(std::move(c));
        }
        frames_.push_back(std::move(f));
    }
    parsedBytes_=pos;
    if(pos!=b.size()) throw std::runtime_error("ANM parser did not land at exact EOF");
}

AnmDecoder::AnmDecoder():pixels_(Width*Height,0){}
void AnmDecoder::applyPalette(const std::vector<std::uint8_t>& p){
    if(p.size()!=768) throw std::runtime_error("ANM palette chunk is not 768 bytes");
    auto cv=[](std::uint8_t v){return static_cast<std::uint8_t>((v<<2)|(v>>4));};
    for(std::size_t i=0;i<256;++i) palette_[i]={cv(p[i*3]),cv(p[i*3+1]),cv(p[i*3+2])};
}
void AnmDecoder::applyFullRle(const std::vector<std::uint8_t>& p){
    // CODE.2 0x27BD8: 400 scanlines. Each expands to exactly 0x140 (320)
    // chunky pixels, then is converted into four VGA planes by 0x27A90.
    std::size_t src=0;
    for(int y=0;y<Height;++y){
        std::size_t out=0;
        while(out<Width){
            if(src>=p.size()) throw std::runtime_error("ANM type-2 RLE truncated");
            const std::uint8_t control=p[src++];
            if((control&0x80u)==0){
                const std::size_t n=std::size_t(control)+1; if(out+n>Width||src+n>p.size()) throw std::runtime_error("ANM type-2 literal overrun");
                std::copy_n(p.begin()+src,n,pixels_.begin()+std::size_t(y)*Width+out); src+=n; out+=n;
            }else{
                const std::size_t n=0x101u-std::size_t(control); if(out+n>Width||src>=p.size()) throw std::runtime_error("ANM type-2 repeat overrun");
                std::fill_n(pixels_.begin()+std::size_t(y)*Width+out,n,p[src++]); out+=n;
            }
        }
    }
    if(src!=p.size()) throw std::runtime_error("ANM type-2 payload has trailing bytes");
}
void AnmDecoder::applySparsePlanar(const std::vector<std::uint8_t>& p){
    // CODE.2 0x27AEC. X and Y are cumulative within their respective loops.
    std::span<const std::uint8_t> b(p.data(),p.size()); std::size_t pos=0;
    if(b.size()<2) throw std::runtime_error("ANM type-3 truncated");
    const auto outer=u16(b,pos); pos+=2; std::uint32_t x=0;
    for(std::uint16_t oi=0;oi<outer;++oi){
        requireRange(b,pos,4,"ANM type-3 outer"); x+=u16(b,pos); pos+=2; const auto runs=u16(b,pos); pos+=2; std::uint32_t y=0;
        for(std::uint16_t ri=0;ri<runs;++ri){
            requireRange(b,pos,4,"ANM type-3 run"); y+=u16(b,pos); pos+=2; const auto n=u16(b,pos); pos+=2;
            requireRange(b,pos,n,"ANM type-3 bytes");
            for(std::uint16_t k=0;k<n;++k,++y){
                if(x>=Width||y>=Height) throw std::runtime_error("ANM type-3 target out of bounds");
                pixels_[std::size_t(y)*Width+x]=b[pos+k];
            }
            pos+=n;
        }
    }
    if(pos!=b.size()) throw std::runtime_error("ANM type-3 payload has trailing bytes");
}
void AnmDecoder::applyRawPlanar(const std::vector<std::uint8_t>& p){
    // CODE.2 0x27B84 copies four 0x5000-byte planes beginning at VGA offset 0x0FF0.
    // 0x0FF0 / 80 = scanline 51; 0x5000 / 80 = 256 scanlines.
    if(p.size()!=4u*0x5000u) throw std::runtime_error("ANM type-8 payload is not 4*0x5000 bytes");
    constexpr int startY=0x0ff0/80; // 51
    for(int plane=0;plane<4;++plane){
        const std::size_t base=std::size_t(plane)*0x5000;
        for(int i=0;i<0x5000;++i){
            const int y=startY+i/80, x=(i%80)*4+plane;
            if(y>=0&&y<Height) pixels_[std::size_t(y)*Width+x]=p[base+i];
        }
    }
}
std::vector<AnmMetadataEvent> AnmDecoder::apply(const AnmFrame& frame){
    std::vector<AnmMetadataEvent> events;
    for(auto const& c:frame.chunks){
        std::span<const std::uint8_t> b(c.payload.data(),c.payload.size());
        switch(c.type){
            case 0: applyPalette(c.payload); break;
            case 1: if(!c.payload.empty()) throw std::runtime_error("ANM clear chunk should be empty"); std::fill(pixels_.begin(),pixels_.end(),0); break;
            case 2: applyFullRle(c.payload); break;
            case 3: applySparsePlanar(c.payload); break;
            case 4: throw std::runtime_error("ANM type-4 repeat/jump not present in shipped movies and not expected in linear decoder");
            case 5: if(b.size()!=4) throw std::runtime_error("ANM music chunk size"); events.push_back({AnmMetadataEvent::Kind::Music,u16(b,0),u16(b,2)}); break;
            case 6: if(b.size()!=2) throw std::runtime_error("ANM sample chunk size"); events.push_back({AnmMetadataEvent::Kind::Sample,u16(b,0),0}); break;
            case 7: if(b.size()!=4) throw std::runtime_error("ANM subtitle chunk size"); events.push_back({AnmMetadataEvent::Kind::Subtitle,u16(b,0),u16(b,2)}); break;
            case 8: applyRawPlanar(c.payload); break;
            case 9: if(b.size()!=2) throw std::runtime_error("ANM direct-sample chunk size"); events.push_back({AnmMetadataEvent::Kind::DirectSample,u16(b,0),0}); break;
            default: throw std::runtime_error("Unknown ANM chunk type");
        }
    }
    return events;
}
} // namespace ravenloft::data
