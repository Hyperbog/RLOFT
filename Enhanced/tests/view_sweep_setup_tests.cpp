#include "rloft/view_sweep_setup.hpp"
#include <cassert>
int main(){
 rloft::RendererTables t{};
 for(std::size_t i=0;i<t.sine_q16_signmag.size();++i)t.sine_q16_signmag[i]=0;
 for(std::size_t i=0;i<t.tangent_q16.size();++i)t.tangent_q16[i]=static_cast<std::uint32_t>(i+1);
 t.constants[5]=22272;
 t.view_angle_tenths.fill(0);
 t.view_angle_tenths[0]=359;t.view_angle_tenths[1]=0;t.view_angle_tenths[2]=-359;
 rloft::ExplorationSessionState s{};s.coordinate_a=0x00048000U;s.coordinate_b=0x00048000U;s.facing=0;
 const auto b=rloft::ViewSweepSetupModel::camera_basis(s,t);
 assert(b.player_a_cell==4&&b.player_b_cell==4);
 assert(b.origin_a==s.coordinate_a&&b.origin_b==s.coordinate_b);
 auto r=rloft::ViewSweepSetupModel::ray(s,t,b,0);
 assert(r.quadrant==0&&r.angle_tenths==359);assert(r.step_a==0x10000U&&r.step_b==0xFFFF0000U);
 assert(!r.left_compare_ge&&r.right_compare_ge);assert(r.tangent_a==541&&r.tangent_b==static_cast<std::uint32_t>(0U-360U));
 r=rloft::ViewSweepSetupModel::ray(s,t,b,1);assert(r.quadrant==0&&r.angle_tenths==0&&r.tangent_a==900&&r.tangent_b==0xFFFFFFFFU);
 r=rloft::ViewSweepSetupModel::ray(s,t,b,2);assert(r.quadrant==3&&r.angle_tenths==3241);assert(r.step_a==0x10000U&&r.step_b==0x10000U);
 // Cardinal facings select the same original sine-table basis offsets.
 s.facing=270;t.sine_q16_signmag[270]=0x8000FFFFU;t.sine_q16_signmag[360]=0x80000000U;
 const auto b270=rloft::ViewSweepSetupModel::camera_basis(s,t);
 assert(b270.sine_forward==0x8000FFFFU&&b270.sine_quarter==0x80000000U);
}
