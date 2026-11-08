// Independent original CODE.2 32-scene resource palette selector.
// This adapter performs validation only; never mutates guest game state.
#include "Resources.hpp"
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft { namespace original_scene_palette {
unsigned select_bank(const std::vector<std::uint8_t>& original_dgroup,
                            unsigned level_id) {
    if (level_id >= kOriginalSceneCount)
        throw std::out_of_range("original CODE.2 scene index is not 0..31");
    const auto offset = kSceneTableOffset +
                        std::size_t(level_id)*kSceneDescriptorStride;
    if (offset >= original_dgroup.size())
        throw std::out_of_range("original CODE.2 scene palette table missing");
    const unsigned bank = original_dgroup[offset];
    if (bank >= kOriginalColorsBanks)
        throw std::runtime_error("original CODE.2 scene selects an unavailable COLORS bank");
    return bank;
}
}} // namespace rloft::original_scene_palette
