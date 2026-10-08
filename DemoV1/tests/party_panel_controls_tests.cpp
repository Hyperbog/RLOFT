#include "rloft/party_panel_controls.hpp"

#include <cassert>
#include <cstddef>

int main() {
    using rloft::PartyPanelControlModel;

    const auto& regions =
        PartyPanelControlModel::gameplay_inventory_regions();
    assert(regions[0].row_min == 0x138);
    assert(regions[0].column_min == 4);
    assert(regions[0].row_max == 0x187);
    assert(regions[0].column_max == 0x33);
    assert(regions[3].column_min == 0x10C);
    assert(regions[3].column_max == 0x13B);

    assert(PartyPanelControlModel::gameplay_actor_at(
               0x138, 4) == 0U);
    assert(PartyPanelControlModel::gameplay_actor_at(
               0x187, 0x33) == 0U);
    assert(PartyPanelControlModel::gameplay_actor_at(
               0x138, 0x39) == 1U);
    assert(PartyPanelControlModel::gameplay_actor_at(
               0x150, 0xD7) == 2U);
    assert(PartyPanelControlModel::gameplay_actor_at(
               0x187, 0x13B) == 3U);
    assert(!PartyPanelControlModel::gameplay_actor_at(
                0x137, 4).has_value());
    assert(!PartyPanelControlModel::gameplay_actor_at(
                0x150, 0x34).has_value());

    assert(PartyPanelControlModel::modal_actor_at_column(0) == 0U);
    assert(PartyPanelControlModel::modal_actor_at_column(79) == 0U);
    assert(PartyPanelControlModel::modal_actor_at_column(80) == 1U);
    assert(PartyPanelControlModel::modal_actor_at_column(319) == 3U);
    assert(!PartyPanelControlModel::modal_actor_at_column(320).has_value());

    // Inclusive 12E70 bounds.
    assert(PartyPanelControlModel::modal_toggle_actor_at(
               0x45, 0x39) == 0U);
    assert(PartyPanelControlModel::modal_toggle_actor_at(
               0x60, 0x4D) == 0U);
    assert(PartyPanelControlModel::modal_toggle_actor_at(
               0x45, 80 + 0x39) == 1U);
    assert(PartyPanelControlModel::modal_toggle_actor_at(
               0x60, 240 + 0x4D) == 3U);
    assert(!PartyPanelControlModel::modal_toggle_actor_at(
                0x44, 0x39).has_value());
    assert(!PartyPanelControlModel::modal_toggle_actor_at(
                0x45, 0x38).has_value());

    // 12E70 click rectangles are actual 32x16 tiles with two-pixel
    // horizontal and vertical gutters. Check all four actor columns.
    for(std::size_t actor=0U;actor<4U;++actor) {
        for(std::size_t slot=0U;slot<12U;++slot) {
            const int row=0x129+static_cast<int>(slot/4U)*0x22;
            const int column=static_cast<int>(actor)*0x50+5+
                             static_cast<int>(slot%4U)*0x12;
            const auto first=PartyPanelControlModel::
                modal_ordinary_slot_at(row,column);
            const auto last=PartyPanelControlModel::
                modal_ordinary_slot_at(row+31,column+15);
            assert(first && last);
            assert(first->actor==actor && last->actor==actor);
            assert(first->slot==slot && last->slot==slot);
            assert(!PartyPanelControlModel::
                   modal_ordinary_slot_at(row+32,column));
            assert(!PartyPanelControlModel::
                   modal_ordinary_slot_at(row,column+16));
        }
    }
    assert(!PartyPanelControlModel::
           modal_ordinary_slot_at(0x128,5));
    assert(!PartyPanelControlModel::
           modal_ordinary_slot_at(0x18D,5));
    assert(!PartyPanelControlModel::
           modal_ordinary_slot_at(0x129,4));
    assert(!PartyPanelControlModel::
           modal_ordinary_slot_at(0x129,0x4B));
    assert(!PartyPanelControlModel::
           modal_ordinary_slot_at(0x129,320));
}
