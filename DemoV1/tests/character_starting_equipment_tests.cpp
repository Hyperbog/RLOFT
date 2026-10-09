#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/character_transfer.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {
constexpr std::size_t kBase=0x3E724U;
constexpr std::size_t kTable=0x0988U;
constexpr std::size_t kFamilies=15U;
constexpr std::size_t kRaces=6U;
constexpr std::size_t kWords=14U;
constexpr std::size_t kObjectCount=336U;
void word(std::vector<std::uint8_t>& b,std::size_t at,std::uint16_t v){
    b.at(at)=static_cast<std::uint8_t>(v);
    b.at(at+1U)=static_cast<std::uint8_t>(v>>8U);
}
void dword(std::vector<std::uint8_t>& b,std::size_t at,std::uint32_t v){
    word(b,at,static_cast<std::uint16_t>(v));
    word(b,at+2U,static_cast<std::uint16_t>(v>>16U));
}
std::uint16_t read(const rloft::CharacterTransferImage& b,
                   std::size_t actor,std::size_t slot){
    const auto at=actor*0x248U+0x40U+slot*2U;
    return static_cast<std::uint16_t>(b[at]) |
        static_cast<std::uint16_t>(b[at+1U]<<8U);
}
std::uint16_t marker(std::size_t family,std::size_t race,
                     std::size_t word_index) {
    return static_cast<std::uint16_t>(
        (family*7U+race*19U+word_index*11U+1U)%330U);
}
rloft::CharacterTransferActor actor(int index,int race,
                                   std::array<int,3> classes) {
    rloft::CharacterTransferActor v{};
    v.draft.actor=index;
    v.draft.sex=index;
    v.draft.race=race;
    v.draft.classes=classes;
    v.draft.option=0;
    v.draft.face=0;
    v.draft.name=index==0?"EQUIP0":"EQUIP1";
    v.stats.abilities={{12,13,14,15,16,17}};
    v.stats.levels={{1,1,1}};
    v.stats.hit_points=12;
    return v;
}
template<class F> bool fails(F&& f) {
    try {f();}catch(const std::exception&){return true;}
    return false;
}
}
int main(){
    std::vector<std::uint8_t> executable(
        kBase+kTable+kFamilies*kRaces*kWords*2U,0U);
    // Every family/race/word gets a DISTINCT formula using an OBJECTS
    // ID that is valid in the original shipped 336-record corpus.
    // This tests all 90 matrix entries, not a single fixed test race.
    for(std::size_t family=0;family<kFamilies;++family)
      for(std::size_t race=0;race<kRaces;++race)
        for(std::size_t w=0;w<kWords;++w)
            word(executable,kBase+kTable+family*0xA8U+
                 race*0x1CU+w*2U,marker(family,race,w));

    std::vector<std::uint8_t> objects(
        4U+kObjectCount*0x41U,0U);
    dword(objects,0U,static_cast<std::uint32_t>(kObjectCount));
    const auto table=rloft::CharacterTransferTables::
        from_original_bytes(executable,objects);

    constexpr std::array<std::array<int,3>,15> kClassFamily{{
        {{0,-1,-1}},{{1,-1,-1}},{{2,-1,-1}},
        {{3,-1,-1}},{{4,-1,-1}},{{5,-1,-1}},
        {{0,5,-1}},{{0,3,-1}},{{0,4,-1}},
        {{3,5,-1}},{{2,3,-1}},{{3,4,-1}},
        {{4,5,-1}},{{0,4,5}},{{0,4,3}}
    }};
    constexpr std::array<std::size_t,4> kEquipSource{{2,3,1,0}};
    for(std::size_t family=0;family<kFamilies;++family) {
        for(std::size_t race=0;race<kRaces;++race) {
            const auto loadout=table.starting_loadout(family,race);
            assert(loadout.family==family);
            assert(loadout.race==race);
            assert(loadout.has_shipped_items());
            for(std::size_t slot=0;slot<21U;++slot) {
                const auto expected=slot<10U
                    ? marker(family,race,4U+slot)
                    : slot>=14U && slot<18U
                        ? marker(family,race,kEquipSource[slot-14U])
                        : 0xFFFFU;
                assert(static_cast<std::uint16_t>(loadout.slots[slot])
                       ==expected);
            }
            // Run the real two-character CODE.1 CHARS transfer,
            // using each family and each original race. Actor B
            // uses a different race to avoid duplicate-container
            // side effects unless testing the special path below.
            const auto other_race=static_cast<int>((race+1U)%kRaces);
            const std::array<rloft::CharacterTransferActor,2> pair{{
                actor(0,static_cast<int>(race),kClassFamily[family]),
                actor(1,other_race,{{1,-1,-1}})
            }};
            const auto chars=rloft::build_character_transfer(pair,table);
            for(std::size_t slot=0;slot<21U;++slot) {
                assert(read(chars,0U,slot)==
                       static_cast<std::uint16_t>(loadout.slots[slot]));
                assert(read(chars,1U,slot)==
                       static_cast<std::uint16_t>(
                           table.starting_loadout(1U,
                               static_cast<std::size_t>(other_race))
                                .slots[slot]));
            }
        }
    }
    // Source-unavailable class/race row is all FFFF, not an
    // invented generic loadout or a copy of a human fighter kit.
    const auto empty_at=kBase+kTable+1U*0xA8U+5U*0x1CU;
    for(std::size_t w=0;w<14U;++w)
        word(executable,empty_at+w*2U,0xFFFFU);
    const auto revised=rloft::CharacterTransferTables::
        from_original_bytes(executable,objects);
    assert(!revised.starting_loadout(1U,5U).has_shipped_items());

    // The second actor's duplicate category-1 item (e.g., container)
    // is suppressed only for identical family+race. Ordinary items
    // and a non-identical race keep their original ids.
    const auto conflict_id=marker(0U,0U,4U);
    objects[4U+static_cast<std::size_t>(conflict_id)*65U+0x2FU]=1U;
    const auto conflicts=rloft::CharacterTransferTables::
        from_original_bytes(executable,objects);
    const std::array<rloft::CharacterTransferActor,2> same{{
        actor(0,0,{{0,-1,-1}}),
        actor(1,0,{{0,-1,-1}})
    }};
    const auto same_chars=rloft::build_character_transfer(
        same,conflicts);
    assert(read(same_chars,0,0)==conflict_id);
    assert(read(same_chars,1,0)==0xFFFFU);
    const std::array<rloft::CharacterTransferActor,2> changed_race{{
        actor(0,0,{{0,-1,-1}}),
        actor(1,1,{{0,-1,-1}})
    }};
    const auto different=rloft::build_character_transfer(
        changed_race,conflicts);
    assert(read(different,1,0)==marker(0,1,4));

    assert(fails([&] { (void)table.starting_loadout(15,0); }));
    assert(fails([&] { (void)table.starting_loadout(0,6); }));
    executable.resize(kBase+kTable+kFamilies*0xA8U-1U);
    assert(fails([&] {
        (void)rloft::CharacterTransferTables::from_original_bytes(
            executable,objects);
    }));
    executable.resize(kBase+kTable+kFamilies*0xA8U,0U);
    word(executable,kBase+kTable+0U*0xA8U,500U);
    assert(fails([&] {
        (void)rloft::CharacterTransferTables::from_original_bytes(
            executable,objects);
    }));
    objects.pop_back();
    assert(fails([&] {
        (void)rloft::CharacterTransferTables::from_original_bytes(
            executable,objects);
    }));
}
