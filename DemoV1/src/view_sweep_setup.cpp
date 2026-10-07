#include "rloft/view_sweep_setup.hpp"
#include "rloft/fixed_projection.hpp"
#include <cstddef>
#include <cstdint>
#include <stdexcept>
namespace rloft { namespace {
std::int16_t high_s16(std::uint32_t v) noexcept{return static_cast<std::int16_t>(v>>16U);}
std::uint16_t low_u16(std::uint32_t v) noexcept{return static_cast<std::uint16_t>(v);}
std::uint16_t neg_low(std::uint16_t v) noexcept{return static_cast<std::uint16_t>(0U-static_cast<std::uint32_t>(v));}
std::int32_t normalize_tenths(std::int32_t a) noexcept{
 if(a<0)a=static_cast<std::int32_t>(static_cast<std::uint32_t>(a)+0xE10U);
 else if(a>=0xE10)a=static_cast<std::int32_t>(static_cast<std::uint32_t>(a)-0xE10U);
 return a;
}
}
ViewCameraBasis ViewSweepSetupModel::camera_basis(const ExplorationSessionState& s,const RendererTables& t){
 if(s.facing>=360U)throw std::out_of_range("view facing");
 const auto view=static_cast<std::size_t>(s.facing);
 if(view+90U>=t.sine_q16_signmag.size())throw std::out_of_range("view sine index");
 ViewCameraBasis out{};
 out.sine_forward=t.sine_q16_signmag[view];
 out.sine_quarter=t.sine_q16_signmag[view+90U];
 const auto vertical=FixedProjection::multiply_signed_factor(t.grid_projection_constant(),out.sine_quarter);
 out.origin_b=s.coordinate_b-vertical;
 const auto horizontal=FixedProjection::multiply_signed_factor(t.grid_projection_constant(),out.sine_forward);
 out.origin_a=s.coordinate_a+horizontal;
 out.origin_b_fraction=low_u16(out.origin_b);
 out.origin_b_fraction_neg=neg_low(out.origin_b_fraction);
 out.origin_a_fraction=low_u16(out.origin_a);
 out.origin_a_fraction_neg=neg_low(out.origin_a_fraction);
 out.origin_b_cell=high_s16(out.origin_b);
 out.origin_a_cell=high_s16(out.origin_a);
 out.player_b_cell=high_s16(s.coordinate_b);
 out.player_a_cell=high_s16(s.coordinate_a);
 return out;
}
ViewRaySetup ViewSweepSetupModel::ray(const ExplorationSessionState& s,const RendererTables& t,const ViewCameraBasis& b,int ray_index){
 if(ray_index<0||ray_index>=static_cast<int>(t.view_angle_tenths.size()))throw std::out_of_range("view ray index");
 std::int32_t angle=normalize_tenths(t.view_angle_tenths[static_cast<std::size_t>(ray_index)]+static_cast<std::int32_t>(s.facing*10U));
 ViewRaySetup out{};out.ray_index=ray_index;out.angle_tenths=angle;
 if(angle<0x384){
  out.quadrant=0;out.step_a=0x00010000U;out.step_b=0xFFFF0000U;out.left_compare_ge=false;out.right_compare_ge=true;
  out.tangent_a=t.tangent_q16[static_cast<std::size_t>(0x383-angle)];
  out.tangent_b=0U-t.tangent_q16[static_cast<std::size_t>(angle)];
  out.fractional_arg_a=b.origin_a_fraction;out.fractional_arg_b=b.origin_b_fraction_neg;
 }else if(angle<0x708){
  out.quadrant=1;out.step_a=0xFFFF0000U;out.step_b=0xFFFF0000U;out.left_compare_ge=false;out.right_compare_ge=false;
  out.tangent_a=0U-t.tangent_q16[static_cast<std::size_t>(angle-0x384)];
  out.tangent_b=0U-t.tangent_q16[static_cast<std::size_t>(0x707-angle)];
  out.fractional_arg_a=b.origin_a_fraction;out.fractional_arg_b=b.origin_b_fraction;
 }else if(angle<0xA8C){
  out.quadrant=2;out.step_a=0xFFFF0000U;out.step_b=0x00010000U;out.left_compare_ge=true;out.right_compare_ge=false;
  out.tangent_a=0U-t.tangent_q16[static_cast<std::size_t>(0xA8B-angle)];
  out.tangent_b=t.tangent_q16[static_cast<std::size_t>(angle-0x708)];
  out.fractional_arg_a=b.origin_a_fraction_neg;out.fractional_arg_b=b.origin_b_fraction;
 }else{
  out.quadrant=3;out.step_a=0x00010000U;out.step_b=0x00010000U;out.left_compare_ge=true;out.right_compare_ge=true;
  out.tangent_a=t.tangent_q16[static_cast<std::size_t>(angle-0xA8C)];
  out.tangent_b=t.tangent_q16[static_cast<std::size_t>(0xE0F-angle)];
  out.fractional_arg_a=b.origin_a_fraction_neg;out.fractional_arg_b=b.origin_b_fraction_neg;
 }
 out.sweep_255f0=FixedProjection::multiply_unsigned_factor(out.tangent_a,out.fractional_arg_a)+b.origin_b;
 out.sweep_255e8=FixedProjection::multiply_unsigned_factor(out.tangent_b,out.fractional_arg_b)+b.origin_a;
 out.cross_cx=high_s16(out.sweep_255f0);
 out.cross_bx=high_s16(out.sweep_255e8);
 return out;
}
}  // namespace rloft
