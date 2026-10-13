#include "rloft_native/native_party_start.hpp"
#include "rloft/character_transfer.hpp"
#include "rloft/boot_flow.hpp"
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <tuple>

int main() {
    rloft::CharacterTransferTables original_shape{};
    for(auto& family:original_shape.family_race_words)
        for(auto& race:family)race.fill(0xFFFFU);
    std::array<rloft::CharacterTransferActor,2> original_actors{};
    for(int i=0;i<2;++i) {
        auto& a=original_actors[std::size_t(i)];
        a.draft.actor=i;
        a.draft.name=i==0?"VIKTOR":"ELENA";
        a.draft.sex=i;
        a.draft.race=i;
        a.draft.classes={{i,-1,-1}};
        a.draft.option=i;
        a.draft.face=i;
        a.stats.abilities.fill(12);
        a.stats.levels[0]=1;
        a.stats.hit_points=8+i;
    }
    const auto chars=rloft::build_character_transfer(
        original_actors,original_shape);
    static_assert(std::tuple_size<rloft::CharacterTransferImage>::value==0x920U,
        "source CHARS image must occupy four 0x248 records");
    // CODE.2 title -> R.EXE status10 -> CODE.1 -> CHARS -> CODE.2
    // -> world is the ONLY first-party startup route. No fake skip party.
    rloft::BootFlow supervisor;
    supervisor.boot();
    supervisor.finish_code2_startup(false,false);
    supervisor.finish_cold_start_presentation();
    supervisor.select_startup_menu(rloft::BootFlow::kCharacterWorkflowMenuIndex);
    supervisor.supervisor_dispatch();
    assert(supervisor.snapshot().stage==rloft::BootStage::CharacterCreation);
    supervisor.finish_character_creation(
        rloft::LegacyResult::ContinueModuleCycle);
    supervisor.supervisor_dispatch();
    assert(supervisor.snapshot().stage==rloft::BootStage::Code2Startup);
    assert(supervisor.snapshot().chars_present);
    const auto start=rloft_native::decode_native_party_start(chars);
    supervisor.finish_code2_startup(true,false);
    assert(supervisor.snapshot().stage==rloft::BootStage::Exploration);
    assert(start.chars==chars);
    assert(start.level==22U && start.column==4.5 && start.row==4.5);
    assert(start.heading_degrees==270U);
    assert(start.actors[0].name=="VIKTOR");
    assert(start.actors[1].name=="ELENA");
    assert(start.actors[0].classes[0]==0);
    assert(start.actors[1].classes[0]==1);
    assert(start.actors[1].portrait==1);
    assert(start.actors[0].current_hp==8);
    assert(start.actors[1].max_hp==9);

    auto reject=[](rloft_native::NativeCharsImage bad) {
        bool thrown=false;
        try {static_cast<void>(rloft_native::decode_native_party_start(bad));}
        catch(const std::runtime_error&){thrown=true;}
        assert(thrown);
    };
    auto bad=chars;
    bad[0x10U]=0xFFU;reject(bad);
    bad=chars;
    bad[0x248U+0x12U]=17U;reject(bad);
    bad=chars;
    bad[0x248U+0x13U]=3U;reject(bad);
    bad=chars;
    bad[0x15U]=0xFFU;reject(bad);
    bad=chars;
    bad[0x248U+0x1EU]=0xFFU;reject(bad);
    bad=chars;
    bad[0]='!';reject(bad);
    bad=chars;
    for(std::size_t i=0;i<16U;++i)bad[i]='A';
    reject(bad);
}
