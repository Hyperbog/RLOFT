#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/enhanced_automap_keyboard.hpp"
#include "rloft/automap_modal.hpp"
#include <cassert>

int main() {
    using namespace rloft;
    const auto up=EnhancedAutomapKeyboard::action_for_key(0x26U,false);
    const auto down=EnhancedAutomapKeyboard::action_for_key(0x28U,false);
    const auto left=EnhancedAutomapKeyboard::action_for_key(0x25U,false);
    const auto right=EnhancedAutomapKeyboard::action_for_key(0x27U,false);
    assert(up && *up==0);
    assert(down && *down==1);
    assert(left && *left==2);
    assert(right && *right==3);
    for (auto vk:{0x26U,0x28U,0x25U,0x27U}) {
        const auto normal=EnhancedAutomapKeyboard::action_for_key(vk,false);
        const auto page=EnhancedAutomapKeyboard::action_for_key(vk,true);
        assert(normal && page && *page==*normal+12);
        assert(AutomapControlModel::navigation_mask(*normal)!=0U);
        assert(AutomapControlModel::navigation_mask(*page)!=0U);
    }
    // Non-navigation keys cannot accidentally activate the map's
    // Close, annotation, or other original mouse-only actions.
    for(auto vk:{0U,0x1BU,0x0DU,0x20U,0x41U,0x70U,0x10U}) {
        assert(!EnhancedAutomapKeyboard::action_for_key(vk,false));
        assert(!EnhancedAutomapKeyboard::action_for_key(vk,true));
    }
    // The same native model that handles the on-screen arrow buttons
    // also handles the new keyboard events and clamps at map bounds.
    ExplorationSessionState session{};
    session.coordinate_a=4U<<16U;
    session.coordinate_b=4U<<16U;
    AutomapModalController map(session);
    const auto before=map.state();
    assert(map.apply_action(*up,64,64)==AutomapModalResult::Redraw);
    assert(map.state().row==before.row-4);
    assert(map.apply_action(*right,64,64)==AutomapModalResult::Redraw);
    assert(map.state().column==before.column+4);
    assert(map.apply_action(
        *EnhancedAutomapKeyboard::action_for_key(0x28U,true),
        64,64)==AutomapModalResult::Redraw);
    assert(map.state().row>before.row);
    assert(map.apply_action(11,64,64)==AutomapModalResult::Close);
    return 0;
}
