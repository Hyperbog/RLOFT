#include "rloft_native/native_code2_scene_staging.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft_native {
namespace {
std::uint16_t read16(const NativeCode2SceneRecord22& a,
                     std::size_t p) noexcept {
    return std::uint16_t(a[p]) | (std::uint16_t(a[p+1U])<<8U);
}
std::uint16_t read16(const std::vector<std::uint8_t>& a,
                     std::size_t p) noexcept {
    return std::uint16_t(a[p]) | (std::uint16_t(a[p+1U])<<8U);
}
std::uint32_t read32(const NativeCode2SceneRecord22& a,
                     std::size_t p) noexcept {
    return std::uint32_t(read16(a,p)) |
           (std::uint32_t(read16(a,p+2U))<<16U);
}
void write16(NativeCode2SceneRecord22& a,std::size_t p,
             std::uint16_t value) noexcept {
    a[p]=static_cast<std::uint8_t>(value);
    a[p+1U]=static_cast<std::uint8_t>(value>>8U);
}
}
NativeCode2StagedNonNpc stage_native_code2_non_npc_2bbf4(
    NativeCode2NonNpcFamily family,
    const NativeCode2SceneRecord22& live_source_record,
    const std::vector<std::uint8_t>& original_definition,
    bool current_cell_visible,
    const NativeCode2ProjectionRegisters& regs,
    std::uint8_t original_minimum_height_c02d,
    const NativeCode2SceneRecord22& preexisting_output_record) {
    const bool placed=family==NativeCode2NonNpcFamily::InitPlaced;
    if(!placed && family!=NativeCode2NonNpcFamily::ObjectsLoose)
        throw std::invalid_argument("unknown CODE.2 non-NPC source class");
    const std::size_t required=placed?7U:0x41U;
    if(original_definition.size()!=required)
        throw std::invalid_argument("wrong original scene definition stride");
    NativeCode2StagedNonNpc result;
    result.record=preexisting_output_record;
    if(!current_cell_visible)return result;

    // 2BC0A/2BE5E: 7-byte INIT descriptors or 0x41-byte OBJECTS
    // descriptors, with separate source-backed base/stride offsets.
    const std::size_t base_offset=placed?0U:0x28U;
    const std::size_t stride_offset=placed?4U:0x2AU;
    const auto packed=read16(live_source_record,0x14U);
    const auto subframe_bits=static_cast<std::uint16_t>(
        static_cast<std::uint16_t>(std::uint32_t(packed)<<7U)>>13U);
    const auto frame=static_cast<std::uint16_t>(
        read16(original_definition,base_offset)+
        unsigned(original_definition[stride_offset])*unsigned(subframe_bits));
    write16(result.record,4U,frame);
    write16(result.record,0x0CU,0U);

    // 2BC76 / 2BECB invoke original 35798 with x/y DWORDs from
    // source+0/+4 and output screen/height WORDs at target+0/+2.
    const auto projected=project_native_code2_point_35798(
        regs,read32(live_source_record,0U),
        read32(live_source_record,4U),read16(result.record,0U));
    write16(result.record,0U,projected.original_screen_x);
    write16(result.record,2U,projected.original_projected_height);
    for(std::size_t p=0x0EU;p<=0x14U;p+=2U)
        write16(result.record,p,0U);

    // 2BCA2..2BCC2 / 2BEF7..2BF17: zero and <=C02D fail the
    // original acceptance gate. Subsequent animation/callback logic
    // is NOT performed; caller must not publish this as a final draw.
    result.passed_original_projection_gate=
        projected.original_projected_height>original_minimum_height_c02d;
    return result;
}
} // namespace rloft_native
