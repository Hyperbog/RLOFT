#pragma once

#include <cstdint>
#include <vector>

#include "rloft/view_sweep_record.hpp"

namespace rloft {

class ViewSweepInfluenceModel final {
public:
    // CODE.2 2B088. Mutates the per-ray shade/influence dword in place for
    // one emitted scene source. width is the live level width used to build
    // the original signed neighbor-offset tables at 170AC.
    static void apply(
        std::vector<ViewSweepRecord>& records,
        std::uint8_t step,
        std::uint32_t source_cell_index,
        int width);
};

}  // namespace rloft
