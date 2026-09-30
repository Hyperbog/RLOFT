#include "TileMapBuild1558.h"

#include <cstddef>

namespace ravenloft::game {
namespace {
std::uint16_t u16(std::span<const std::uint8_t> s,std::size_t o) noexcept {
 return static_cast<std::uint16_t>(s[o]) |
        (static_cast<std::uint16_t>(s[o+1u])<<8u);
}
std::int16_t s16(std::span<const std::uint8_t> s,std::size_t o) noexcept {
 return static_cast<std::int16_t>(u16(s,o));
}
}

void TileMapBuild1558::run(TileMapBuildState1558& s,
                           const TileMapBuildOps1558& o) {
    s.width12756=s.sourceWidth25619;
    s.height12754=s.sourceHeight2561B;

    const std::int32_t w=s.width12756;
    const std::int32_t h=s.height12754;
    if (w<=0) return;

    auto openNeighbor=[&](std::int32_t index) noexcept {
        const std::size_t p=static_cast<std::size_t>(index)*2u;
        return (s.cells198E0[p+1u]&0x04u)!=0 &&
               (s.cells198E0[p]&0x3Fu)==0;
    };
    auto marker=[&]() noexcept -> std::uint8_t {
        return s.value258A8==0xFFu ? 0x2Fu : 0x2Eu;
    };

    for(std::int32_t x=0;x<w;++x) {
        for(std::int32_t y=0;y<h;++y) {
            const std::int32_t index=w*y+x;
            const std::size_t p=static_cast<std::size_t>(index)*2u;
            std::uint8_t out=0xFFu;

            if(y==s.playerY255D6 && x==s.playerX255D2) {
                out=marker();
            } else if((s.cells198E0[p+1u]&0x04u)!=0) {
                const std::uint8_t raw=s.cells198E0[p];
                const std::uint8_t id=raw&0x3Fu;
                if(id==0 || s16(s.types25622,static_cast<std::size_t>(id)*2u)==0x93) {
                    out=marker();
                } else if((s.cells198E0[p+1u]&0x08u)!=0) {
                    out=5;
                } else if((raw&0x80u)==0) {
                    out=1;
                } else if(o.call1E8C8(x,y)==0) {
                    out=3;
                } else {
                    out=o.call1E930(index)==0x8000u ? 4u : 2u;
                }
            } else {
                const std::uint8_t id=s.cells198E0[p]&0x3Fu;
                if(id!=0) {
                    if(s16(s.types25622,static_cast<std::size_t>(id)*2u)==0x93) {
                        out=marker();
                    } else {
                        bool adjacent=false;
                        if(x>0) {
                            if(y>0) adjacent|=openNeighbor(index-w-1);
                            adjacent|=openNeighbor(index-1);
                            if(y<h-1) adjacent|=openNeighbor(index+w-1);
                        }
                        if(y>0) adjacent|=openNeighbor(index-w);
                        if(y<h-1) adjacent|=openNeighbor(index+w);
                        if(x<w-1) {
                            if(y>0) adjacent|=openNeighbor(index-w+1);
                            adjacent|=openNeighbor(index+1);
                            if(y<h-1) adjacent|=openNeighbor(index+w+1);
                        }
                        if(adjacent) out=1;
                    }
                }
            }

            s.output1760[static_cast<std::size_t>(index)]=out;
        }
    }
}

} // namespace ravenloft::game
