#include "rloft_native/native_code2_projection.hpp"

#include <cstdint>
#include <limits>
#include <stdexcept>

namespace rloft_native {
namespace {
constexpr std::int64_t kUnsignedDword=4294967296LL;
std::int64_t as_signed32(std::uint32_t value) noexcept {
    return value<=0x7fffffffU?std::int64_t(value):
        std::int64_t(value)-kUnsignedDword;
}
std::uint32_t wrapped_sum(std::uint32_t a,std::uint32_t b) noexcept {
    return a+b; // original modulo-32-bit arithmetic
}
std::uint32_t wrapped_difference(std::uint32_t a,std::uint32_t b) noexcept {
    return a-b;
}
std::int64_t checked_signed_quotient(
    std::int64_t numerator,std::int64_t denominator) {
    if(denominator==0)
        throw std::runtime_error("original CODE.2 signed division by zero");
    const auto quotient=numerator/denominator; // signed IDIV rounds toward zero
    if(quotient<std::numeric_limits<std::int32_t>::min() ||
       quotient>std::numeric_limits<std::int32_t>::max())
        throw std::runtime_error(
            "original CODE.2 signed 32-bit division quotient overflow");
    return quotient;
}
}

std::uint32_t native_code2_mul_35680(
    std::uint32_t value,std::uint32_t coefficient) noexcept {
    // The x86 uses MUL BX twice (low/high 16-bit halves of ABS(value))
    // and carries DX into the high partial product. Its 32-bit result
    // is the low 32 bits of ((abs(value)*coefficient.low16)>>16).
    const bool negative_input=(value&0x80000000U)!=0;
    const bool negative_coefficient=(coefficient&0x80000000U)!=0;
    const auto magnitude=negative_input?std::uint32_t(0U-value):value;
    const auto low_coefficient=std::uint16_t(coefficient);
    const auto product=(std::uint64_t(magnitude)*
                        std::uint64_t(low_coefficient))>>16U;
    const auto positive=static_cast<std::uint32_t>(product);
    return negative_input!=negative_coefficient?
        std::uint32_t(0U-positive):positive;
}

NativeCode2WallHeight project_native_code2_wall_height_35730(
    const NativeCode2ProjectionRegisters& regs) {
    const auto delta_x=wrapped_difference(
        regs.live_x_255f0,regs.origin_x_255ec);
    const auto delta_y=wrapped_difference(
        regs.live_y_255e8,regs.origin_y_255f4);
    const auto p=native_code2_mul_35680(delta_x,regs.direction_x_255f8);
    const auto q=native_code2_mul_35680(delta_y,regs.direction_y_255fc);
    const auto depth=wrapped_difference(p,q);
    const bool clamp=as_signed32(depth)<0x1680LL;
    const auto bounded=clamp?0x1680U:depth;
    const auto divisor=bounded>>8U; // original logical SHR
    if(divisor==0U)
        throw std::runtime_error("original CODE.2 wall height divisor zero");
    const auto height=regs.height_numerator_255e0/divisor; // original unsigned DIV
    return {depth,bounded,divisor,
            static_cast<std::uint16_t>(height&0xffffU),clamp,false};
}

NativeCode2ProjectedPoint project_native_code2_point_35798(
    const NativeCode2ProjectionRegisters& regs,
    std::uint32_t subject_x,std::uint32_t subject_y,
    std::uint16_t preceding_screen_x) {
    const auto dx=wrapped_difference(subject_x,regs.origin_x_255ec);
    const auto dy=wrapped_difference(subject_y,regs.origin_y_255f4);
    const auto depth=wrapped_difference(
        wrapped_difference(
            native_code2_mul_35680(dx,regs.direction_x_255f8),
            native_code2_mul_35680(dy,regs.direction_y_255fc)),
        0x2000U);
    const auto transverse=wrapped_sum(
        native_code2_mul_35680(dx,regs.direction_y_255fc),
        native_code2_mul_35680(dy,regs.direction_x_255f8));
    NativeCode2ProjectedPoint result;
    result.source_depth=depth;
    result.clipped_depth=depth;
    result.transverse=transverse;
    result.original_screen_x=preceding_screen_x;
    if(as_signed32(depth)<0x5800LL) {
        // Original writes word zero to [EDI], but leaves [ESI]
        // screen coordinate untouched in the rejected branch.
        return result;
    }
    const auto numerator=as_signed32(regs.horizontal_scale_c6cc)*
                         as_signed32(transverse);
    const auto xoffset=checked_signed_quotient(numerator,as_signed32(depth));
    result.original_screen_x=static_cast<std::uint16_t>(
        std::uint32_t(regs.horizontal_origin_c6d0)+
        static_cast<std::uint32_t>(xoffset));
    const auto divisor=as_signed32(depth)>>8U;
    const auto h=checked_signed_quotient(
        as_signed32(regs.height_numerator_255e0),divisor);
    result.original_projected_height=
        static_cast<std::uint16_t>(h);
    result.accepted=true;
    return result;
}
} // namespace rloft_native
