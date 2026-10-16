#pragma once
#include <cmath>
#include <stdexcept>
namespace rloft {
// Original CODE.2 angle convention (exploration_motion): heading 0 = +row,
// heading 90 = -column; unlike OpenGL's usual 0 = +X.
struct EnhancedGpuDirection final { double x{},y{}; };
class EnhancedGpuCameraModel final {
public:
    [[nodiscard]] static EnhancedGpuDirection forward(double degrees) {
        if(!std::isfinite(degrees))throw std::invalid_argument("nonfinite camera heading");
        constexpr double rad=3.14159265358979323846/180.0;
        return {-std::sin(degrees*rad),std::cos(degrees*rad)};
    }
    [[nodiscard]] static EnhancedGpuDirection right(double degrees) {
        const auto f=forward(degrees);
        return {f.y,-f.x};
    }
    // CODE.2 35834 scales lateral/depth by original SINTAB C6CC,
    // with screen center C6D0=127 and 256 wide source viewport.
    // GLU uses *vertical* FOV while source projection is horizontal.
    [[nodiscard]] static double vertical_fov_degrees(
        unsigned source_focal,double physical_aspect) {
        if(source_focal==0U || !std::isfinite(physical_aspect) ||
           physical_aspect<=0.0)
            throw std::invalid_argument("invalid original 3D FOV");
        constexpr double rad=180.0/3.14159265358979323846;
        return 2.0*std::atan(
            128.0/static_cast<double>(source_focal)/physical_aspect)*rad;
    }
    // Optional GPU sprites retain the 128-row RES3 artwork's native width
    // proportion; the source software raster and game state are unchanged.
    [[nodiscard]] static double billboard_world_width(unsigned native_width) {
        if(native_width==0U || native_width>128U)
            throw std::invalid_argument("invalid native RES3 sprite width");
        return 2.0*static_cast<double>(native_width)/128.0;
    }
};
} // namespace rloft
