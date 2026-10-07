#include "rloft/party_hud.hpp"

#include <cassert>

int main() {
    assert(rloft::PartyHudModel::anchor_x(0) == 0);
    assert(rloft::PartyHudModel::anchor_x(1) == 0x35);
    assert(rloft::PartyHudModel::anchor_x(2) == 0xD3);
    assert(rloft::PartyHudModel::anchor_x(3) == 0x108);

    assert(rloft::PartyHudModel::panel_state(0) == 2);
    assert(rloft::PartyHudModel::panel_state(0x0040) == 1);
    assert(rloft::PartyHudModel::panel_state(0x0400) == 5);
    assert(rloft::PartyHudModel::panel_state(0x0800) == 3);
    assert(rloft::PartyHudModel::panel_state(0x1000) == 4);
    assert(rloft::PartyHudModel::panel_state(0x4000) == 0);
    assert(rloft::PartyHudModel::panel_state(0x5000) == 0);

    rloft::PartyActorPresentation actor{};
    actor.actor_index = 1;
    actor.status_word = 0;
    actor.active = true;
    actor.portrait_index = 2;
    actor.sex_code = 1;
    actor.current_hp = 10;
    actor.max_hp = 20;
    actor.hp_modifier_113 = 1;
    actor.hp_modifier_1ea = 1;
    actor.condition_effect_flags = 4;

    auto panel = rloft::PartyHudModel::panel(
        actor, 15, 336, 0);

    assert(panel.anchor_x == 0x35);
    assert(!panel.inactive);
    assert(panel.inactive_resource_id == 31);
    assert(panel.inactive_y == 0x113);
    assert(panel.inactive_x == 0x37);
    assert(panel.panel_resource_id == 51);
    assert(panel.panel_y == 0x136);
    assert(panel.panel_x == 0x36);
    assert(panel.portrait_resource_id == 361);
    assert(panel.portrait_state == 2);
    assert(panel.portrait_y == 0x138);
    assert(panel.portrait_x == 0x39);
    assert(panel.condition_outline);
    assert(!panel.status_outline);

    assert(panel.health_bar.effective_hp == 12);
    assert(panel.health_bar.scaled_height == 48);
    assert(panel.health_bar.y == 0x158);
    assert(panel.health_bar.x == 0x65);
    assert(panel.health_bar.rows == 48);
    assert(panel.health_bar.color == 0x43);

    actor.current_hp = 2;
    actor.hp_modifier_113 = 0;
    actor.hp_modifier_1ea = 0;
    panel = rloft::PartyHudModel::panel(
        actor, 15, 336, 0);
    assert(panel.health_bar.scaled_height == 8);
    assert(panel.health_bar.y == 0x180);
    assert(panel.health_bar.color == 0x57);

    actor.current_hp = 30;
    panel = rloft::PartyHudModel::panel(
        actor, 15, 336, 0);
    assert(panel.health_bar.scaled_height == 120);
    assert(panel.health_bar.y == 0x138);
    assert(panel.health_bar.rows == 0x50);
    assert(panel.health_bar.color == 0x1B);

    actor.status_word = 0x8000;
    panel = rloft::PartyHudModel::panel(
        actor, 15, 336, 0);
    assert(panel.inactive);
    assert(!panel.health_bar.drawable());

    actor.status_word = 0x1200;
    actor.current_hp = 10;
    panel = rloft::PartyHudModel::panel(
        actor, 15, 336, 1);
    assert(panel.panel_resource_id == 52);
    assert(panel.inactive_resource_id == 32);
    assert(panel.portrait_state == 4);
    assert(panel.status_outline);
}
