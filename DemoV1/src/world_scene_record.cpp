#include "rloft/world_scene_record.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

namespace rloft {

void WorldSceneOrderModel::assign_render_order(
    std::vector<WorldSceneRecord>& records) noexcept {
    std::vector<bool> selected(
        records.size(), false);

    for (std::size_t rank = 0;
         rank < records.size();
         ++rank) {
        std::size_t candidate =
            records.size();
        std::uint16_t smallest =
            std::numeric_limits<std::uint16_t>::max();

        for (std::size_t i = 0;
             i < records.size();
             ++i) {
            if (selected[i]) {
                continue;
            }
            if (candidate == records.size() ||
                records[i].projected_extent <
                    smallest) {
                candidate = i;
                smallest =
                    records[i].projected_extent;
            }
        }

        if (candidate == records.size()) {
            return;
        }
        selected[candidate] = true;
        records[candidate].render_order =
            static_cast<std::uint16_t>(rank);
    }
}

}  // namespace rloft
