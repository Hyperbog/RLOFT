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
  // The shipped original SINTAB focal C6CC=175 and center C6D0=127:
  // horizontal screen FOV ~=72.366, not the previously misused
  // 75-degree VERTICAL FOV. Physical pixel aspect makes the original
  // 256x256 scene viewport ~5:3 inside the preserved 4:3 game image.
  const auto h=rloft::EnhancedGpuCameraModel::vertical_fov_degrees(175,1.);
  const auto v=rloft::EnhancedGpuCameraModel::vertical_fov_degrees(175,5./3.);
  assert(std::abs(h-72.36560815488484)<0.01);
  assert(v>47. && v<48.);
  assert(v<h);
  bool rejected=false;
  try{(void)rloft::EnhancedGpuCameraModel::vertical_fov_degrees(0,1.);}
  catch(const std::invalid_argument&){rejected=true;}
  assert(rejected);
  // RES3 scenery sprites are 128 rows tall with independently decoded
  // widths. Do not stretch every optional GPU tree/NPC to one fixed width.
  using rloft::EnhancedGpuCameraModel;
  assert(EnhancedGpuCameraModel::billboard_world_width(1U)==2.0/128.0);
  assert(EnhancedGpuCameraModel::billboard_world_width(32U)==0.5);
  assert(EnhancedGpuCameraModel::billboard_world_width(64U)==1.0);
  assert(EnhancedGpuCameraModel::billboard_world_width(128U)==2.0);
  for(unsigned width:{0U,129U,999U}) {
    bool invalid=false;
    try{(void)EnhancedGpuCameraModel::billboard_world_width(width);}
    catch(const std::invalid_argument&){invalid=true;}
    assert(invalid);
  }
  return 0;
}
