#include "rloft_native/native_code2_ray_quadrant.hpp"
#include <stdexcept>

namespace rloft_native {
NativeCode2Quadrant32954
quadrant_native_code2_32954(unsigned original_angle_tenths) {
    if(original_angle_tenths>=3600U)
        throw std::invalid_argument("CODE.2 ray angle outside original 0..3599 domain");
    NativeCode2Quadrant32954 r{};
    r.source_tenths=original_angle_tenths;
    r.original_quadrant=original_angle_tenths/900U;
    // Source 32A19, 32A6F, 32ACD and 32B26 explicitly set the
    // original x/y signed 16.16 stepping constants at 25610/25604.
    constexpr std::uint32_t positive=0x00010000U;
    constexpr std::uint32_t negative=0xffff0000U;
    switch(r.original_quadrant) {
        case 0: r.original_step_x_25610=positive;
                r.original_step_y_25604=negative;break;
        case 1: r.original_step_x_25610=negative;
                r.original_step_y_25604=negative;break;
        case 2: r.original_step_x_25610=negative;
                r.original_step_y_25604=positive;break;
        case 3: r.original_step_x_25610=positive;
                r.original_step_y_25604=positive;break;
        default:throw std::logic_error("impossible CODE.2 ray quadrant");
    }
    // Original 32FA5 reflects when x 25610 == -0x10000.
    // Original 3313D reflects when y 25604 != -0x10000.
    r.x_path_reflect_32f8c=r.original_step_x_25610==negative;
    r.y_path_reflect_33124=r.original_step_y_25604!=negative;
    return r;
}
} // namespace rloft_native
