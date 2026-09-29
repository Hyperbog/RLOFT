#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/Binary.h"
#include "engine/core/FileSystem.h"
#include <algorithm>
#include <stdexcept>
namespace ravenloft::data {
NpcDatabase::NpcDatabase(const std::filesystem::path& path){auto b=ssi::core::readFile(path);auto n=u32(b,0);if(n!=83||b.size()!=4+n*69)throw std::runtime_error("Invalid NPCS.DAT");records_.reserve(n);for(std::uint32_t i=0;i<n;++i){std::span<const std::uint8_t> r(b.data()+4+i*69,69);NpcDefinition d;std::copy(r.begin(),r.end(),d.raw.begin());d.label=printablePrefix(r);d.baseSprite=u16(r,0x14);d.collisionHalfExtent=u16(r,0x16);d.facingStride=r[0x18];d.viewCount=r[0x19];d.actionGroup=r[0x1A];d.light=r[0x1B];d.reactionGroup=r[0x1C];d.hitDie=r[0x1D];d.damageDice=r[0x1E];d.damageDie=r[0x1F];d.damageBonus=r[0x20];d.armorClass=static_cast<std::int8_t>(r[0x21]);d.thac0=r[0x22];d.xp=u16(r,0x23);d.traits39=r[0x39];d.traits3A=r[0x3A];d.ambientSound=u16(r,0x3D);d.attackSound=u16(r,0x3F);d.hitSound=u16(r,0x41);d.deathSound=u16(r,0x43);records_.push_back(d);}}
ObjectDatabase::ObjectDatabase(const std::filesystem::path& path){auto b=ssi::core::readFile(path);auto n=u32(b,0);if(n!=336||b.size()!=4+n*65)throw std::runtime_error("Invalid OBJECTS.DAT");records_.reserve(n);for(std::uint32_t i=0;i<n;++i){std::span<const std::uint8_t> r(b.data()+4+i*65,65);ObjectDefinition d;std::copy(r.begin(),r.end(),d.raw.begin());d.label=printablePrefix(r);records_.push_back(d);}}
}
