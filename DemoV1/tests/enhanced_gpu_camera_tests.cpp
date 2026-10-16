#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/enhanced_gpu_camera.hpp"
#include <cassert>
#include <cmath>
int main() {
  constexpr double eps=1e-10;
  constexpr double angles[]{0.,90.,180.,270.};
  constexpr double dx[]{0.,-1.,0.,1.};
  constexpr double dy[]{1.,0.,-1.,0.};
  for(int i=0;i<4;++i) {
    const auto f=rloft::EnhancedGpuCameraModel::forward(angles[i]);
    const auto r=rloft::EnhancedGpuCameraModel::right(angles[i]);
    assert(std::abs(f.x-dx[i])<eps);
    assert(std::abs(f.y-dy[i])<eps);
    assert(std::abs(f.x*r.x+f.y*r.y)<eps);
    assert(std::abs(r.x*r.x+r.y*r.y-1.)<eps);
  }
  return 0;
}
