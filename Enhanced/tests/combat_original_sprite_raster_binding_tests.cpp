#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_sprite_raster_binding.hpp"
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

int main() {
    using namespace rloft;
    VgaPlanarMemory vga{};
    CombatOriginalSpriteMachineState machine{};
    machine.writable_code_object1.resize(0x2A676U);
    machine.writable_code_object1[0x2A609U]=0x8AU;
    machine.writable_code_object1[0x2A60AU]=0x83U;
    machine.writable_code_object1[0x2A670U]=0x8AU;
    machine.writable_code_object1[0x2A671U]=0x83U;
    CombatOriginalSpriteVideoState global{};
    global.first_row_shift_efed=4U;
    global.second_row_shift_eff1=2U;
    global.page_base_f001=0xA0000U;
    global.row_pitch_eff9=80U;
    CombatOriginalSpriteRefreshState refresh{};
    refresh.refresh_callback_pointer_f0c8=1U;
    std::uint32_t reads=0U,shader_reads=0U,refreshes=0U;

    CombatOriginalSpriteMemoryReaders readers{};
    readers.capture_sprite_at_pointer=[&](std::uint32_t addr)
      ->std::optional<CombatOriginalSpriteMemory> {
        ++reads;
        if(reads==2U) {
            // Original 2A590/2A595 writes BOTH code operands
            // before any shaded sprite header is read.
            assert(machine.writable_code_object1[0x2A60BU]==0U);
            assert(machine.writable_code_object1[0x2A60CU]==0x90U);
            assert(machine.writable_code_object1[0x2A672U]==0U);
            assert(machine.writable_code_object1[0x2A673U]==0x90U);
        }
        if(addr!=0x1000U)return std::nullopt;
        CombatOriginalSpriteMemory r{};
        r.captured_base_address=addr;
        r.resource_header_address=addr;
        r.bytes={8,0,0,0,1,0,1,0,10,11,20,21,30,31,40,41};
        return r;
    };
    readers.capture_shader_at_pointer=[&](std::uint32_t addr)
      ->std::optional<CombatOriginalShaderMemory> {
        ++shader_reads;
        if(addr!=0x9000U)return std::nullopt;
        CombatOriginalShaderMemory s{};
        s.captured_base_address=addr;
        s.original_lookup_pointer=addr;
        s.bytes.resize(256U);
        for(std::uint32_t i=0;i<256U;++i)
            s.bytes[i]=static_cast<std::uint8_t>(i^0x5AU);
        return s;
    };
    auto services=CombatOriginalSpriteRasterBinder::bind(
        vga,machine,global,readers,[&](){
            ++refreshes;
            assert(*vga.sequencer_map_mask_02()==2U);
        });

    CombatOriginalSpriteRuntimeModel::draw_normal_2939c(
        refresh,0,1U,2U,0x1000U,services);
    assert(reads==1U && shader_reads==0U && refreshes==1U);
    assert(vga.read(0xA0000U+80U,2U)==10U);
    assert(vga.read(0xA0000U+80U,3U)==20U);
    assert(vga.read(0xA0000U+81U,0U)==30U);
    assert(vga.read(0xA0000U+81U,1U)==40U);

    CombatOriginalSpriteRuntimeModel::draw_shaded_293ec(
        refresh,0,1U,2U,0x1000U,0x9000U,services);
    assert(reads==2U && shader_reads==1U && refreshes==2U);
    assert(vga.read(0xA0000U+80U,2U)==(10U^0x5AU));
    assert(vga.read(0xA0000U+81U,0U)==(30U^0x5AU));
    assert(machine.writable_code_object1[0x2A60BU]==0U);
    assert(machine.writable_code_object1[0x2A60CU]==0x90U);
    assert(machine.writable_code_object1[0x2A672U]==0U);
    assert(machine.writable_code_object1[0x2A673U]==0x90U);
    assert(global.row==0U && global.column==0U);
    assert(!machine.transparent_row_count_f009);

    // Existing wrapper's null resource bypasses capture and F0C8.
    CombatOriginalSpriteRuntimeModel::draw_normal_2939c(
        refresh,0,1U,2U,0U,services);
    assert(reads==2U && refreshes==2U);
    bool rejected=false;
    try {
        CombatOriginalSpriteRuntimeModel::draw_normal_2939c(
            refresh,0,1U,2U,0x9999U,services);
    } catch(const std::logic_error&) {rejected=true;}
    assert(rejected && refreshes==2U);
}
