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
};
} // namespace rloft
