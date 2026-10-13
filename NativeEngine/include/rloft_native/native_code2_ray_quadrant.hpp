#pragma once

#include <cstdint>

namespace rloft_native {

// Original CODE.2 32954, x86 object 1: bounded quadrant setup
// 32A19..32B73. The input is the ORIGINAL tenth-degree ray angle
// after original c73c[]/c714 composition, normalized to 0..3599.
// The host UI's radians->ray-angle mapping is a separate approximation.
struct NativeCode2Quadrant32954 {
    unsigned source_tenths{};
    unsigned original_quadrant{};
    std::uint32_t original_step_x_25610{};
    std::uint32_t original_step_y_25604{};
    bool x_path_reflect_32f8c{};
    bool y_path_reflect_33124{};
    unsigned original_x_wall_first_bank{3};
    unsigned original_x_wall_second_bank{2};
    unsigned original_y_wall_first_bank{1};
    unsigned original_y_wall_second_bank{0};
    bool original_live_angle_source_verified{false};
    bool original_complete_raycast_verified{false};
};

// Exact original quadrant boundary and signed-state setup. This does not
// synthesize or certify the original DOS ray angle, screen x, DDA or wall
// word-selection scheduler.
[[nodiscard]] NativeCode2Quadrant32954
quadrant_native_code2_32954(unsigned original_angle_tenths);

} // namespace rloft_native
