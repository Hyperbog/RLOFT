#include "rloft/sintab_pixel_microcode.hpp"

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
rloft::RendererScaleBlock example() {
    rloft::RendererScaleBlock b{};
    b.rasterizer_x86 = {
        0x8A,0x86,0x00,0x00,0x00,0x00, // mov al,[esi]
        0x88,0xC4,                     // mov ah,al
        0x29,0xC8,                     // sub eax,ecx
        0x88,0x87,0x50,0x00,0x00,0x00, // mov [edi+80],al
        0x88,0xA7,0xA0,0x00,0x00,0x00, // mov [edi+160],ah
        0xC3,                          // ret
    };
    b.code_patch_offset.fill(0x0204U);
    b.code_patch_offset[128U] = 0x021AU;
    return b;
}
} // namespace

int main() {
    const auto block=example();
    const auto audit=rloft::SintabPixelMicrocode::audit(block);
    assert(audit.source_reads==1U);
    assert(audit.mov_ah_al==1U);
    assert(audit.sub_eax_ecx==1U);
    assert(audit.destination_writes==2U);
    assert(audit.returns==1U);
    assert(audit.instruction_count==6U);

    rloft::VgaPlanarMemory video;
    const auto destination =
        rloft::VgaPlanarMemory::kPhysicalBase + 20U;
    // 0xA5A5 - 0x0102 = 0xA4A3. The AL byte goes to row 1;
    // AH goes to row 2, preserving the exact x86 subtraction.
    const auto result=rloft::SintabPixelMicrocode::execute(
        block.rasterizer_x86,0U,{0xA5U},0,0x0102U,
        destination,0x05U,video);
    assert(result.source_reads==1U);
    assert(result.destination_writes==2U);
    assert(video.read(destination+80U,0U)==0xA3U);
    assert(video.read(destination+80U,2U)==0xA3U);
    assert(video.read(destination+160U,0U)==0xA4U);
    assert(video.read(destination+160U,2U)==0xA4U);
    assert(video.read(destination+80U,1U)==0U);

    // CODE.2 369D8 inserts a temporary C3 at the stop-selector
    // patch offset and restores it after each VGA plane pass.
    auto interrupted=block.rasterizer_x86;
    interrupted[16U]=0xC3U;
    video.clear();
    const auto short_result=rloft::SintabPixelMicrocode::execute(
        interrupted,0U,{0xA5U},0,0x0102U,
        destination,0x01U,video);
    assert(short_result.destination_writes==1U);
    assert(video.read(destination+80U,0U)==0xA3U);
    assert(video.read(destination+160U,0U)==0U);

    // Patched EB displacement skips exactly a six-byte store and
    // preserves the high color byte from the earlier SUB.
    auto jumped=block.rasterizer_x86;
    jumped[10U]=0xEBU;
    jumped[11U]=0x04U; // after 2-byte JMP skip remaining 4 bytes
    video.clear();
    const auto jumped_result=rloft::SintabPixelMicrocode::execute(
        jumped,0U,{0xA5U},0,0x0102U,
        destination,0x08U,video);
    assert(jumped_result.destination_writes==1U);
    assert(video.read(destination+80U,3U)==0U);
    assert(video.read(destination+160U,3U)==0xA4U);

    // Every decoded source address is based on original ESI plus the
    // displacement embedded in the literal SINTAB code.
    video.clear();
    const auto shifted=rloft::SintabPixelMicrocode::execute(
        block.rasterizer_x86,0U,{0U,0xB1U},1,0x0201U,
        destination,0x02U,video);
    assert(shifted.source_reads==1U);
    assert(video.read(destination+80U,1U)==0xB0U);
    assert(video.read(destination+160U,1U)==0xAFU);

    bool rejected=false;
    try {
        (void)rloft::SintabPixelMicrocode::execute(
            block.rasterizer_x86,0U,{},0,0U,
            destination,1U,video);
    } catch (const std::out_of_range&) {rejected=true;}
    assert(rejected);
    rejected=false;
    try {
        (void)rloft::SintabPixelMicrocode::execute(
            block.rasterizer_x86,0U,{0xA5U},0,0U,
            0x90000U,1U,video);
    } catch (const std::out_of_range&) {rejected=true;}
    assert(rejected);

    auto invalid=block;
    invalid.rasterizer_x86[8U]=0x90U;
    rejected=false;
    try {
        (void)rloft::SintabPixelMicrocode::audit(invalid);
    } catch (const std::runtime_error&) {rejected=true;}
    assert(rejected);

    // CI does not ship copyrighted assets. If an original SINTAB is
    // installed locally, verify every generated code body with this
    // same portable audit; otherwise the synthetic tests still run.
    const auto path=std::getenv("RLOFT_SINTAB_PATH");
    if (path!=nullptr && path[0]!=0) {
        std::ifstream file(path,std::ios::binary);
        if(!file) throw std::runtime_error("cannot open SINTAB");
        std::vector<std::uint8_t> bytes(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());
        const auto tables=rloft::RendererTables::parse(bytes);
        assert(tables.scale_blocks.size()==127U);
        std::size_t total_reads=0U, total_writes=0U;
        std::vector<std::uint8_t> original_source(4096U,0U);
        for(std::size_t i=0U;i<original_source.size();++i) {
            original_source[i]=
                static_cast<std::uint8_t>((i*31U+17U)&0xFFU);
        }

        // Independent Python reference executes the original 127 code
        // bodies with the same deterministic ESI bytes, zeroed planar
        // video for each block, and both ECX lookup variants. Hash each
        // final 21,000-byte plane-0 region with FNV-1a (64-bit), then fold
        // the block index and digest's eight little-endian bytes into a
        // corpus checksum. This detects actual wrong pixel values/rows,
        // not merely the previously tested read/write instruction counts.
        constexpr std::uint64_t fnv_start =
            0xCBF29CE484222325ULL;
        constexpr std::uint64_t fnv_prime =
            0x100000001B3ULL;
        const auto mix =
            [](std::uint64_t hash, std::uint8_t value) {
                return (hash ^ value) * fnv_prime;
            };
        std::uint64_t fingerprints[2] = {
            fnv_start, fnv_start,
        };
        constexpr std::uint32_t ecx_cases[2] = {
            0x0102U, 0x3412U,
        };
        for(std::size_t block_index=0U;
            block_index<tables.scale_blocks.size();
            ++block_index) {
            const auto& original =
                tables.scale_blocks[block_index];
            const auto report =
                rloft::SintabPixelMicrocode::audit(original);
            total_reads+=report.source_reads;
            total_writes+=report.destination_writes;
            for(std::size_t case_index=0U;
                case_index<2U;
                ++case_index) {
                rloft::VgaPlanarMemory block_video;
                const auto execution =
                    rloft::SintabPixelMicrocode::execute(
                        original.rasterizer_x86,
                        0U,
                        original_source,
                        0,
                        ecx_cases[case_index],
                        rloft::VgaPlanarMemory::kPhysicalBase,
                        0x0FU,
                        block_video);
                assert(execution.source_reads==report.source_reads);
                assert(execution.destination_writes==
                       report.destination_writes);

                std::uint64_t digest = fnv_start;
                for(std::uint32_t offset=0U;
                    offset<21000U;
                    ++offset) {
                    digest = mix(
                        digest,
                        block_video.read(
                            rloft::VgaPlanarMemory::kPhysicalBase+
                                offset,
                            0U));
                }
                auto& aggregate = fingerprints[case_index];
                aggregate = mix(
                    aggregate,
                    static_cast<std::uint8_t>(block_index));
                for(unsigned shift=0U;shift<64U;shift+=8U) {
                    aggregate = mix(
                        aggregate,
                        static_cast<std::uint8_t>(
                            (digest>>shift)&0xFFU));
                }
            }
        }
        assert(fingerprints[0]==0x0658C6DB74B80324ULL);
        assert(fingerprints[1]==0x0D284A7FB40210EEULL);
        assert(total_reads==11587U);
        assert(total_writes==23910U);
    }
}
