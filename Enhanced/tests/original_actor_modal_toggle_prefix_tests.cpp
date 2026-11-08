#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_actor_modal_toggle_prefix.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

int main() {
    using rloft::OriginalActorModalModeState;
    using Adapter=rloft::OriginalActorModalTogglePrefix;
    static_assert(Adapter::kSourceF790Call==0x11B63U,"original E8 site");
    OriginalActorModalModeState state{};
    state.mode_6f90={{0xCAFEB000U,0x12345679U,0xDEADBE00U,0xFF00AA11U}};
    state.c050=0xAABBCC02U; // 291B0 reads BYTE, not whole DWORD
    state.lane_7f6c=0x50U;
    std::vector<std::string> effects;
    Adapter::execute_11ad8_to_11b68(state,1U,1,
        [&](std::uint8_t page) {
            assert(page==2U);
            assert(state.mode_6f90[1]==0x12345678U);
            effects.push_back("291B0");
        },
        [&](std::size_t actor) {
            assert(actor==1U && state.lane_7f6c==0x50U);
            effects.push_back("11268:1");
        },
        [&]() {effects.push_back("F790");});
    assert((effects==std::vector<std::string>{"291B0","11268:1","F790"}));
    assert((state.mode_6f90==std::array<std::uint32_t,4>{{
        0xCAFEB000U,0x12345678U,0xDEADBE00U,0xFF00AA11U}}));

    effects.clear();
    // Bit-2 branch: source captures selected DWORD before 291B0,
    // toggles low bit once, then writes that full DWORD to four lanes.
    Adapter::execute_11ad8_to_11b68(state,1U,3,
        [&](std::uint8_t page) {
            assert(page==2U);
            assert(state.mode_6f90[0]==0xCAFEB000U);
            state.mode_6f90[1]=0U; // callback must not alter captured EDI
            effects.push_back("291B0");
        },
        [&](std::size_t actor) {
            assert(state.mode_6f90[actor]==0x12345679U);
            assert(state.lane_7f6c==actor*80U);
            effects.push_back("11268:"+std::to_string(actor));
        },
        [&]() {
            assert(state.lane_7f6c==240U);
            effects.push_back("F790");
        });
    assert((effects==std::vector<std::string>{
        "291B0","11268:0","11268:1","11268:2","11268:3","F790"}));
    assert((state.mode_6f90==std::array<std::uint32_t,4>{{
        0x12345679U,0x12345679U,0x12345679U,0x12345679U}}));
    assert(state.c050==0xAABBCC02U); // later 11BAD/11C0F not in prefix

    // Original reload of held/known happens only at 11B53, after all
    // actor paints. A callback may change these live globals.
    std::int16_t held=42;
    std::uint8_t known=0;
    effects.clear();
    Adapter::execute_11ad8_to_11b68(state,3U,0,
        [&](std::uint8_t) {effects.push_back("291B0");},
        [&](std::size_t actor) {
            assert(actor==3U);
            held=-1;
            known=1U;
            effects.push_back("11268:3");
        },
        [&]() {
            assert(held==-1 && known==1U);
            effects.push_back("F790");
        });
    assert(effects.size()==3U && effects.back()=="F790");

    const auto before=state;
    bool called=false;
    try {
        Adapter::execute_11ad8_to_11b68(state,4U,2,
            [&](std::uint8_t){called=true;},
            [&](std::size_t){called=true;},
            [&](){called=true;});
    } catch(const std::out_of_range&) {}
    assert(!called && state.mode_6f90==before.mode_6f90 &&
           state.lane_7f6c==before.lane_7f6c);

    // No rollback is invented: the source byte XOR happens before the
    // 291B0 call, so an exceptional host callback leaves that write.
    bool threw=false;
    const auto old=state.mode_6f90[0];
    try {
        Adapter::execute_11ad8_to_11b68(state,0U,0,
            [&](std::uint8_t)->void{throw std::runtime_error("forced");},
            [&](std::size_t){called=true;},
            [&](){called=true;});
    }catch(const std::runtime_error&){threw=true;}
    assert(threw && !called && state.mode_6f90[0]==(old^1U));
}
