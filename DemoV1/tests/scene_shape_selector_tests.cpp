#include "rloft/scene_shape_selector.hpp"

#include <cassert>
#include <stdexcept>

int main() {
    rloft::RendererTables tables{};
    tables.constants[2] = 160U; // C6D0.

    rloft::ExplorationSessionState session{};
    session.facing = 0;

    // LEV022-style Assassin facing 6, centered on screen.
    assert(
        rloft::SceneShapeSelectorModel::select_bank_local(
            315U, 160U, 0x0030U, 9U, 0U, 8U,
            session, tables) ==
        319U);

    // Reaction state 1 applies the encoded modulo-5/modulo-3 offset.
    assert(
        rloft::SceneShapeSelectorModel::select_bank_local(
            347U, 160U, 0x0032U, 9U, 0U, 8U,
            session, tables) ==
        348U);

    // State 3 performs no common 2B2F4 directional adjustment.
    assert(
        rloft::SceneShapeSelectorModel::select_bank_local(
            350U, 160U, 0x0036U, 9U, 0U, 8U,
            session, tables) ==
        350U);

    // A centered class-2 item with packed state zero selects subframe zero.
    assert(
        rloft::SceneShapeSelectorModel::select_bank_local(
            694U, 160U, 0U, 0x88U, 2U, 8U,
            session, tables) ==
        694U);

    // CODE.2 2B52D..2B5A0: with a centered target and facing
    // zero, relative angle -180+22 normalizes to 202 degrees.
    // Integer 202/45=4; 4 modulo 8 modulo 2 selects subframe ZERO.
    assert(
        rloft::SceneShapeSelectorModel::select_bank_local(
            100U, 160U, 0U, 1U, 2U, 2U,
            session, tables) ==
        100U);
    // A 45-degree relative change crosses to sector 5; 5%2 selects
    // subframe one. Exercise both parity cases from original IDIV.
    session.facing = 45U;
    assert(
        rloft::SceneShapeSelectorModel::select_bank_local(
            100U, 160U, 0U, 1U, 2U, 2U,
            session, tables) ==
        101U);
    session.facing = 0U;

    // Level 22 class-1 types 62..64 are outside the mutable cache groups.
    // Type 62 is also inside 2B188's 0x3C..0x43 stride-16 mirror range.
    assert(
        rloft::SceneShapeSelectorModel::select_bank_local(
            100U, 160U, 0U, 62U, 1U, 16U,
            session, tables) ==
        107U);

    // Normal stride-16 sources fold 16 half-sectors to eight frames.
    assert(
        rloft::SceneShapeSelectorModel::select_bank_local(
            100U, 160U, 0U, 62U, 2U, 16U,
            session, tables) ==
        100U);

    bool cache_rejected = false;
    try {
        (void)rloft::SceneShapeSelectorModel::select_bank_local(
            100U, 160U, 0U, 10U, 1U, 8U,
            session, tables);
    } catch (const std::logic_error&) {
        cache_rejected = true;
    }
    assert(cache_rejected);

    return 0;
}
