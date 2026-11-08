#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "rloft/game_source.hpp"

namespace rloft {

struct PlacedSceneDescriptor final {
    std::uint16_t local_shape_base{};
    std::uint16_t spatial_extent{};
    std::uint8_t shape_stride{};
    std::uint8_t variant_count{};
    std::uint8_t scene_influence_step{};
};

class PlacedSceneCatalog final {
public:
    [[nodiscard]] static PlacedSceneCatalog parse_init(
        const std::vector<std::uint8_t>& init_bytes);
    [[nodiscard]] static PlacedSceneCatalog load(
        const GameSource& source);

    [[nodiscard]] std::size_t size() const noexcept {
        return descriptors_.size();
    }
    [[nodiscard]] const PlacedSceneDescriptor& at(
        std::size_t type_id) const;

private:
    std::vector<PlacedSceneDescriptor> descriptors_;
};

}  // namespace rloft
