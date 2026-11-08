#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "rloft/party_presentation.hpp"

namespace rloft {

struct PartyHealthBar final {
    std::int32_t effective_hp{};
    std::int16_t max_hp{};
    std::int32_t scaled_height{};
    std::int32_t y{};
    int x{};
    std::int32_t rows{};
    int width{4};
    std::uint8_t color{};

    [[nodiscard]] bool drawable() const noexcept {
        return rows > 0;
    }
};

struct PartyHudPanel final {
    std::size_t actor_index{};
    int anchor_x{};

    bool inactive{};
    int inactive_resource_id{};
    int inactive_y{0x113};
    int inactive_x{};

    int panel_resource_id{};
    int panel_y{0x136};
    int panel_x{};

    int portrait_resource_id{};
    int portrait_state{};
    int portrait_y{0x138};
    int portrait_x{};

    PartyHealthBar health_bar{};
    bool condition_outline{};
    bool status_outline{};
};

class PartyHudModel final {
public:
    [[nodiscard]] static PartyHudPanel panel(
        const PartyActorPresentation& actor,
        std::uint16_t res0_raster_base,
        std::uint16_t res1_portrait_base,
        int presentation_mode = 0);

    [[nodiscard]] static std::array<PartyHudPanel, 4> panels(
        const std::array<PartyActorPresentation, 4>& actors,
        std::uint16_t res0_raster_base,
        std::uint16_t res1_portrait_base,
        int presentation_mode = 0);

    [[nodiscard]] static int panel_state(
        std::uint16_t status_word) noexcept;

    [[nodiscard]] static int anchor_x(
        std::size_t actor_index);
};

}  // namespace rloft
