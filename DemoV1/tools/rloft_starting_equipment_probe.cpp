#include "rloft/character_transfer.hpp"
#include "rloft/game_source.hpp"
#include "rloft/object_catalog.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

namespace {
constexpr std::array<const char*,15> kFamily{{
    "FIGHTER","PALADIN","RANGER","CLERIC","MAGE","THIEF",
    "FIGHTER/THIEF","FIGHTER/CLERIC","FIGHTER/MAGE",
    "CLERIC/THIEF","RANGER/CLERIC","CLERIC/MAGE",
    "MAGE/THIEF","FIGHTER/MAGE/THIEF","FIGHTER/MAGE/CLERIC"
}};
constexpr std::array<const char*,6> kRace{{
    "HUMAN","ELF","DWARF","HALFLING","GNOME","HALF-ELF"
}};
const char* role(std::size_t slot) {
    if(slot<10U) return "PACK";
    if(slot==14U) return "HAND_A";
    if(slot==15U) return "HAND_B";
    if(slot==16U) return "BODY";
    if(slot==17U) return "HEAD";
    return "EMPTY";
}
}
int main(int argc,char** argv) {
    try {
        if(argc>2) {
            std::cerr<<"Usage: rloft_starting_equipment_probe"
                        " [original RLOFT directory or game.gog]\n";
            return 2;
        }
        auto source=argc==2
            ? rloft::GameSource::open(argv[1])
            : []()->rloft::GameSource {
                auto found=rloft::GameSource::auto_locate();
                if(!found)throw std::runtime_error(
                    "No original game found; pass RLOFT installation path");
                return std::move(*found);
            }();
        const auto table=rloft::CharacterTransferTables::
            from_original_bytes(
                source.read("CODE.1"),source.read("OBJECTS.DAT"));
        const auto objects=rloft::ObjectCatalog::load(source);
        std::size_t populated=0U,items=0U;
        std::cout<<"Original CODE.1 5F4C starting equipment\n"
                 <<"SOURCE=CODE.1_DGROUP_0988\n"
                 <<"OBJECT_COUNT="<<objects.size()<<"\n"
                 <<"FAMILY_COUNT=15\nRACE_COUNT=6\n";
        for(std::size_t family=0;family<kFamily.size();++family) {
            for(std::size_t race=0;race<kRace.size();++race) {
                const auto kit=table.starting_loadout(family,race);
                if(!kit.has_shipped_items())continue;
                ++populated;
                std::cout<<"LOADOUT\t"<<family<<"\t"<<kFamily[family]
                         <<"\t"<<race<<"\t"<<kRace[race]<<"\n";
                for(std::size_t slot=0;slot<kit.slots.size();++slot) {
                    const auto id=kit.slots[slot];
                    if(id<0)continue;
                    const auto& def=objects.at(
                        static_cast<std::size_t>(id));
                    ++items;
                    std::cout<<"ITEM\t"<<slot<<"\t"<<role(slot)
                             <<"\t"<<id<<"\t"<<def.name<<"\n";
                }
            }
        }
        std::cout<<"POPULATED_LOADOUTS="<<populated<<"\n"
                 <<"EMPTY_OR_UNAVAILABLE_ROWS="<<90U-populated<<"\n"
                 <<"TOTAL_STARTING_ITEM_OCCURRENCES="<<items<<"\n"
                 <<"NOTE=CODE.1 separately suppresses repeated category-1"
                   " item in actor 2 when family AND race match\n";
        // Original 1.2a data are 44 source-populated class/race rows.
        // Fail loudly on a different executable rather than creating
        // a deceptive "complete" report from an unknown version.
        if(populated!=44U || objects.size()!=336U) {
            std::cerr<<"Source-version warning: shipped CODE.1/OBJECTS"
                         " corpus differs from the audited 1.2a baseline\n";
            return 3;
        }
        return 0;
    }catch(const std::exception& e) {
        std::cerr<<"Original starting equipment audit error: "
                 <<e.what()<<"\n";
        return 1;
    }
}
