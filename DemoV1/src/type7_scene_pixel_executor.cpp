#include "rloft/type7_scene_pixel_executor.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft {

Type7PixelDrawReport Type7ScenePixelExecutor::draw(
    const WorldSceneRenderPlan& plan,
    const Type7VgaSceneTrace& trace,
    const std::vector<std::uint8_t>& decoded_res3_body,
    const RendererTables& tables,
    VgaPlanarMemory& video) {
    Type7PixelDrawReport report{};
    if (trace.runs.empty()) {
        if (!plan.visible_runs.empty()) {
            throw std::logic_error(
                "visible scene runs were omitted from VGA trace");
        }
        return report;
    }
    if (!plan.rasterizer_setup.drawable ||
        trace.runs.size()!=plan.visible_runs.size()) {
        throw std::logic_error(
            "scene pixel executor received inconsistent render plan");
    }
    const auto& block=tables.scale_blocks.at(
        plan.traversal.scale_block_index);
    // The SINTAB source code is restricted to exactly its certified opcode
    // grammar before any temporary self-modifying-code patch is introduced.
    (void)SintabPixelMicrocode::audit(block);
    auto patched=Type7RasterizerPatchModel::stage(
        block,plan.rasterizer_setup);

    for (std::size_t run_index=0U;
         run_index<trace.runs.size();
         ++run_index) {
        const auto& run=trace.runs[run_index];
        const auto& planned=plan.visible_runs[run_index];
        if (run.source_x!=planned.source_x ||
            run.source_column_index!=planned.source_column_index ||
            run.viewport_x!=planned.viewport_x ||
            run.width!=planned.width) {
            throw std::invalid_argument(
                "scene VGA raster trace does not match visible run");
        }

        // 36B14: grouped paths dispatch one 369D8 with up to three VGA
        // plane passes. Individual-column paths dispatch one 369D8 per
        // destination column, each with a separate parity-selected ECX.
        // Group contiguous plane-pass records by destination column.
        const auto& passes=run.dispatch.calls;
        std::size_t first=0U;
        while(first<passes.size()) {
            auto last=first+1U;
            while(last<passes.size() &&
                  passes[last].destination_column==
                      passes[first].destination_column) {
                ++last;
            }

            // 369D8 is descriptor-major within each 36B14 invocation:
            // for each descriptor patch a temporary C3 at stop,
            // invoke generated code for all active VGA masks, restore C3,
            // then move to the next descriptor.
            for (const auto& descriptor:run.descriptors) {
                if (descriptor.source_x!=run.source_x ||
                    descriptor.source_column_index!=run.source_column_index ||
                    descriptor.stop_code_offset>=patched.code.size() ||
                    descriptor.entry_code_offset>=patched.code.size()) {
                    throw std::out_of_range(
                        "scene type-7 descriptor leaves SINTAB code body");
                }
                const auto original=
                    patched.code[descriptor.stop_code_offset];
                patched.code[descriptor.stop_code_offset]=0xC3U;
                ++report.descriptor_invocations;

                for(std::size_t i=first;i<last;++i) {
                    const auto& pass=passes[i];
                    const auto result=
                        SintabPixelMicrocode::execute(
                            patched.code,
                            descriptor.entry_code_offset,
                            decoded_res3_body,
                            static_cast<std::int64_t>(
                                descriptor.signed_source_adjustment),
                            pass.ecx_lookup,
                            pass.physical_address,
                            pass.sequencer_mask,
                            video);
                    ++report.vga_passes;
                    report.source_reads+=result.source_reads;
                    report.indexed_writes+=result.destination_writes;
                }
                patched.code[descriptor.stop_code_offset]=original;
            }
            first=last;
        }
    }
    // Code mutations were made only to this private byte buffer.
    // The original RendererScaleBlock and its generated code stay immutable.
    Type7RasterizerPatchModel::restore(patched);
    if(patched.code!=block.rasterizer_x86) {
        throw std::runtime_error(
            "scene SINTAB temporary code patches failed restoration");
    }
    return report;
}

} // namespace rloft
