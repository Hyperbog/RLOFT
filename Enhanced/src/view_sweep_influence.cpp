#include "rloft/view_sweep_influence.hpp"

#include <array>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft {
namespace {

bool offset_matches(
    std::uint32_t record_cell,
    std::uint32_t source_cell,
    std::int32_t offset) noexcept {
    return static_cast<std::int64_t>(
               record_cell) ==
           static_cast<std::int64_t>(
               source_cell) +
               static_cast<std::int64_t>(
                   offset);
}

}  // namespace

void ViewSweepInfluenceModel::apply(
    std::vector<ViewSweepRecord>& records,
    std::uint8_t step,
    std::uint32_t source_cell_index,
    int width) {
    if (step == 0U) {
        return;
    }
    if (width <= 0) {
        throw std::invalid_argument(
            "scene influence requires positive level width");
    }

    const auto w =
        static_cast<std::int32_t>(width);
    const std::array<std::int32_t, 8>
        ring1{{
            -1, 1,
            -w, w,
            w - 1, w + 1,
            -w - 1, -w + 1,
        }};
    const std::array<std::int32_t, 16>
        ring2{{
            -2, 2,
            -2 * w, 2 * w,
            2 * w - 2, 2 * w + 2,
            -2 * w - 2, -2 * w + 2,
            -2 * w - 1, -2 * w + 1,
            2 * w - 1, 2 * w + 1,
            w - 2, w + 2,
            -w - 2, -w + 2,
        }};

    std::uint32_t previous_cell =
        0xFFFFFFFFU;
    std::uint32_t carried_value = 0U;

    for (auto& record : records) {
        if (record.cell_index !=
            previous_cell) {
            previous_cell =
                record.cell_index;
            carried_value =
                record.shade_decrement;

            if (record.cell_index ==
                source_cell_index) {
                carried_value += step;
            } else if (step > 1U) {
                for (const auto offset :
                     ring1) {
                    if (offset_matches(
                            record.cell_index,
                            source_cell_index,
                            offset)) {
                        carried_value +=
                            static_cast<std::uint32_t>(
                                step - 1U);
                        break;
                    }
                }
            }

            if (step == 3U) {
                for (const auto offset :
                     ring2) {
                    if (offset_matches(
                            record.cell_index,
                            source_cell_index,
                            offset)) {
                        ++carried_value;
                        break;
                    }
                }
            }
        }

        // 2B16F writes the carried dword on every iteration, including
        // consecutive records whose cell index matches the prior record.
        record.shade_decrement =
            carried_value;
    }
}

}  // namespace rloft
