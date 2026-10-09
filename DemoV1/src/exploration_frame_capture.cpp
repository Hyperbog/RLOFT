#include "rloft/exploration_frame_capture.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

namespace rloft {
namespace {
constexpr std::size_t kCode2DgroupBase=0x75F24U;
constexpr std::array<std::size_t,3> kMaskOffsets{{
    0xC668U,0xC688U,0xC6A8U
}};
std::string filename(std::size_t sequence) {
    std::ostringstream name;
    name<<"frame_"<<std::setw(4)<<std::setfill('0')<<sequence;
    return name.str();
}
void write_binary(const std::filesystem::path& path,
                  const std::uint8_t* bytes,std::size_t size) {
    std::ofstream out(path,std::ios::binary|std::ios::trunc);
    if (!out || !out.write(
            reinterpret_cast<const char*>(bytes),
            static_cast<std::streamsize>(size))) {
        throw std::runtime_error(
            "Unable to write native exploration capture: "+path.string());
    }
}
const char* scene_outcome_name(Type7SceneRenderOutcome outcome) {
    switch(outcome) {
        case Type7SceneRenderOutcome::PlanningFailed:
            return "planning-failed";
        case Type7SceneRenderOutcome::FullyOccluded:
            return "occluded";
        case Type7SceneRenderOutcome::TraceUnavailable:
            return "trace-unavailable";
        case Type7SceneRenderOutcome::Rasterized:
            return "rasterized";
        case Type7SceneRenderOutcome::RasterFailed:
            return "raster-failed";
    }
    throw std::invalid_argument("unknown native RES3 scene outcome");
}
std::string clean(std::string value) {
    for (auto& c:value) {
        if (c=='\t' || c=='\r' || c=='\n') c=' ';
    }
    return value;
}
} // namespace

void ExplorationFrameCapture::write(
    const std::filesystem::path& directory,
    const ExplorationCapturePosition& position,
    const ExplorationRuntimeFrame& runtime,
    const ExplorationPresentedFrame& presented,
    const IndexedFramebuffer& framebuffer,
    const std::vector<std::uint8_t>& code2,
    std::string_view source_revision) {
    const auto& pixels=framebuffer.pixels();
    constexpr std::size_t kPixelCount=
        static_cast<std::size_t>(IndexedFramebuffer::kWidth)*
        IndexedFramebuffer::kHeight;
    if (pixels.size()!=kPixelCount ||
        code2.size()<kCode2DgroupBase+0xC6C8U+1U ||
        presented.type7_scene_outcomes.size()!=
            runtime.world_scene_render_plans.size()) {
        throw std::invalid_argument(
            "Exploration capture requires indexed 320x400 and original CODE.2");
    }
    std::filesystem::create_directories(directory);
    const auto base=directory/filename(position.sequence);
    write_binary(base.string()+".idx",pixels.data(),pixels.size());
    write_binary(base.string()+".pal",
                 presented.palette.rgb.data(),
                 presented.palette.rgb.size());

    std::ofstream manifest(base.string()+".tsv",
                           std::ios::binary|std::ios::trunc);
    if (!manifest)
        throw std::runtime_error("Unable to write capture metadata");
    const auto& coverage=presented.surface_coverage;
    manifest<<"format\trloft-native-indexed-capture-v2\n"
            <<"provenance\tnative-preview-not-original-DOS\n"
            <<"source_revision\t"<<clean(std::string(source_revision))<<"\n"
            <<"dimensions\t320\t400\n"
            <<"palette\t256x3\tVGA-6bit-raw\n"
            <<"level\t"<<position.level<<"\n"
            <<"cell\t"<<position.cell_x<<"\t"<<position.cell_y<<"\n"
            <<"facing\t"<<position.facing<<"\n"
            <<"scene_video_page\t"<<unsigned(runtime.scene_video_page)<<"\n"
            <<"scene_individual_columns_mode\t"
            <<unsigned(runtime.scene_individual_columns_mode?1U:0U)<<"\n"
            <<"source_coordinate_fixed\t"
            <<runtime.source_coordinate_a<<"\t"
            <<runtime.source_coordinate_b<<"\n"
            <<"source_facing_raw\t"<<runtime.source_facing<<"\n"
            <<"main_runs\t"<<coverage.sintab_main_runs_rasterized
            <<"\t"<<coverage.sintab_main_runs_pending
            <<"\t"<<coverage.sintab_main_runs_failed<<"\n"
            <<"prepass_runs\t"<<coverage.sintab_prepass_runs_rasterized
            <<"\t"<<coverage.sintab_prepass_runs_pending
            <<"\t"<<coverage.sintab_prepass_runs_failed<<"\n"
            <<"mask_origin\timmutable CODE.2 object-2, not DOS live RAM\n"
            <<"scene_records\t"<<runtime.world_scene_records.size()<<"\n"
            <<"scene_draw_calls\t"<<runtime.world_scene_draw_calls.size()<<"\n"
            <<"scene_render_plans\t"<<runtime.world_scene_render_plans.size()<<"\n"
            <<"scene_plan_failures\t"<<runtime.scene_plan_failures.size()<<"\n"
            <<"scene_video_calls\t"<<coverage.type7_calls_rasterized
            <<"\t"<<coverage.type7_scene_calls_pending
            <<"\t"<<coverage.type7_calls_failed<<"\n"
            <<"scene_columns\t"<<coverage.type7_columns_rasterized
            <<"\t"<<coverage.type7_visible_columns_pending
            <<"\t"<<coverage.type7_unvalidated_wide_runs<<"\n"
            <<"scene_indexed_writes\t"<<coverage.type7_indexed_writes<<"\n"
            <<"run\tpass\tfirst_ray\twidth_C6F4\tphase\tmask_index"
            <<"\tC668\tC688\tC6A8\tdepth\tresource"
            <<"\ttexture_column\tshade_decrement\n";

    for (const auto& run:runtime.sweep_runs) {
        const std::uint64_t index=
            (static_cast<std::uint64_t>(run.first_ray & 3U)<<3U)+
            run.ray_count;
        manifest<<"run\t"
                <<(run.pass==SweepRenderPass::Main?"main":"prepass")
                <<"\t"<<run.first_ray<<"\t"<<run.ray_count
                <<"\t"<<(run.first_ray&3U)<<"\t"<<index;
        for(const auto offset:kMaskOffsets) {
            const std::uint64_t pos=
                kCode2DgroupBase+offset+index;
            if(pos<code2.size()) {
                manifest<<"\t"<<unsigned(code2[
                    static_cast<std::size_t>(pos)]);
            } else {
                manifest<<"\tOUT_OF_FILE";
            }
        }
        manifest<<"\t"<<run.depth<<"\t"<<run.resource_id
                <<"\t"<<run.texture_column_index
                <<"\t"<<run.shade_decrement<<"\n";
    }
    // These are original-world, source-indexed visibility decisions,
    // distinct from environment backdrop placements or wall rays.
    manifest<<"scene_record\tclass\tsource_index\tRES3_resource"
            <<"\tscreen_x\tprojected_extent\trender_order"
            <<"\tsource_type_id\n";
    for(const auto& record:runtime.world_scene_records) {
        manifest<<"scene_record\t"<<record.source_class
                <<"\t"<<record.source_index
                <<"\t"<<record.resource_id
                <<"\t"<<record.screen_x
                <<"\t"<<record.projected_extent
                <<"\t"<<record.render_order
                <<"\t"<<record.source_type_id<<"\n";
    }
    manifest<<"scene_call\tclass\tsource_index\tRES3_resource"
            <<"\tvisible_columns\tscale_extent\tplan_status\n";
    for(std::size_t call_index=0U;
        call_index<runtime.world_scene_render_plans.size();
        ++call_index) {
        const auto& plan=runtime.world_scene_render_plans[call_index];
        manifest<<"scene_call\t"<<plan.call.source_class
                <<"\t"<<plan.call.source_index
                <<"\t"<<plan.call.resource_id
                <<"\t"<<plan.visible_columns.size()
                <<"\t"<<plan.call.scale_extent
                <<"\t"<<(plan.source_unresolved?"unresolved":"planned")
                <<"\n";
        // 36B14's source-column runs must retain original width.
        // In C029=1 an eight-plus-pixel source run is a sequence
        // of exact width-1 original 369D8 calls, NOT a grouped
        // VGA mask-table overread. Preserve this distinction in TSV.
        for(const auto& run:plan.visible_runs) {
            const auto supported=run.width!=0U &&
                run.width<=255U &&
                (runtime.scene_individual_columns_mode ||
                    run.width<=7U);
            manifest<<"scene_run\t"<<call_index
                    <<"\t"<<run.source_x
                    <<"\t"<<run.source_column_index
                    <<"\t"<<run.viewport_x
                    <<"\t"<<run.width
                    <<"\t"<<(runtime.scene_individual_columns_mode
                           ?"individual":"grouped")
                    <<"\t"<<(supported?"supported":"unverified")
                    <<"\n";
        }
    }
    for(const auto& failure:runtime.scene_plan_failures) {
        manifest<<"scene_plan_failure\t"<<failure.call_index
                <<"\t"<<failure.source_class
                <<"\t"<<failure.source_index
                <<"\t"<<failure.resource_id
                <<"\t"<<clean(failure.reason)<<"\n";
    }
    // Call-aligned native raster results, including calls that were
    // projected but not displayed because a trace is still unverified.
    for(std::size_t index=0U;
        index<presented.type7_scene_outcomes.size(); ++index) {
        const auto& call=runtime.world_scene_render_plans[index].call;
        manifest<<"scene_outcome\t"<<index
                <<"\t"<<call.source_class
                <<"\t"<<call.source_index
                <<"\t"<<call.resource_id
                <<"\t"<<scene_outcome_name(
                    presented.type7_scene_outcomes[index])
                <<"\n";
    }
    if(runtime.scene_vga_trace_failure.has_value()) {
        manifest<<"scene_trace_failure\t"
                <<clean(*runtime.scene_vga_trace_failure)<<"\n";
    }
    // Partial trace failure does not prevent unrelated original objects
    // drawing. Preserve exact call index and source identity for replay.
    for(const auto& failure:runtime.scene_vga_scene_failures) {
        manifest<<"scene_call_trace_failure\t"
                <<failure.call_index<<"\t"
                <<failure.source_class<<"\t"
                <<failure.source_index<<"\t"
                <<failure.resource_id<<"\t"
                <<clean(failure.reason)<<"\n";
    }
    for(const auto& failure:presented.type7_failures) {
        manifest<<"scene_raster_failure\t"<<failure.source_class
                <<"\t"<<failure.source_index
                <<"\t"<<failure.resource_id
                <<"\t"<<clean(failure.reason)<<"\n";
    }
    manifest<<"failure\tpass\tresource\tfirst_ray\treason\n";
    for (const auto& failed:presented.surface_main_failures) {
        manifest<<"failure\tmain\t"<<failed.resource_id
                <<"\t"<<failed.first_ray<<"\t"
                <<clean(failed.reason)<<"\n";
    }
    for (const auto& failed:presented.surface_prepass_failures) {
        manifest<<"failure\tprepass\t"<<failed.resource_id
                <<"\t"<<failed.first_ray<<"\t"
                <<clean(failed.reason)<<"\n";
    }
    if(!manifest)
        throw std::runtime_error("Unable to complete capture metadata");
}

void ExplorationFrameCapture::write_failure(
    const std::filesystem::path& directory,
    const ExplorationCaptureFailure& failure) {
    std::filesystem::create_directories(directory);
    std::ofstream manifest(
        directory/"failure.tsv", std::ios::binary|std::ios::trunc);
    if (!manifest)
        throw std::runtime_error("Unable to write native sweep failure capture");
    manifest<<"format\trloft-native-exploration-failure-v1\n"
            <<"provenance\tnative-preview-not-original-DOS\n"
            <<"source_revision\t"<<clean(failure.source_revision)<<"\n"
            <<"stage\tbuild_frame\n"
            <<"level\t"<<failure.level<<"\n"
            <<"coordinate_fixed\t"<<failure.coordinate_a
            <<"\t"<<failure.coordinate_b<<"\n"
            <<"cell\t"<<(static_cast<std::int32_t>(failure.coordinate_a)>>16)
            <<"\t"<<(static_cast<std::int32_t>(failure.coordinate_b)>>16)
            <<"\n"
            <<"facing\t"<<failure.facing<<"\n"
            <<"dimensions\t"<<failure.map_width
            <<"\t"<<failure.map_height<<"\n"
            <<"prior_frames_written\t"<<failure.frames_written<<"\n"
            <<"exception\t"<<clean(failure.reason)<<"\n";
    if (!manifest)
        throw std::runtime_error("Unable to complete native sweep failure capture");
}

} // namespace rloft
