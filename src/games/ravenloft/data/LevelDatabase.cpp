#include "games/ravenloft/data/LevelDatabase.h"
#include "games/ravenloft/data/Binary.h"
#include "engine/core/FileSystem.h"
#include <algorithm>
#include <stdexcept>

namespace ravenloft::data {
LevelDatabase::LevelDatabase(const std::filesystem::path& path) : bytes_(ssi::core::readFile(path)) {
    const auto n=u32(bytes_,0); if(n!=32) throw std::runtime_error("Expected 32 LEVELS.DAT records");
    offsets_.reserve(n);
    for(std::uint32_t i=0;i<n;++i) offsets_.push_back(u32(bytes_,4+4*i));
    for(std::size_t i=0;i<offsets_.size();++i){const std::size_t a=offsets_[i]; const std::size_t e=(i+1<offsets_.size()?std::size_t(offsets_[i+1]):bytes_.size()); if(a>e||e>bytes_.size()) throw std::runtime_error("Invalid LEVELS.DAT offset table");}
}
LevelRecord LevelDatabase::load(std::size_t index) const {
    if(index>=offsets_.size()) throw std::out_of_range("Level index");
    const std::size_t a=offsets_[index], e=(index+1<offsets_.size()?offsets_[index+1]:bytes_.size());
    std::span<const std::uint8_t> d(bytes_.data()+a,e-a);
    LevelRecord L; L.index=static_cast<std::uint32_t>(index);
    for(int i=0;i<7;++i)L.sections[i]=u32(d,1+4*i);
    if(!std::is_sorted(L.sections.begin(),L.sections.end()) || L.sections.back()>d.size()) throw std::runtime_error("Invalid level section table");
    L.width=u16(d,30); L.height=u16(d,32); L.ceilingPlane=i16(d,34); L.floorPlane=i16(d,36);
    // Header fields below are addressed by the original renderer after the 29-byte
    // record prefix is stripped into DS:0x25618.  File offsets are therefore
    // DS-relative offset + 29.
    L.upperSourceRows=d[38];       // DS:0x25621, upper-span source row count.
    L.skySelector=d[685];          // DS:0x258A8, sky/background selector.
    L.ceilingFallbackColor=d[693];// DS:0x258B0, CODE.2 0x5A27D.
    L.floorFallbackColor=d[694];  // DS:0x258B1, CODE.2 0x5A2EE.
    L.paletteIndex=d[697];        // DS:0x258B4, palette selection path 0x53B37.
    if(!L.width||!L.height||L.width>128||L.height>128) throw std::runtime_error("Invalid level dimensions");
    if(L.sections[1] != 705u + 2u*L.width*L.height) throw std::runtime_error("Level grid boundary mismatch");
    for(int b=0;b<4;++b) for(int i=0;i<64;++i) L.wallBanks[b][i]=u16(d,39 + b*128 + i*2);
    L.cells.reserve(std::size_t(L.width)*L.height);
    for(std::size_t i=0;i<std::size_t(L.width)*L.height;++i)L.cells.push_back(u16(d,705+2*i));
    std::size_t pos=L.sections[1];
    for(std::uint16_t x=0;x<L.width;++x){
        const auto n=u16(d,pos);pos+=2;
        for(std::uint16_t j=0;j<n;++j){
            requireRange(d,pos,10,"interaction"); Interaction r; r.type=d[pos]; r.inputColumn=x; r.recordOffset=static_cast<std::uint32_t>(pos);
            std::copy_n(d.begin()+pos,10,r.raw.begin()); L.interactions.push_back(r); pos+=10;
        }
    }
    if(pos!=L.sections[2]) throw std::runtime_error("Interaction section mismatch");
    auto parsePlacements=[&](int section,std::size_t countOffset,std::vector<Placement>& out){
        const auto start=L.sections[section], end=L.sections[section+1];
        if((end-start)%11) throw std::runtime_error("Placement section stride mismatch");
        const auto declared=u16(d,countOffset); if((end-start)/11!=declared) throw std::runtime_error("Placement count mismatch");
        out.reserve(declared);
        for(std::size_t p=start;p<end;p+=11){Placement r; r.type=u16(d,p);r.x={u16(d,p+2),u16(d,p+4)};r.y={u16(d,p+6),u16(d,p+8)};r.flags=d[p+10];r.recordOffset=static_cast<std::uint32_t>(p);out.push_back(r);}
    };
    parsePlacements(2,679,L.npcs); parsePlacements(3,683,L.scenery); parsePlacements(4,681,L.objects);
    if(L.sections[6]-L.sections[5] != 1300) throw std::runtime_error("Event VM area is not 50x26");
    for(std::size_t bi=0;bi<50;++bi){
        const std::size_t base=L.sections[5]+bi*26; auto& block=L.events[bi];
        for(std::size_t ii=0;ii<5;++ii){const std::size_t q=base+ii*5;block.instructions[ii].opcode=d[q]; for(int k=0;k<4;++k)block.instructions[ii].operands[k]=d[q+1+k]; if(d[q]||d[q+1]||d[q+2]||d[q+3]||d[q+4])block.nonEmpty=true;}
        block.tail=d[base+25]; if(block.tail) block.nonEmpty=true;
    }
    pos=L.sections[6]; L.automap.width=u16(d,pos);L.automap.height=u16(d,pos+2);pos+=4;
    if(L.automap.width!=L.width||L.automap.height!=L.height)throw std::runtime_error("Automap dimensions mismatch");
    const std::size_t mapBytes=std::size_t(L.width)*L.height; requireRange(d,pos,mapBytes+4,"automap");
    L.automap.cells.assign(d.begin()+pos,d.begin()+pos+mapBytes);pos+=mapBytes;
    const auto noteLen=u32(d,pos);pos+=4;requireRange(d,pos,noteLen,"automap annotations");L.automap.annotations.assign(d.begin()+pos,d.begin()+pos+noteLen);pos+=noteLen;
    if(pos!=d.size())throw std::runtime_error("Automap section did not consume record");
    return L;
}
}
