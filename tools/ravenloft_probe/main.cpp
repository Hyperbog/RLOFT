#include "games/ravenloft/data/LevelDatabase.h"
#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/InitDatabase.h"
#include "games/ravenloft/data/TextDatabase.h"
#include "games/ravenloft/data/ResourceArchive.h"
#include "games/ravenloft/data/GraphicsDecode.h"
#include "games/ravenloft/data/SaveGame.h"
#include <filesystem>
#include <iostream>
#include <map>
#include <stdexcept>

using namespace ravenloft::data;
int main(int argc,char**argv){try{
    if(argc<2){std::cerr<<"Usage: ravenloft_probe <RLOFT directory> [level]\n";return 2;}
    std::filesystem::path root=argv[1];std::size_t level=argc>=3?std::stoul(argv[2]):22;
    LevelDatabase levels(root/"LEVELS.DAT");NpcDatabase npcs(root/"NPCS.DAT");ObjectDatabase objects(root/"OBJECTS.DAT");InitDatabase init(root/"INIT");TextDatabase text(root/"T");ResourceArchive res3(root/"RES3"),res4(root/"RES4");
    auto L=levels.load(level);
    std::cout<<"Ravenloft data probe\nLevel "<<level<<"  "<<L.width<<"x"<<L.height<<"\n";
    std::cout<<"Placements: NPC "<<L.npcs.size()<<", scenery "<<L.scenery.size()<<", objects "<<L.objects.size()<<"\n";
    std::cout<<"Interactions: "<<L.interactions.size()<<"\n";
    std::size_t eventCount=0;for(auto&e:L.events)if(e.nonEmpty)++eventCount;std::cout<<"Non-empty event blocks: "<<eventCount<<"\n";
    std::cout<<"Planes: ceiling="<<L.ceilingPlane<<" floor="<<L.floorPlane<<"\n";
    std::cout<<"NPCs:\n";for(auto&p:L.npcs){auto&d=npcs.at(p.type);std::cout<<"  type "<<p.type<<" "<<d.label<<" @ ("<<p.x.value()<<", "<<p.y.value()<<") flags="<<unsigned(p.flags)<<" HD="<<unsigned(d.hitDie)<<" AC="<<int(d.armorClass)<<" THAC0="<<unsigned(d.thac0)<<"\n";}
    std::map<std::uint16_t,std::size_t> sceneryTypes;for(auto&p:L.scenery)sceneryTypes[p.type]++;
    std::cout<<"Scenery types:\n";for(auto [type,n]:sceneryTypes){auto&s=init.scenery().at(type);std::cout<<"  type "<<type<<" x"<<n<<" base="<<s.base<<" resolvedSprite="<<(init.sceneryResourceBase()+s.base)<<" views="<<unsigned(s.views)<<" frames="<<unsigned(s.frames)<<" radius="<<s.radius<<" effect="<<unsigned(s.effect)<<"\n";}
    std::cout<<"Object placements:\n";for(auto&p:L.objects)std::cout<<"  type "<<p.type<<" "<<objects.at(p.type).label<<" @ ("<<p.x.value()<<", "<<p.y.value()<<") flags="<<unsigned(p.flags)<<"\n";
    std::cout<<"Resource counts: RES3="<<res3.count()<<" RES4="<<res4.count()<<" T="<<text.count()<<"\n";
    // Decode every RES3/RES4 asset directly referenced by this level's static scenery/wall tables as a format sanity check.
    std::size_t decodedSprites=0;for(auto [type,n]:sceneryTypes){auto idx=std::size_t(init.sceneryResourceBase()+init.scenery().at(type).base);if(idx<res3.count()){auto im=decodeSpriteType7(res3.record(idx));(void)im;++decodedSprites;}}
    std::size_t decodedWalls=0;for(auto const&bank:L.wallBanks)for(auto idx:bank)if(idx&&idx<res4.count()){auto im=decodeTextureType6(res4.record(idx));(void)im;++decodedWalls;}
    std::cout<<"Decoded referenced base sprites: "<<decodedSprites<<"; wall-bank entries: "<<decodedWalls<<"\n";
    auto savePath=root/"SAVE.DAT";if(std::filesystem::exists(savePath)){SaveGame save(savePath);std::cout<<"Quick Start: "<<save.quickStart().description<<"\n"<<"  Start level="<<save.quickStart().level<<" position=("<<save.quickStart().partyX()<<", "<<save.quickStart().partyY()<<") facing="<<save.quickStart().facingDegrees<<" degrees\n";for(std::size_t i=0;i<save.quickStart().party.size();++i){auto const&c=save.quickStart().party[i];if(!c.active())continue;std::cout<<"  PC"<<(i+1)<<" "<<c.name<<" — "<<raceName(c.race)<<" "<<className(c.classes[0]);if(c.classes[1]!=0xFF)std::cout<<"/"<<className(c.classes[1]);std::cout<<" "<<alignmentName(c.alignment)<<" L"<<unsigned(c.levels[0]);if(c.classes[1]!=0xFF)std::cout<<"/"<<unsigned(c.levels[1]);std::cout<<" items:";for(auto id:c.carriedItems)if(id!=0xFFFF && id<objects.count())std::cout<<" ["<<id<<" "<<objects.at(id).label<<"]";std::cout<<"\n";}}
    return 0;
}catch(const std::exception&e){std::cerr<<"ERROR: "<<e.what()<<"\n";return 1;}}
