#pragma once

#include <cstdint>

namespace rloft_native {

// Uninterpreted DWORD/WORD values at original CODE.2 DGROUP offsets.
// Source is owned by the original call site; using arbitrary diagnostic
// values is NOT equivalent to reconstructing the original camera.
struct NativeCode2ProjectionRegisters {
    std::uint32_t live_x_255f0{};
    std::uint32_t origin_x_255ec{};
    std::uint32_t live_y_255e8{};
    std::uint32_t origin_y_255f4{};
    std::uint32_t direction_x_255f8{};
    std::uint32_t direction_y_255fc{};
    std::uint32_t height_numerator_255e0{};
    std::uint32_t horizontal_scale_c6cc{};
    std::uint16_t horizontal_origin_c6d0{};
};

// Exactly CODE.2 35680's unsigned two-part 16x16 multiplication
// with 32-bit input magnitude and the coefficient's low 16 bits;
// high bit 31 of the coefficient controls sign. The result is
// trunc((abs(input) * coefficient_low_word) / 65536) with source
// sign correction and original modulo-32-bit output.
// This is NOT ordinary 16.16 signed 32x32 multiplication.
[[nodiscard]] std::uint32_t native_code2_mul_35680(
    std::uint32_t signed_input,std::uint32_t original_coefficient) noexcept;

// Original 35730: projected height return is low 16 bits of the
// UNSIGNED 0x255E0 / max(signed(depth),0x1680)>>8 calculation.
struct NativeCode2WallHeight {
    std::uint32_t source_depth{};
    std::uint32_t clamped_depth{};
    std::uint32_t unsigned_divisor{};
    std::uint16_t original_return_word{};
    bool clamped_to_original_near_limit{};
    bool original_runtime_viewpoint_verified{false};
};
[[nodiscard]] NativeCode2WallHeight project_native_code2_wall_height_35730(
    const NativeCode2ProjectionRegisters& regs);

// CODE.2 35798 projected point: signed 64/32 IDIV for horizontal
// placement and signed 32/32 IDIV for projected height. Rejects the
// original (depth<0x5800) near plane by zeroing ONLY the output
// height WORD; pre-existing screen X WORD remains untouched.
struct NativeCode2ProjectedPoint {
    bool accepted{};
    std::uint32_t source_depth{};
    std::uint32_t clipped_depth{};
    std::uint32_t transverse{};
    std::uint16_t original_screen_x{};
    std::uint16_t original_projected_height{};
    bool original_camera_traversal_verified{false};
    bool original_dos_frame_parity_verified{false};
};
[[nodiscard]] NativeCode2ProjectedPoint project_native_code2_point_35798(
    const NativeCode2ProjectionRegisters& regs,
    std::uint32_t subject_x,std::uint32_t subject_y,
    std::uint16_t preceding_screen_x);

// Original 35798 signed IDIV failure is an arithmetic fault. This
// portable reimplementation throws on 32-bit signed-quotient overflow
// rather than returning invented geometry.
} // namespace rloft_native
