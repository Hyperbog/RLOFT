#include "games/ravenloft/data/WorkLevel.h"
#include "games/ravenloft/data/Binary.h"
#include "engine/core/FileSystem.h"
#include <algorithm>
#include <span>
#include <stdexcept>

namespace ravenloft::data {
namespace {
RuntimePlacement22 parse22(std::span<const std::uint8_t> d, std::size_t p) {
    requireRange(d,p,22,"runtime placement");
    RuntimePlacement22 r;
    std::copy_n(d.begin()+p,22,r.raw.begin());
    r.yFixed = i32(d,p+0x00);
    r.xFixed = i32(d,p+0x04);
    r.state = u32(d,p+0x08);
    r.field0C = u16(d,p+0x0C);
    r.type = u16(d,p+0x0E);
    r.field10 = u16(d,p+0x10);
    r.hitPoints = i16(d,p+0x12);
    r.field14 = u16(d,p+0x14);
    return r;
}
}

WorkLevel::WorkLevel(const std::filesystem::path& path) {
    auto bytes=ssi::core::readFile(path);
    std::span<const std::uint8_t> d(bytes.data(),bytes.size());
    if(d.size()<705) throw std::runtime_error("LEV work file too small");
    snapshot_.marker=d[0];
    for(int i=0;i<7;++i) snapshot_.sections[i]=u32(d,1+4*i);
    if(!std::is_sorted(snapshot_.sections.begin(),snapshot_.sections.end()) || snapshot_.sections.back()>d.size())
        throw std::runtime_error("Invalid LEV work section table");
    snapshot_.width=u16(d,30); snapshot_.height=u16(d,32);
    snapshot_.ceilingPlane=i16(d,34); snapshot_.floorPlane=i16(d,36);
    if(!snapshot_.width||!snapshot_.height||snapshot_.width>128||snapshot_.height>128)
        throw std::runtime_error("Invalid LEV work dimensions");
    if(snapshot_.sections[1] != 705u + 2u*snapshot_.width*snapshot_.height)
        throw std::runtime_error("LEV work grid boundary mismatch");
    const std::size_t gridCount=std::size_t(snapshot_.width)*snapshot_.height;
    snapshot_.cells.reserve(gridCount);
    for(std::size_t i=0;i<gridCount;++i) snapshot_.cells.push_back(u16(d,705+2*i));

    // Mutable work files expand section 1 beyond the serialized LEVELS.DAT interaction bytes.
    // The exact per-runtime-record semantics are not needed for the opening demo yet, so retain
    // the section byte-for-byte rather than invent field names.
    snapshot_.interactionRuntimeBytes.assign(d.begin()+snapshot_.sections[1], d.begin()+snapshot_.sections[2]);

    const std::uint16_t npcCount=u16(d,679), objectCount=u16(d,681), sceneryCount=u16(d,683);
    auto parseArray=[&](int section,std::uint16_t count,std::vector<RuntimePlacement22>& out,const char* label){
        auto start=snapshot_.sections[section]; const auto end=snapshot_.sections[section+1];
        const auto payload=std::uint32_t(count)*22u;
        // The shipped LEV022.DAT scenery work section carries a 16-bit count prefix,
        // while its NPC section does not. Accept only the two layouts demonstrated by
        // the original work file; do not silently infer other padding.
        if(end-start==payload+2u && u16(d,start)==count) start+=2;
        if(end-start != payload) throw std::runtime_error(std::string(label)+" runtime stride/count mismatch");
        out.reserve(count);
        for(std::size_t p=start;p<end;p+=22) out.push_back(parse22(d,p));
    };
    parseArray(2,npcCount,snapshot_.npcs,"NPC");
    parseArray(3,sceneryCount,snapshot_.scenery,"scenery");
    parseArray(4,objectCount,snapshot_.objects,"object");

    if(snapshot_.sections[6]-snapshot_.sections[5] != 1300) throw std::runtime_error("LEV work event VM area is not 50x26");
    for(std::size_t bi=0;bi<50;++bi){
        const std::size_t base=snapshot_.sections[5]+bi*26; auto& block=snapshot_.events[bi];
        for(std::size_t ii=0;ii<5;++ii){
            const std::size_t q=base+ii*5; block.instructions[ii].opcode=d[q];
            for(int k=0;k<4;++k) block.instructions[ii].operands[k]=d[q+1+k];
            for(int k=0;k<5;++k) if(d[q+k]) block.nonEmpty=true;
        }
        block.tail=d[base+25]; if(block.tail) block.nonEmpty=true;
    }

    std::size_t pos=snapshot_.sections[6];
    snapshot_.automap.width=u16(d,pos); snapshot_.automap.height=u16(d,pos+2); pos+=4;
    if(snapshot_.automap.width!=snapshot_.width||snapshot_.automap.height!=snapshot_.height)
        throw std::runtime_error("LEV work automap dimensions mismatch");
    const std::size_t mapBytes=gridCount; requireRange(d,pos,mapBytes+4,"LEV work automap");
    snapshot_.automap.cells.assign(d.begin()+pos,d.begin()+pos+mapBytes); pos+=mapBytes;
    const auto noteLen=u32(d,pos); pos+=4; requireRange(d,pos,noteLen,"LEV work automap annotations");
    snapshot_.automap.annotations.assign(d.begin()+pos,d.begin()+pos+noteLen); pos+=noteLen;
    if(pos!=d.size()) throw std::runtime_error("LEV work automap did not consume file");
}
} // namespace ravenloft::data
