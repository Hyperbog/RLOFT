#include "rloft/SAVE.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
void require(bool yes,const char* message) {
    if(!yes)throw std::runtime_error(message);
}
template<class F> void rejects(F action) {
    bool failed=false;
    try{action();}catch(const std::exception&){failed=true;}
    require(failed,"invalid SAVE.DAT operation accepted");
}
void write32(std::vector<std::uint8_t>& b,std::size_t at,std::uint32_t v) {
    for(unsigned i=0;i<4;++i)b.at(at+i)=static_cast<std::uint8_t>(v>>(i*8U));
}
std::vector<std::uint8_t> synthetic() {
    std::vector<std::uint8_t> bytes(rloft::SaveDat::kZeroEffectFileBytes);
    for(std::size_t i=0;i<bytes.size();++i)
        bytes[i]=static_cast<std::uint8_t>(i*43U+17U);
    write32(bytes,rloft::SaveDat::kEffectCountAt,0U);
    return bytes;
}
void test_synthetic() {
    using rloft::SaveDat;
    const auto original=synthetic();
    SaveDat save(original);
    require(save.encode()==original,"synthetic SAVE file is not lossless");
    require(save.layout().transient_count==0U &&
            save.layout().container_at==0x9A7U &&
            save.layout().roster_at==0xD68U &&
            save.layout().container_ui_at==0x2438U,
            "SAVE original zero-effect offsets incorrect");
    const auto globals=save.globals();
    save.set_globals(globals);
    require(save.encode()==original,"SAVE global projection not reversible");
    for(std::size_t i=0;i<SaveDat::kPartyActors;++i) {
        auto actor=save.party(i);
        save.set_party(i,actor);
    }
    for(std::size_t i=0;i<SaveDat::kRosterActors;++i) {
        auto actor=save.roster(i);
        save.set_roster(i,actor);
    }
    for(std::size_t i=0;i<SaveDat::kContainerSlots.size();++i) {
        auto state=save.container(i);
        require(state.object_ids.size()==SaveDat::kContainerSlots[i],
                "SAVE container slot count incorrect");
        save.set_container(i,state);
    }
    auto spell=save.selected_spell_characters();
    save.set_selected_spell_characters(spell);
    save.set_gameplay_ticks(save.gameplay_ticks());
    save.set_environment_controls(save.environment_shading_level(),
                                  save.level_light_adjustment());
    save.set_charged_items(save.charged_items());
    for(std::size_t i=0;i<4U;++i)save.set_container_ui(i,save.container_ui(i));
    require(save.encode()==original,"SAVE unchanged typed images altered backing bytes");

    auto edited=save.globals();
    edited.level=22U;
    edited.facing=270U;
    edited.time_of_day_16_16=0x000E8000U;
    edited.held_object_id=-1;
    edited.held_item_known=1U;
    save.set_globals(edited);
    auto actor=save.party(2U);actor[0x248U-1U]=0x47U;save.set_party(2U,actor);
    auto reserve=save.roster(9U);reserve[1U]=0xB5U;save.set_roster(9U,reserve);
    auto container=save.container(61U);container.object_ids[0]=-1;
    container.state_bits^=0x80U;save.set_container(61U,container);
    save.set_selected_spell_characters({1U,3U});
    save.set_gameplay_ticks(0xFFFFFFFFU);
    save.set_environment_controls(10U,8U);
    auto charges=save.charged_items();charges[14U]=7U;save.set_charged_items(charges);
    save.set_container_ui(3U,{{0xFFU,0xFFU,0x00U,0xFFU}});
    require(save.encode()!=original,"SAVE typed edits not serialized");
    const SaveDat changed(save.encode());
    require(changed.globals().held_object_id==-1 &&
            changed.globals().time_of_day_16_16==0x000E8000U &&
            changed.party(2U)==actor && changed.roster(9U)==reserve &&
            changed.container(61U).object_ids[0]==-1 &&
            changed.selected_spell_characters()[1U]==3U &&
            changed.gameplay_ticks()==0xFFFFFFFFU &&
            changed.environment_shading_level()==10U &&
            changed.level_light_adjustment()==8U &&
            changed.charged_items()[14U]==7U &&
            changed.container_ui(3U)[2U]==0U,
            "SAVE edited source cannot reload");

    auto original_tail=save.encode();
    auto effect=SaveDat::TransientImage{};
    for(std::size_t i=0;i<effect.size();++i)effect[i]=static_cast<std::uint8_t>(i+1U);
    save.replace_transients({effect,effect});
    require(save.encode().size()==original_tail.size()+44U &&
            save.layout().transient_count==2U &&
            save.layout().container_at==0x9A7U+44U &&
            save.transient(1U)==effect &&
            save.container(61U).state_bits==changed.container(61U).state_bits &&
            save.roster(9U)==reserve &&
            save.container_ui(3U)==changed.container_ui(3U),
            "SAVE variable-length effects corrupted later sections");
    auto rebuilt=SaveDat(save.encode());
    require(rebuilt.encode()==save.encode(),"SAVE dynamic binary reload failed");
    save.replace_transients({});
    require(save.encode()==original_tail,
            "SAVE effects removal did not restore every original byte");
    rejects([&]{(void)save.transient(0U);});
    rejects([&]{(void)save.container(62U);});
    rejects([&]{(void)save.party(4U);});
    rejects([&]{(void)save.roster(10U);});
    rejects([&]{(void)save.container_ui(4U);});
    rejects([&]{save.set_container(0U,{});});
    rejects([&]{save.replace_transients(std::vector<SaveDat::TransientImage>(151U));});
    require(save.encode()==original_tail,"rejected SAVE edit damaged state");
    auto invalid=original;
    invalid.pop_back();
    rejects([&]{(void)SaveDat(invalid);});
    invalid=original;
    invalid.push_back(0U);
    rejects([&]{(void)SaveDat(invalid);});
    invalid=original;
    write32(invalid,SaveDat::kEffectCountAt,151U);
    rejects([&]{(void)SaveDat(invalid);});
    rejects([&]{(void)SaveDat({});});
}
void test_original(const std::vector<std::uint8_t>& bytes) {
    using rloft::SaveDat;
    SaveDat save(bytes);
    require(save.encode()==bytes,"uploaded original SAVE changed on load");
    const auto info=save.layout();
    require(info.container_at+0x3AAU==info.shading_at &&
            info.roster_at+10U*0x248U==info.container_ui_at,
            "original SAVE section boundaries invalid");
    auto original_globals=save.globals();
    save.set_globals(original_globals);
    for(std::size_t i=0;i<4U;++i)save.set_party(i,save.party(i));
    for(std::size_t i=0;i<10U;++i)save.set_roster(i,save.roster(i));
    for(std::size_t i=0;i<62U;++i)save.set_container(i,save.container(i));
    for(std::size_t i=0;i<4U;++i)save.set_container_ui(i,save.container_ui(i));
    save.set_selected_spell_characters(save.selected_spell_characters());
    save.set_gameplay_ticks(save.gameplay_ticks());
    save.set_environment_controls(save.environment_shading_level(),
                                  save.level_light_adjustment());
    save.set_charged_items(save.charged_items());
    require(save.encode()==bytes,"original SAVE typed sections not byte-exact");
    // Changing one source field never alters any other byte in the image.
    auto altered=original_globals;
    altered.facing^=1U;
    save.set_globals(altered);
    const SaveDat restored(save.encode());
    require(restored.globals().facing==altered.facing,
            "original SAVE edit failed to reload");
    for(std::size_t i=0;i<bytes.size();++i) {
        if(i<0x5EU || i>=0x62U)
            require(save.encode()[i]==bytes[i],"original SAVE unrelated bytes modified");
    }
    save.set_globals(original_globals);
    require(save.encode()==bytes,"original SAVE was not restored byte-exactly");
}
} // namespace
int main(int argc,char** argv) {
    test_synthetic();
    if(argc>2)throw std::invalid_argument("usage: SAVE_tests [SAVE.DAT]");
    if(argc==2) {
        std::ifstream f(argv[1],std::ios::binary);
        if(!f)throw std::runtime_error("Cannot open original SAVE.DAT");
        const std::vector<std::uint8_t> bytes(
            (std::istreambuf_iterator<char>(f)),std::istreambuf_iterator<char>());
        test_original(bytes);
    }
}
