#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "rloft/view_sweep_record.hpp"

namespace rloft {

enum class SweepRenderPass {
    Prepass,
    Main,
};

struct ViewSweepRun final {
    SweepRenderPass pass{SweepRenderPass::Main};
    std::size_t first_ray{};
    std::size_t ray_count{};

    std::uint32_t depth{};
    std::uint32_t resource_id{};
    std::uint32_t texture_column_offset{};
    std::uint32_t texture_source_byte_offset{};
    std::uint32_t texture_column_index{};
    std::uint32_t shade_decrement{};

    [[nodiscard]] bool drawable() const noexcept {
        return resource_id != 0x93U;
    }
};

class ViewSweepRunModel final {
public:
    // CODE.2 335DC. The first pass includes only records whose +0C prepass
    // field equals 1. The second pass includes all records. Runs are broken
    // only when depth or texture-column selector changes, matching the
    // executable's assumptions about resource continuity.
    [[nodiscard]] static std::vector<ViewSweepRun> build(
        const std::vector<ViewSweepRecord>& records);
};

}  // namespace rloft
