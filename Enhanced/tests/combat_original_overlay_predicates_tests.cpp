#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_overlay_predicates.hpp"

#include <cassert>
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <vector>

int main() {
    using Model=rloft::CombatOriginalOverlayPredicatesModel;
    rloft::CombatOriginalObjectTypeBitServices memory{};
    std::unordered_map<std::uint32_t,std::uint32_t> bitwords;
    std::vector<std::uint32_t> reads;
    memory.original_dgroup_dword=[&](std::uint32_t address)
        ->std::optional<std::uint32_t> {
        reads.push_back(address);
        const auto entry=bitwords.find(address);
        if(entry==bitwords.end())return std::nullopt;
        return entry->second;
    };
    // Constant type classes never touch original world state.
    for(std::uint32_t type=0x22U;type<=0x46U;++type){
        const auto classification=Model::type_2ad18(type,memory);
        if(type==0x3AU)assert(classification&&!*classification);
        else if(type==0x3BU||type==0x3CU)assert(!classification);
        else assert(classification&&*classification);
    }
    assert(!reads.empty());
    reads.clear();
    for(const auto type:std::vector<std::uint32_t>{
        0U,0x21U,0x3AU,0x47U,0x48U,0x4DU,0x51U,0x53U,0xFFFFFFFFU}){
        assert(Model::type_2ad18(type,memory)==false);
    }
    assert(Model::type_2ad18(0x52U,memory)==true);
    assert(reads.empty());
    for(const auto type:std::vector<std::uint32_t>{
        0x48U,0x49U,0x4AU,0x4DU,0x4FU})
        assert(Model::type_2ade0(type));
    for(const auto type:std::vector<std::uint32_t>{
        0U,0x47U,0x4BU,0x4CU,0x4EU,0x50U,0x52U,0xFFFFFFFFU})
        assert(!Model::type_2ade0(type));

    // Original bit #0x48 is BIT 8 at DWORD:22208, not BIT 0x48.
    // Bits 0x35 and 0x36 live at DWORD:22204, bits 21/22.
    bitwords[0x22208U]=0U;
    bitwords[0x22204U]=0U;
    assert(Model::type_2ad18(0x3BU,memory)==false);
    assert((reads==std::vector<std::uint32_t>{0x22208U,0x22204U}));
    reads.clear();
    bitwords[0x22204U]=(1U<<21U);
    assert(Model::type_2ad18(0x3BU,memory)==true);
    assert(Model::type_2ad18(0x3CU,memory)==false);
    bitwords[0x22204U]|=(1U<<22U);
    assert(Model::type_2ad18(0x3CU,memory)==true);
    bitwords[0x22208U]=(1U<<8U);
    reads.clear();
    assert(Model::type_2ad18(0x3BU,memory)==false);
    assert((reads==std::vector<std::uint32_t>{0x22208U}));
    assert(Model::type_2ad18(0x3CU,memory)==false);
    bitwords.erase(0x22208U);
    assert(!Model::type_2ad18(0x3BU,memory));

    // Source memory must be read LIVE on each use: there are no
    // frozen classification flags from binder creation time.
    rloft::CombatOriginalOverlayProbeServices probe{};
    probe=Model::bind_2ae30(memory,probe);
    assert(probe.predicate_2ad18&&probe.predicate_2ade0);
    assert(probe.predicate_2ade0(0x49U)==true);
    assert(probe.predicate_2ade0(0x51U)==false);
    assert(!probe.predicate_2ad18(0x3BU));
    bitwords[0x22208U]=0U;
    assert(probe.predicate_2ad18(0x3BU)==true);
    bitwords[0x22208U]=(1U<<8U);
    assert(probe.predicate_2ad18(0x3BU)==false);
}
