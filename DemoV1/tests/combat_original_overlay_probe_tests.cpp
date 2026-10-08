#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_overlay_probe.hpp"

#include <cassert>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <vector>

int main() {
    using namespace rloft;
    using Model=CombatOriginalOverlayProbeModel;
    CombatOriginalOverlayProbeState state{};
    CombatOriginalOverlayProbeServices api{};
    std::unordered_map<std::uint32_t,std::uint16_t> memory;
    unsigned reads=0U;
    api.original_word=[&](std::uint32_t addr)
        ->std::optional<std::uint16_t>{
        ++reads;
        const auto at=memory.find(addr);
        if(at==memory.end())return std::nullopt;
        return at->second;
    };
    const auto count=0xF03AU; // original C050=0 -> other page1
    const auto base=0x293A8U; // 27148 + one 2260 span
    const auto id=3U;
    auto seed=[&](std::uint32_t record,std::uint16_t flag,
                  std::uint16_t type,std::uint16_t frame){
        const auto p=base+record*0x16U;
        memory[p+8U]=flag;
        memory[p+6U]=id;
        memory[p+2U]=frame;
        memory[p+0xEU]=90U;
        memory[p+0x10U]=130U;
        memory[p+0x12U]=40U;
        memory[p+0x14U]=70U;
        memory[0x14D8EU+id*0x16U]=type;
        memory[0x22246U+id*0x16U]=type;
    };
    seed(0U,0U,0x50U,0x7EC8U);
    memory[count]=1U;
    std::vector<int> called;
    api.predicate_2ad18=[&](std::uint32_t type)
      ->std::optional<bool>{
        assert(type==0x50U);
        called.push_back(1);
        return false;
    };
    api.predicate_2ade0=[&](std::uint32_t type)
      ->std::optional<bool>{
        assert(type==0x50U);
        called.push_back(2);
        return false;
    };
    state.skip_probing_d680=1U;
    assert(Model::probe_2ae30(state,100U,50U,api)==0);
    assert(reads==0U);
    state.skip_probing_d680=0U;
    assert(Model::probe_2ae30(state,100U,50U,api)==1);
    assert((called==std::vector<int>{1,2}));
    called.clear();
    // Type 0x50 with frame row below source SAR threshold
    // is skipped BEFORE hit rectangle or helper callbacks.
    memory[base+2U]=0x7EC7U;
    assert(Model::probe_2ae30(state,100U,50U,api)==0);
    assert(called.empty());
    memory[base+2U]=0x7EC8U;
    assert(Model::probe_2ae30(state,89U,50U,api)==0);
    assert(Model::probe_2ae30(state,131U,50U,api)==0);
    assert(Model::probe_2ae30(state,100U,39U,api)==0);
    assert(Model::probe_2ae30(state,100U,71U,api)==0);
    assert(called.empty());
    assert(Model::probe_2ae30(state,90U,40U,api)==1);
    assert(Model::probe_2ae30(state,130U,70U,api)==1);
    called.clear();
    api.predicate_2ad18=[&](std::uint32_t)
      ->std::optional<bool>{called.push_back(1);return true;};
    assert(Model::probe_2ae30(state,100U,50U,api)==2);
    assert((called==std::vector<int>{1})); // short circuit
    called.clear();
    api.predicate_2ad18=[&](std::uint32_t)
      ->std::optional<bool>{called.push_back(1);return false;};
    api.predicate_2ade0=[&](std::uint32_t)
      ->std::optional<bool>{called.push_back(2);return true;};
    assert(Model::probe_2ae30(state,100U,50U,api)==2);
    assert((called==std::vector<int>{1,2}));
    // A flag-one record uses the SECOND type table 22246, and
    // checks only the four original accepted type codes.
    seed(0U,1U,0x52U,0U);
    called.clear();
    assert(Model::probe_2ae30(state,100U,50U,api)==2);
    assert(called.empty());
    memory[0x22246U+id*0x16U]=0x71U;
    assert(Model::probe_2ae30(state,100U,50U,api)==2);
    memory[0x22246U+id*0x16U]=0x6EU;
    assert(Model::probe_2ae30(state,100U,50U,api)==2);
    memory[0x22246U+id*0x16U]=0x91U;
    assert(Model::probe_2ae30(state,100U,50U,api)==2);
    memory[0x22246U+id*0x16U]=0x77U;
    assert(Model::probe_2ae30(state,100U,50U,api)==0);
    memory[base+8U]=3U;
    assert(Model::probe_2ae30(state,100U,50U,api)==0);
    memory[count]=0U;
    assert(Model::probe_2ae30(state,100U,50U,api)==0);

    // Two records; first fails the inclusive bounds, then second
    // matches a valid type. Original always uses 22-byte stride.
    memory[count]=2U;
    seed(0U,1U,0x52U,0U);
    memory[base+0xEU]=111U;
    seed(1U,1U,0x52U,0U);
    assert(Model::probe_2ae30(state,100U,50U,api)==2);
    // Page inversion must preserve the other page's own table.
    state.active_scene_page_c050=1U;
    memory[0xF038U]=0U;
    assert(Model::probe_2ae30(state,100U,50U,api)==0);
    state.active_scene_page_c050=0U;
    memory.erase(base+0x16U+0x14U);
    assert(!Model::probe_2ae30(state,100U,50U,api));

    const auto bound=Model::bind_1a288(
        state,api,CombatOriginalOverlayServices{});
    assert(bound.probe_2ae30);
    bool failed=false;
    try{(void)bound.probe_2ae30(100,50);}
    catch(const std::logic_error&){failed=true;}
    assert(failed);
}
