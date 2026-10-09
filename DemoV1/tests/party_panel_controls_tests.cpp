#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/party_panel_controls.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

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

    // CODE.2 12E70 exactly routes 0x10..0x27 row and
    // actor-local 0x44..0x4D to original 12988 MAX/CURRENT/
    // encumbrance. The 0x2B..0x40 icons open *other* modals.
    for(int actor=0;actor<4;++actor) {
        const int base=actor*0x50;
        assert(PartyPanelControlModel::modal_encumbrance_actor_at(
            0x10,base+0x44)==static_cast<std::size_t>(actor));
        assert(PartyPanelControlModel::modal_encumbrance_actor_at(
            0x27,base+0x4D)==static_cast<std::size_t>(actor));
        assert(!PartyPanelControlModel::modal_encumbrance_actor_at(
            0x0F,base+0x44));
        assert(!PartyPanelControlModel::modal_encumbrance_actor_at(
            0x28,base+0x44));
        assert(!PartyPanelControlModel::modal_encumbrance_actor_at(
            0x2B,base+0x39));
        assert(!PartyPanelControlModel::modal_encumbrance_actor_at(
            0x10,base+0x43));
    }
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

    // Original CODE.2 11C3C uses the LIVE ten-byte descriptor's
    // exact x-offset and columns for hand-open containers.
    // 4x2 chest/quiver, 3x2 scroll case, 2x2 pouch.
    for(const std::size_t actor:{0U,1U,2U,3U}) {
        for(const auto cols:{2U,3U,4U}) {
            const auto size=cols*2U;
            const int xoffset=cols==4U?5:14;
            for(std::size_t slot=0;slot<size;++slot) {
                const int row=0xDB+static_cast<int>(slot/cols)*0x22;
                const int col=static_cast<int>(actor)*80+xoffset+
                    static_cast<int>(slot%cols)*0x12;
                const auto first=PartyPanelControlModel::
                    modal_container_slot_at(
                        row,col,actor,static_cast<std::uint8_t>(cols),
                        static_cast<std::uint8_t>(xoffset),size);
                const auto last=PartyPanelControlModel::
                    modal_container_slot_at(
                        row+31,col+15,actor,static_cast<std::uint8_t>(cols),
                        static_cast<std::uint8_t>(xoffset),size);
                assert(first && last);
                assert(first->actor==actor && first->slot==slot);
                assert(last->actor==actor && last->slot==slot);
                assert(!PartyPanelControlModel::modal_container_slot_at(
                    row+32,col,actor,static_cast<std::uint8_t>(cols),
                    static_cast<std::uint8_t>(xoffset),size));
                assert(!PartyPanelControlModel::modal_container_slot_at(
                    row,col+16,actor,static_cast<std::uint8_t>(cols),
                    static_cast<std::uint8_t>(xoffset),size));
            }
        }
    }
    assert(!PartyPanelControlModel::modal_container_slot_at(
        0xDB,5,0,0U,5U,8U));
    assert(!PartyPanelControlModel::modal_container_slot_at(
        0xDA,5,0,4U,5U,8U));
    assert(!PartyPanelControlModel::modal_container_slot_at(
        0xDB,5,0,4U,5U,9U));

    // Original CODE.2 229CC actor hand-region table at D684, with
    // inclusive boundaries and deliberately separate spell lanes.
    const int min_columns[4][2] = {
        {4,37},{57,90},{215,248},{268,301}
    };
    for(std::size_t actor=0U; actor<4U; ++actor) {
        for(std::size_t hand=0U; hand<2U; ++hand) {
            const int x=min_columns[actor][hand];
            const auto first=PartyPanelControlModel::gameplay_hand_at(
                276,x);
            const auto last=PartyPanelControlModel::gameplay_hand_at(
                307,x+15);
            assert(first && last);
            assert(first->actor==actor && first->hand==hand);
            assert(last->actor==actor && last->hand==hand);
            assert(!PartyPanelControlModel::gameplay_hand_at(
                275,x));
            assert(!PartyPanelControlModel::gameplay_hand_at(
                308,x));
            assert(!PartyPanelControlModel::gameplay_hand_at(
                276,x+16));
        }
    }
    assert(!PartyPanelControlModel::gameplay_hand_at(276,21));
    assert(!PartyPanelControlModel::gameplay_hand_at(315,4));
    assert(!PartyPanelControlModel::gameplay_hand_at(300,320));

    // Original CODE.2 12E70 stores the address of one of these two
    // EXACT 10x24-byte tables into live DGROUP:12C00. Original
    // physical CODE.2 data offsets are 75F24+6DB0 (race other than
    // 2/3/4), 75F24+6EA0 (race 2/3/4).
    constexpr int encoded[2][10][6]={{
        {17,1,128,30,153,49},
        {18,1,154,34,170,44},
        {16,1,154,27,291,52},
        {19,1,130,17,163,31},
        {20,1,178,16,195,27},
        {20,1,178,52,195,63},
        {12,1,248,3,279,18},
        {13,1,248,61,279,76},
        {14,1,110,3,291,26},
        {15,1,110,53,291,76}
    },{
        {17,1,146,30,171,49},
        {18,1,172,34,188,44},
        {16,1,172,27,291,52},
        {19,1,148,17,181,31},
        {20,1,196,16,213,27},
        {20,1,196,52,213,63},
        {12,1,248,3,279,18},
        {13,1,248,61,279,76},
        {14,1,128,3,291,26},
        {15,1,128,53,291,76}
    }};
    constexpr std::size_t base=0x75F24U;
    constexpr std::size_t position[2]{0x6DB0U,0x6EA0U};
    std::vector<std::uint8_t> original_code2(
        base+0x6EA0U+10U*24U,0U);
    const auto dword=[&](std::size_t at,std::uint32_t value) {
        for(std::size_t b=0U;b<4U;++b) {
            original_code2.at(at+b)=
                static_cast<std::uint8_t>(value>>(8U*b));
        }
    };
    for(std::size_t table=0U;table<2U;++table) {
        for(std::size_t i=0U;i<10U;++i) {
            for(std::size_t field=0U;field<6U;++field) {
                dword(base+position[table]+i*24U+field*4U,
                      static_cast<std::uint32_t>(
                          encoded[table][i][field]));
            }
        }
    }
    const auto hit_tables=rloft::OriginalEquipmentHitTables::
        from_code2(original_code2);
    for(std::size_t table=0U;table<2U;++table) {
        for(std::size_t i=0U;i<10U;++i) {
            const auto& d=hit_tables.descriptor(table!=0U,i);
            assert(d.slot==encoded[table][i][0]);
            assert(d.accepted_pointer_flags==1U);
            assert(d.rect.row_min==encoded[table][i][2]);
            assert(d.rect.column_min==encoded[table][i][3]);
            assert(d.rect.row_max==encoded[table][i][4]);
            assert(d.rect.column_max==encoded[table][i][5]);
            std::array<std::int16_t,21U> items{};
            items.fill(-1);
            const auto slot=static_cast<std::size_t>(d.slot);
            // Empty descriptor boxes do NOT pick up phantom items.
            assert(!hit_tables.occupied_hit(
                table!=0U,d.rect.row_min,d.rect.column_min,
                1U,items));
            items[slot]=5;
            const auto low=hit_tables.occupied_hit(
                table!=0U,d.rect.row_min,d.rect.column_min,
                1U,items);
            const auto high=hit_tables.occupied_hit(
                table!=0U,d.rect.row_max,d.rect.column_max,
                1U,items);
            assert(low && *low==slot);
            assert(high && *high==slot);
            // All ten shipped descriptors accept original left-click
            // flags=1, not flag2 or a synthesized key state.
            assert(!hit_tables.occupied_hit(
                table!=0U,d.rect.row_min,d.rect.column_min,
                2U,items));
            assert(!hit_tables.occupied_hit(
                table!=0U,d.rect.row_min-1,d.rect.column_min,
                1U,items));
            assert(!hit_tables.occupied_hit(
                table!=0U,d.rect.row_min,d.rect.column_min-1,
                1U,items));
        }
    }
    // The original race variant is meaningful: head/hand/armor
    // rows shift down by 18 pixels, while rings remain unchanged.
    assert(hit_tables.descriptor(false,0U).rect.row_min==128);
    assert(hit_tables.descriptor(true,0U).rect.row_min==146);
    assert(hit_tables.descriptor(false,6U).rect.row_min==248);
    assert(hit_tables.descriptor(true,6U).rect.row_min==248);
    bool invalid=false;
    try {
        (void)hit_tables.descriptor(false,10U);
    }catch(const std::out_of_range&){invalid=true;}
    assert(invalid);
    original_code2.pop_back();
    invalid=false;
    try {
        (void)rloft::OriginalEquipmentHitTables::from_code2(
            original_code2);
    }catch(const std::runtime_error&){invalid=true;}
    assert(invalid);
}
