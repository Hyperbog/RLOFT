#include "rloft_native/native_code2_ray_quadrant.hpp"
#include <cassert>
#include <stdexcept>
using namespace rloft_native;
int main() {
    constexpr unsigned positive=0x00010000U,negative=0xffff0000U;
    // Independently check the source x86 threshold constants 384h,
    // 708h, A8Ch and E10h; no original files are required.
    for(unsigned angle=0;angle<3600U;++angle) {
        const auto q=quadrant_native_code2_32954(angle);
        const unsigned expected=angle<900U?0U:angle<1800U?1U:
                                angle<2700U?2U:3U;
        assert(q.original_quadrant==expected);
        assert(q.original_step_x_25610==
               ((expected==0U || expected==3U)?positive:negative));
        assert(q.original_step_y_25604==
               ((expected<=1U)?negative:positive));
        assert(q.x_path_reflect_32f8c==(expected==1U || expected==2U));
        assert(q.y_path_reflect_33124==(expected==2U || expected==3U));
        assert(q.original_x_wall_first_bank==3U &&
               q.original_x_wall_second_bank==2U &&
               q.original_y_wall_first_bank==1U &&
               q.original_y_wall_second_bank==0U);
        assert(!q.original_live_angle_source_verified);
        assert(!q.original_complete_raycast_verified);
    }
    for(auto invalid:{3600U,3601U,65535U}) {
        bool caught=false;
        try {static_cast<void>(quadrant_native_code2_32954(invalid));}
        catch(const std::invalid_argument&) {caught=true;}
        assert(caught);
    }
}
