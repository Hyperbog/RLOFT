#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_overlay_position.hpp"
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

int main(){
    using namespace rloft;
    CombatOriginalOverlayPositionState state{};
    CombatOriginalOverlayPositionServices callbacks{};
    std::vector<std::int32_t> moves;
    callbacks.pointer_move_33c90=[&](std::int32_t row,std::int32_t col){
        moves.push_back(row);
        moves.push_back(col);
    };
    callbacks.disabled_side_effect_1f820=[&](){
        state.row_a6b4=42;
        state.column_a6b6=-8;
        state.third_word_a6b8=17;
    };
    state.row_a6b4=-100;
    state.column_a6b6=500;
    state.third_word_a6b8=-12;
    // No mode active: raw globals returned, NO guessed clamp.
    auto r=CombatOriginalOverlayPositionModel::run_1a3b8(state,callbacks);
    assert(!r.requested_position_update&&r.output_row==-100);
    assert(r.output_column==500&&r.output_third_word==-12);
    assert(moves.empty());

    // Original A6BE clamp is [0x124,0x187] rows, [0x7e,0xc3]
    // columns. Stores clamped WORDs before the first 33C90 call.
    state.clamp_mode_a6be=1U;
    callbacks.pointer_move_33c90=[&](std::int32_t row,std::int32_t col){
        assert(state.row_a6b4==row&&state.column_a6b6==col);
        moves.push_back(row);moves.push_back(col);
    };
    r=CombatOriginalOverlayPositionModel::run_1a3b8(state,callbacks);
    assert(r.requested_position_update);
    assert(state.row_a6b4==0x124&&state.column_a6b6==0xc3);
    assert((moves==std::vector<std::int32_t>{0x124,0xc3}));
    moves.clear();

    // Original A6BC bounds [4,0x103] and [0x11,0x11f].
    // On a changed position, the original 33C90 runs FIRST,
    // then the globally stored WORDs are set to the targets.
    state.clamp_mode_a6be=0U;
    state.constrain_mode_a6bc=2U;
    state.row_a6b4=-20;state.column_a6b6=700;
    callbacks.pointer_move_33c90=[&](std::int32_t row,std::int32_t col){
        assert(row==4&&col==0x11f);
        assert(state.row_a6b4==-20&&state.column_a6b6==700);
        state.row_a6b4=123;state.column_a6b6=124; // side effects overwritten
        moves.push_back(row);moves.push_back(col);
    };
    r=CombatOriginalOverlayPositionModel::run_1a3b8(state,callbacks);
    assert(r.requested_position_update);
    assert(state.row_a6b4==4&&state.column_a6b6==0x11f);
    assert((moves==std::vector<std::int32_t>{4,0x11f}));
    moves.clear();
    // Original A6BC reports EBX=1 even if the target is unchanged.
    callbacks.pointer_move_33c90=[&](std::int32_t,std::int32_t){
        assert(false&&"no service call for already clamped position");
    };
    r=CombatOriginalOverlayPositionModel::run_1a3b8(state,callbacks);
    assert(r.requested_position_update&&moves.empty());

    // First callback can change whether second A6BC branch runs.
    state.row_a6b4=300;state.column_a6b6=100;
    state.clamp_mode_a6be=1U;
    state.constrain_mode_a6bc=0U;
    callbacks.pointer_move_33c90=[&](std::int32_t row,std::int32_t col){
        moves.push_back(row);moves.push_back(col);
        state.constrain_mode_a6bc=1U;
    };
    r=CombatOriginalOverlayPositionModel::run_1a3b8(state,callbacks);
    assert(r.requested_position_update);
    assert(moves.size()==4U); // first A6BE, second A6BC
    assert(state.row_a6b4==0x103);

    // Disabled D680 zeros mode WORDs and invokes 1F820 before
    // reading all three original output globals.
    state.disabled_d680=1U;
    state.clamp_mode_a6be=9U;
    state.constrain_mode_a6bc=9U;
    r=CombatOriginalOverlayPositionModel::run_1a3b8(state,callbacks);
    assert(!r.requested_position_update);
    assert(state.clamp_mode_a6be==0U&&state.constrain_mode_a6bc==0U);
    assert(r.output_row==42&&r.output_column==-8);
    assert(r.output_third_word==17);

    // Directly bind all three DOS WORD outputs into 1A288 state.
    CombatOriginalCompositeState compositor{};
    CombatOriginalOverlayGlobals overlay{compositor};
    state.disabled_d680=0U;
    state.row_a6b4=-2;state.column_a6b6=8;state.third_word_a6b8=1234;
    const auto adapter=CombatOriginalOverlayPositionModel::bind_1a288(
        state,callbacks,CombatOriginalOverlayServices{});
    assert(adapter.position_1a3b8);
    adapter.position_1a3b8(overlay);
    assert(compositor.overlay_row_9e31==-2);
    assert(compositor.overlay_column_9e33==8);
    assert(compositor.overlay_third_word_9e35==1234);

    // Missing original services fail closed instead of inventing
    // host cursor movement or alternate disable behavior.
    state.clamp_mode_a6be=1U;
    bool failed=false;
    try{
        (void)CombatOriginalOverlayPositionModel::run_1a3b8(
            state,CombatOriginalOverlayPositionServices{});
    }catch(const std::invalid_argument&){failed=true;}
    assert(failed);
}
