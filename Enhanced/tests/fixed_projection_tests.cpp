#include "rloft/fixed_projection.hpp"
#include <cassert>
#include <cstdint>
int main(){
 using rloft::FixedProjection;
 assert(FixedProjection::multiply_signed_factor(0x00010000U,0x00008000U)==0x00008000U);
 assert(FixedProjection::multiply_signed_factor(0xFFFF0000U,0x00008000U)==0xFFFF8000U);
 assert(FixedProjection::multiply_signed_factor(0x00010000U,0xFFFF8000U)==0xFFFF8000U);
 assert(FixedProjection::multiply_unsigned_factor(0x00010000U,0xFFFF8000U)==0x00008000U);
 rloft::SweepDepthInput i{};i.current_a=0x00010000U;i.factor_a=0x00008000U;i.numerator=0x00100000U;
 assert(FixedProjection::sweep_depth(i)==0x2000U);
 i.current_a=0;i.factor_a=0;i.numerator=0x00001600U;
 assert(FixedProjection::sweep_depth(i)==0x0100U);
}
