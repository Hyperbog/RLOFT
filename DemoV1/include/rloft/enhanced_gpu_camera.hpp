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
};
} // namespace rloft
