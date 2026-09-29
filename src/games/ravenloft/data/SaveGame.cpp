#include "games/ravenloft/data/SaveGame.h"
#include "games/ravenloft/data/Binary.h"
#include "engine/core/FileSystem.h"
#include <algorithm>
#include <fstream>
#include <limits>
#include <span>
#include <stdexcept>

namespace ravenloft::data {
namespace {
constexpr std::size_t kShippedSaveSize = 9288;
constexpr std::size_t kPartyOffset = 0x77;
constexpr std::size_t kCharacterSize = 0x248;
constexpr std::size_t kInventoryOffset = 0x40;
constexpr std::size_t kInventorySlots = 14;
constexpr std::size_t kDynamicCountOffset = kPartyOffset + kCharacterSize*4; // 0x997

std::string fixedString(std::span<const std::uint8_t> b, std::size_t offset, std::size_t maxLen) {
    requireRange(b, offset, maxLen, "fixed string");
    std::string out;
    for (std::size_t i=0; i<maxLen && b[offset+i] != 0; ++i) {
        const auto c = b[offset+i];
        if (c < 0x20 || c > 0x7e) break;
        out.push_back(static_cast<char>(c));
    }
    return out;
}

CharacterRecord decodeCharacter(std::span<const std::uint8_t> r) {
    if (r.size() != kCharacterSize) throw std::runtime_error("Invalid Ravenloft character record size");
    CharacterRecord c;
    std::copy(r.begin(), r.end(), c.raw.begin());
    c.name = fixedString(r, 0x00, 16);
    c.status = u16(r, 0x10);
    c.portrait = r[0x12];
    c.sex = r[0x13];
    c.race = r[0x14];
    c.classes = {r[0x15], r[0x16], r[0x17]};
    c.alignment = r[0x1A];
    // CODE.1 0x37C0 rolls the initial HP total into +0x1E, and finalization
    // at 0x4F2E copies +0x1E to +0x1C. CODE.2 preserves these same 0x248-byte
    // records, making +0x1C current HP and +0x1E maximum/base HP.
    c.currentHitPoints = u16(r, 0x1C);
    c.maximumHitPoints = u16(r, 0x1E);
    c.levels = {r[0x20], r[0x22], r[0x24]};
    c.xp = {u32(r,0x26),u32(r,0x2A),u32(r,0x2E)};
    // CODE.1 writes the six generated ability values as paired bytes at +0x32..+0x3D.
    for (std::size_t i=0; i<6; ++i) c.abilities[i] = r[0x32 + i*2];
    c.exceptionalStrength = r[0x3E];
    // The first fourteen 16-bit item references at +0x40 are the carried inventory array.
    for (std::size_t i=0; i<kInventorySlots; ++i) c.carriedItems[i] = u16(r, kInventoryOffset + i*2);
    return c;
}

void appendU16(std::vector<std::uint8_t>& out,std::uint16_t value){
    out.push_back(static_cast<std::uint8_t>(value&0xFFu));
    out.push_back(static_cast<std::uint8_t>((value>>8u)&0xFFu));
}
void appendI16(std::vector<std::uint8_t>& out,std::int16_t value){appendU16(out,static_cast<std::uint16_t>(value));}
void appendU32(std::vector<std::uint8_t>& out,std::uint32_t value){
    out.push_back(static_cast<std::uint8_t>(value&0xFFu));
    out.push_back(static_cast<std::uint8_t>((value>>8u)&0xFFu));
    out.push_back(static_cast<std::uint8_t>((value>>16u)&0xFFu));
    out.push_back(static_cast<std::uint8_t>((value>>24u)&0xFFu));
}
void appendI32(std::vector<std::uint8_t>& out,std::int32_t value){appendU32(out,static_cast<std::uint32_t>(value));}

template<class Range>
void appendBytes(std::vector<std::uint8_t>& out,const Range& bytes){out.insert(out.end(),bytes.begin(),bytes.end());}

std::array<std::uint8_t,50> encodedDescription(const QuickStartSave& save){
    auto raw=save.descriptionRaw;
    std::span<const std::uint8_t> rs(raw.data(),raw.size());
    if(fixedString(rs,0,rs.size())==save.description)return raw;
    raw.fill(0);
    const auto n=std::min(save.description.size(),raw.size());
    for(std::size_t i=0;i<n;++i)raw[i]=static_cast<std::uint8_t>(save.description[i]);
    return raw;
}
}

const std::array<std::uint8_t,kContainerDescriptorCount>& containerDescriptorCapacities() {
    // CODE.2 initialized descriptor table DS:7330, 62 entries x 10 bytes; byte +0 is capacity.
    static constexpr std::array<std::uint8_t,kContainerDescriptorCount> caps{{
        4,8,8,8,6,6,8,6,8,8,4,8,8,6,8,4,8,4,8,8,8,8,8,8,6,8,8,4,6,8,8,
        8,8,8,8,8,8,8,4,8,8,8,4,8,8,6,6,8,8,8,4,8,8,8,6,8,4,6,8,8,6,8
    }};
    return caps;
}

bool CharacterRecord::active() const {
    return !name.empty() && (status & 0x8000u) == 0;
}

SaveGame::SaveGame(const std::filesystem::path& path) {
    auto bytes = ssi::core::readFile(path);
    if(bytes.size()<kShippedSaveSize)throw std::runtime_error("Ravenloft SAVE.DAT is smaller than the base writer layout");
    std::span<const std::uint8_t> b(bytes.data(), bytes.size());

    std::copy_n(bytes.begin(),quickStart_.descriptionRaw.size(),quickStart_.descriptionRaw.begin());
    quickStart_.description = fixedString(b, 0, quickStart_.descriptionRaw.size());
    // CODE.2 0x67D4..0x6A23: fixed 50-byte description, 32-byte quest
    // bitfield, current level, 16.16 X/Y, facing, and the fixed DS fields.
    std::copy_n(bytes.begin()+0x32, 32, quickStart_.questFlags.begin());
    quickStart_.level = u32(b, 0x52);
    quickStart_.partyXFixed = i32(b, 0x56);
    quickStart_.partyYFixed = i32(b, 0x5A);
    quickStart_.facingDegrees = i32(b, 0x5E);
    quickStart_.savedF03C = u16(b,0x62);
    quickStart_.savedF03E = u16(b,0x64);
    quickStart_.savedFD08 = u32(b,0x66);
    quickStart_.saved6DAE = u16(b,0x6A);
    quickStart_.timerF58C = u32(b, 0x6C);
    quickStart_.worldClock = u32(b, 0x70);
    quickStart_.heldObjectId = i16(b,0x74);
    quickStart_.heldObjectState = b[0x76];

    requireRange(b, kPartyOffset, kCharacterSize*4, "SAVE.DAT party records");
    for (std::size_t i=0; i<4; ++i) {
        const auto offset = kPartyOffset + i*kCharacterSize;
        quickStart_.party[i] = decodeCharacter(b.subspan(offset, kCharacterSize));
    }

    // CODE.2 0x6A53..0x6B7D / loader 0x6DE3..0x6F12:
    // four-byte dynamic count, count*0x16 records, eight auxiliary bytes,
    // A7E4 clock, then 62 variable-capacity container WORD arrays, each
    // followed by descriptor byte +9.
    quickStart_.dynamicEntityCount = u32(b,kDynamicCountOffset);
    const std::size_t remainingAfterCount=bytes.size()-(kDynamicCountOffset+4u);
    if(quickStart_.dynamicEntityCount>remainingAfterCount/kSaveDynamicRecordSize)
        throw std::runtime_error("SAVE.DAT dynamic entity count exceeds file size");
    std::size_t pos=kDynamicCountOffset+4u;
    quickStart_.dynamicEntities.resize(quickStart_.dynamicEntityCount);
    for(auto& record:quickStart_.dynamicEntities){
        requireRange(b,pos,record.size(),"SAVE.DAT dynamic entity record");
        std::copy_n(bytes.begin()+static_cast<std::ptrdiff_t>(pos),record.size(),record.begin());
        pos+=record.size();
    }
    requireRange(b,pos,quickStart_.savedD8A8.size(),"SAVE.DAT D8A8 block");
    std::copy_n(bytes.begin()+static_cast<std::ptrdiff_t>(pos),quickStart_.savedD8A8.size(),quickStart_.savedD8A8.begin());
    pos+=quickStart_.savedD8A8.size();
    requireRange(b,pos,4,"SAVE.DAT A7E4 gameplay clock");
    quickStart_.gameplayClockA7E4=u32(b,pos); pos+=4u;

    const auto& caps=containerDescriptorCapacities();
    for(std::size_t i=0;i<kContainerDescriptorCount;++i){
        requireRange(b,pos,static_cast<std::size_t>(caps[i])*2u+1u,"SAVE.DAT container descriptor");
        auto& items=quickStart_.containerItems[i]; items.resize(caps[i]);
        for(std::size_t j=0;j<caps[i];++j)items[j]=i16(b,pos+j*2u);
        quickStart_.containerState[i]=b[pos+static_cast<std::size_t>(caps[i])*2u];
        pos += static_cast<std::size_t>(caps[i])*2u+1u;
    }
    requireRange(b,pos,8,"SAVE.DAT lighting state");
    quickStart_.savedBaseDarkness = i32(b,pos); pos+=4u;
    quickStart_.ambientLightModifier = i32(b,pos); pos+=4u;

    requireRange(b,pos,quickStart_.saved67EC.size(),"SAVE.DAT DS:67EC block");
    std::copy_n(bytes.begin()+static_cast<std::ptrdiff_t>(pos),quickStart_.saved67EC.size(),quickStart_.saved67EC.begin());
    pos+=quickStart_.saved67EC.size();
    requireRange(b,pos,quickStart_.savedA808.size(),"SAVE.DAT DS:A808 block");
    std::copy_n(bytes.begin()+static_cast<std::ptrdiff_t>(pos),quickStart_.savedA808.size(),quickStart_.savedA808.begin());
    pos+=quickStart_.savedA808.size();
    requireRange(b,pos,quickStart_.saved6FA0.size(),"SAVE.DAT DS:6FA0 block");
    std::copy_n(bytes.begin()+static_cast<std::ptrdiff_t>(pos),quickStart_.saved6FA0.size(),quickStart_.saved6FA0.begin());
    pos+=quickStart_.saved6FA0.size();
    if(pos!=bytes.size())throw std::runtime_error("Unexpected trailing bytes after Ravenloft SAVE.DAT writer layout");
}

std::vector<std::uint8_t> SaveGame::encode(const QuickStartSave& save){
    if(save.dynamicEntityCount!=save.dynamicEntities.size())
        throw std::runtime_error("SAVE.DAT dynamicEntityCount does not match dynamicEntities");
    if(save.dynamicEntities.size()>std::numeric_limits<std::uint32_t>::max())
        throw std::runtime_error("SAVE.DAT dynamic entity count overflow");

    std::size_t containerBytes=0;
    const auto& caps=containerDescriptorCapacities();
    for(std::size_t i=0;i<caps.size();++i){
        if(save.containerItems[i].size()!=caps[i])throw std::runtime_error("SAVE.DAT container capacity mismatch");
        containerBytes+=static_cast<std::size_t>(caps[i])*2u+1u;
    }
    const std::size_t expected=kShippedSaveSize+save.dynamicEntities.size()*kSaveDynamicRecordSize;
    std::vector<std::uint8_t> out; out.reserve(expected);

    // CODE.2 0x67D4..0x6A23 fixed writer sequence.
    appendBytes(out,encodedDescription(save));
    appendBytes(out,save.questFlags);
    appendU32(out,save.level);
    appendI32(out,save.partyXFixed);
    appendI32(out,save.partyYFixed);
    appendI32(out,save.facingDegrees);
    appendU16(out,save.savedF03C);
    appendU16(out,save.savedF03E);
    appendU32(out,save.savedFD08);
    appendU16(out,save.saved6DAE);
    appendU32(out,save.timerF58C);       // writer calls 0x30700 first
    appendU32(out,save.worldClock);
    appendI16(out,save.heldObjectId);
    out.push_back(save.heldObjectState);
    for(const auto& character:save.party)appendBytes(out,character.raw);

    // CODE.2 0x6A53..0x6B7D.
    appendU32(out,save.dynamicEntityCount);
    for(const auto& record:save.dynamicEntities)appendBytes(out,record);
    appendBytes(out,save.savedD8A8);
    appendU32(out,save.gameplayClockA7E4);
    for(std::size_t i=0;i<caps.size();++i){
        for(const auto id:save.containerItems[i])appendI16(out,id);
        out.push_back(save.containerState[i]);
    }

    // CODE.2 0x6B7F..0x6C55.
    appendI32(out,save.savedBaseDarkness);
    appendI32(out,save.ambientLightModifier);
    appendBytes(out,save.saved67EC);
    appendBytes(out,save.savedA808);
    appendBytes(out,save.saved6FA0);

    if(out.size()!=expected || containerBytes!=938u)
        throw std::runtime_error("Internal Ravenloft SAVE.DAT writer size mismatch");
    return out;
}

void SaveGame::write(const std::filesystem::path& path,const QuickStartSave& save){
    const auto bytes=encode(save);
    std::ofstream file(path,std::ios::binary|std::ios::trunc);
    if(!file)throw std::runtime_error("Cannot open SAVE.DAT for writing: "+path.string());
    if(!bytes.empty())file.write(reinterpret_cast<const char*>(bytes.data()),static_cast<std::streamsize>(bytes.size()));
    if(!file)throw std::runtime_error("Cannot write SAVE.DAT: "+path.string());
}

const char* raceName(std::uint8_t race) {
    static constexpr const char* names[] = {"Human","Half-Elf","Halfling","Gnome","Dwarf","Elf"};
    return race < 6 ? names[race] : "Unused";
}
const char* className(std::uint8_t cls) {
    static constexpr const char* names[] = {"Fighter","Paladin","Ranger","Cleric","Mage","Thief"};
    return cls < 6 ? names[cls] : "Unused";
}
const char* alignmentName(std::uint8_t alignment) {
    static constexpr const char* names[] = {"LG","LN","NG","TN","CG","CN"};
    return alignment < 6 ? names[alignment] : "Unused";
}
}
