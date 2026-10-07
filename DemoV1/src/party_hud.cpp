#include "rloft/party_hud.hpp"

#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {

std::int32_t rounded_health_height(
    std::int32_t effective_hp,
    std::int16_t max_hp) {
    if (max_hp == 0) {
        throw std::runtime_error(
            "party HUD actor has zero maximum HP");
    }

    const auto product =
        static_cast<std::uint32_t>(effective_hp) * 0x50U;
    const auto half =
        static_cast<std::int32_t>(max_hp) >> 1;
    const auto numerator =
        static_cast<std::int32_t>(
            product +
            static_cast<std::uint32_t>(half));

    if (numerator ==
            static_cast<std::int32_t>(0x80000000U) &&
        max_hp == -1) {
        throw std::runtime_error(
            "party HUD HP division overflow");
    }
    return numerator /
           static_cast<std::int32_t>(max_hp);
}

}  // namespace

int PartyHudModel::anchor_x(
    std::size_t actor_index) {
    if (actor_index >= 4) {
        throw std::out_of_range(
            "party HUD actor index");
    }
    return static_cast<int>(
        (actor_index % 2U) * 0x35U +
        (actor_index / 2U) * 0xD3U);
}

int PartyHudModel::panel_state(
    std::uint16_t status_word) noexcept {
    if ((status_word & 0x4000U) != 0U) return 0;
    if ((status_word & 0x1000U) != 0U) return 4;
    if ((status_word & 0x0040U) != 0U) return 1;
    if ((status_word & 0x0800U) != 0U) return 3;
    if ((status_word & 0x0400U) != 0U) return 5;
    return 2;
}

PartyHudPanel PartyHudModel::panel(
    const PartyActorPresentation& actor,
    std::uint16_t res0_raster_base,
    std::uint16_t res1_portrait_base,
    int presentation_mode) {
    if (actor.actor_index >= 4) {
        throw std::out_of_range(
            "party HUD actor index");
    }
    if (presentation_mode < 0) {
        throw std::out_of_range(
            "negative party HUD presentation mode");
    }

    PartyHudPanel out{};
    out.actor_index = actor.actor_index;
    out.anchor_x = anchor_x(actor.actor_index);
    out.inactive =
        (actor.status_word & 0x8000U) != 0U;

    out.inactive_resource_id =
        static_cast<int>(res0_raster_base) +
        0x10 + presentation_mode;
    out.inactive_x = out.anchor_x + 2;

    out.panel_resource_id =
        static_cast<int>(res0_raster_base) +
        0x24 + presentation_mode;
    out.panel_x = out.anchor_x + 1;

    out.portrait_resource_id =
        static_cast<int>(res1_portrait_base) +
        actor.portrait_local_index();
    out.portrait_state =
        panel_state(actor.status_word);
    out.portrait_x = out.anchor_x + 4;

    out.condition_outline =
        actor.condition_effect_flags != 0U;
    out.status_outline =
        (actor.status_word & 0x1E00U) != 0U;

    if (!out.inactive &&
        actor.status_word < 0x4000U) {
        const auto effective =
            actor.effective_hp();
        const auto bar =
            rounded_health_height(
                effective,
                actor.max_hp);

        out.health_bar.effective_hp = effective;
        out.health_bar.max_hp = actor.max_hp;
        out.health_bar.scaled_height = bar;
        out.health_bar.x =
            out.anchor_x + 0x30;

        if (bar > 0x50) {
            out.health_bar.y = 0x138;
            out.health_bar.rows = 0x50;
            out.health_bar.color = 0x1B;
        } else {
            out.health_bar.y = 0x188 - bar;
            out.health_bar.rows = bar;
            out.health_bar.color =
                bar > 0x14 ? 0x43 : 0x57;
        }
    }

    return out;
}

std::array<PartyHudPanel, 4> PartyHudModel::panels(
    const std::array<PartyActorPresentation, 4>& actors,
    std::uint16_t res0_raster_base,
    std::uint16_t res1_portrait_base,
    int presentation_mode) {
    std::array<PartyHudPanel, 4> out{};
    for (std::size_t i = 0; i < out.size(); ++i) {
        out[i] = panel(
            actors[i],
            res0_raster_base,
            res1_portrait_base,
            presentation_mode);
    }
    return out;
}

}  // namespace rloft
