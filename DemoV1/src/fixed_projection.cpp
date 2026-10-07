#include "rloft/fixed_projection.hpp"
#include <cstdint>
#include <stdexcept>
namespace rloft { namespace {
std::uint32_t mul_fraction(std::uint32_t value,std::uint32_t factor,bool factor_sign) noexcept {
 const std::uint16_t bx=static_cast<std::uint16_t>(factor);
 std::uint16_t si=factor_sign?static_cast<std::uint16_t>(factor>>16U):0U;
 if(static_cast<std::int16_t>(value>>16U)<0){
   value=0U-value;
   si=static_cast<std::uint16_t>(si^0x8000U);
 }
 std::uint32_t product=static_cast<std::uint32_t>(static_cast<std::uint16_t>(value))*bx;
 const std::uint16_t di=static_cast<std::uint16_t>(product>>16U);
 product=static_cast<std::uint32_t>(static_cast<std::uint16_t>(value>>16U))*bx;
 std::uint16_t ax=static_cast<std::uint16_t>(product);
 std::uint16_t dx=static_cast<std::uint16_t>(product>>16U);
 const std::uint32_t sum=static_cast<std::uint32_t>(ax)+di;
 ax=static_cast<std::uint16_t>(sum);
 dx=static_cast<std::uint16_t>(static_cast<std::uint32_t>(dx)+(sum>>16U));
 if(static_cast<std::int16_t>(si)<0){
   dx=static_cast<std::uint16_t>(0U-static_cast<std::uint32_t>(dx));
   const bool borrow=ax!=0U;
   ax=static_cast<std::uint16_t>(0U-static_cast<std::uint32_t>(ax));
   dx=static_cast<std::uint16_t>(static_cast<std::uint32_t>(dx)-(borrow?1U:0U));
 }
 return (static_cast<std::uint32_t>(dx)<<16U)|ax;
}
}
std::uint32_t FixedProjection::multiply_signed_factor(std::uint32_t v,std::uint32_t f) noexcept{return mul_fraction(v,f,true);}
std::uint32_t FixedProjection::multiply_unsigned_factor(std::uint32_t v,std::uint32_t f) noexcept{return mul_fraction(v,f,false);}
std::uint32_t FixedProjection::sweep_depth(const SweepDepthInput& i){
 const auto a=i.current_a-i.origin_a;
 const auto b=i.current_b-i.origin_b;
 const auto first=multiply_signed_factor(a,i.factor_a);
 const auto second=multiply_signed_factor(b,i.factor_b);
 std::uint32_t delta=first-second;
 if(static_cast<std::int32_t>(delta)<static_cast<std::int32_t>(0x1680U))delta=0x1680U;
 const auto divisor=delta>>8U;
 if(divisor==0U)throw std::runtime_error("encoded sweep depth divisor is zero");
 return (i.numerator/divisor)&0xFFFFU;
}
}  // namespace rloft
