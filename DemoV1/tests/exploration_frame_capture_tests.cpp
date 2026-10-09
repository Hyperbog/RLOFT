#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/exploration_frame_capture.hpp"

#include <cassert>
#include <cstdio>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

int main() {
    namespace fs=std::filesystem;
    using namespace rloft;
    // The native capture launcher uses the explicitly bounded extended
    // sampling mode, without allowing arbitrary filesystem growth.
    for (const auto count:{1U,16U,64U,128U}) {
        assert(ExplorationFrameCapture::parse_frame_limit(
            std::to_wstring(count))==count);
    }
    for (const auto invalid:{
        L"",L"0",L"129",L"999999999999999999999",
        L"-1",L"+16",L"16x",L" 16",L"1.6"
    }) {
        bool failed=false;
        try {
            (void)ExplorationFrameCapture::parse_frame_limit(invalid);
        } catch(const std::invalid_argument&) {
            failed=true;
        }
        assert(failed);
    }
    const auto dir=fs::temp_directory_path()/
        "rloft_exploration_frame_capture_test";
    fs::remove_all(dir);
    IndexedFramebuffer image{};
    image.clear(0x39U);
    ExplorationRuntimeFrame frame{};
    frame.scene_video_page=1U;
    ViewSweepRun wall{};
    wall.pass=SweepRenderPass::Main;
    wall.first_ray=3U;
    wall.ray_count=9U;
    wall.depth=64U;
    wall.resource_id=0x242U;
    wall.texture_column_index=7U;
    wall.shade_decrement=2U;
    frame.sweep_runs.push_back(wall);
    WorldSceneRecord scene{};
    scene.source_class=1U; // shipped Level 22 placed-object class
    scene.source_index=4U;
    scene.resource_id=445U;
    scene.screen_x=112U;
    scene.projected_extent=80U;
    scene.render_order=2U;
    scene.source_type_id=62U;
    frame.world_scene_records.push_back(scene);
    WorldSceneDrawCall draw{};
    draw.source_class=1U;
    draw.source_index=4U;
    draw.resource_id=445U;
    draw.screen_x=112U;
    draw.scale_extent=80U;
    frame.world_scene_draw_calls.push_back(draw);
    WorldSceneRenderPlan plan{};
    plan.call=draw;
    plan.source_unresolved=true;
    plan.visible_columns.push_back(Type7VisibleColumn{32U,0U,128U});
    frame.world_scene_render_plans.push_back(plan);
    frame.scene_plan_failures.push_back({
        0U,1U,4U,445U,"shape source unavailable"
    });
    frame.scene_vga_trace_failure="unsupported original scene descriptor";
    frame.scene_vga_scene_failures.push_back({
        0U,1U,4U,445U,"per-call RES3 shape lookup unavailable"
    });
    ExplorationPresentedFrame presentation{};
    presentation.palette.rgb[3U]=42U;
    presentation.surface_coverage.sintab_main_runs_pending=1U;
    presentation.surface_coverage.sintab_main_runs_failed=1U;
    presentation.surface_coverage.type7_calls_rasterized=1U;
    presentation.surface_coverage.type7_columns_rasterized=1U;
    presentation.surface_coverage.type7_indexed_writes=18U;
    presentation.surface_coverage.type7_calls_failed=1U;
    presentation.type7_scene_outcomes.push_back(
        Type7SceneRenderOutcome::PlanningFailed);
    presentation.type7_failures.push_back({
        445U,1U,4U,"unsupported original scene descriptor"
    });
    presentation.surface_main_failures.push_back({
        0x242U,3U,"unsupported original wide run"
    });
    constexpr std::size_t base=0x75F24U;
    std::vector<std::uint8_t> code2(base+0xC6A8U+64U,0U);
    const auto mask_index=(3U<<3U)+9U;
    code2[base+0xC668U+mask_index]=0x0FU;
    code2[base+0xC688U+mask_index]=0x05U;
    code2[base+0xC6A8U+mask_index]=0xF3U;

    ExplorationFrameCapture::write(
        dir,{1U,22U,44,34,127U},frame,presentation,image,code2);
    std::fprintf(stderr,"capture-test: exported native frame files\n");

    const auto basepath=dir/"frame_0001";
    std::ifstream indexed(basepath.string()+".idx",std::ios::binary);
    assert(indexed);
    const std::vector<std::uint8_t> bytes{
        std::istreambuf_iterator<char>(indexed),
        std::istreambuf_iterator<char>()};
    assert(bytes.size()==128000U);
    assert(bytes.front()==0x39U && bytes.back()==0x39U);
    std::fprintf(stderr,"capture-test: indexed pixels verified\n");
    std::ifstream pal(basepath.string()+".pal",std::ios::binary);
    assert(pal);
    const std::vector<std::uint8_t> palette{
        std::istreambuf_iterator<char>(pal),
        std::istreambuf_iterator<char>()};
    assert(palette.size()==768U && palette[3]==42U);
    std::fprintf(stderr,"capture-test: palette verified\n");
    std::ifstream metadata(basepath.string()+".tsv",std::ios::binary);
    assert(metadata);
    const std::string tsv{
        std::istreambuf_iterator<char>(metadata),
        std::istreambuf_iterator<char>()};
    assert(tsv.find("format\trloft-native-indexed-capture-v2")!=
           std::string::npos);
    assert(tsv.find("provenance\tnative-preview-not-original-DOS")!=
           std::string::npos);
    assert(tsv.find("source_revision\tlocal-unversioned")!=
           std::string::npos);
    assert(tsv.find("mask_origin\timmutable CODE.2 object-2")!=
           std::string::npos);
    assert(tsv.find("run\tmain\t3\t9\t3\t33\t15\t5\t243\t64\t578\t7\t2")!=
           std::string::npos);
    assert(tsv.find("failure\tmain\t578\t3\tunsupported original wide run")!=
           std::string::npos);
    assert(tsv.find("scene_records\t1")!=std::string::npos);
    assert(tsv.find("scene_draw_calls\t1")!=std::string::npos);
    assert(tsv.find("scene_render_plans\t1")!=std::string::npos);
    assert(tsv.find("scene_plan_failures\t1")!=std::string::npos);
    assert(tsv.find("scene_plan_failure\t0\t1\t4\t445\tshape source unavailable")!=
           std::string::npos);
    assert(tsv.find("scene_outcome\t0\t1\t4\t445\tplanning-failed")!=
           std::string::npos);
    assert(tsv.find("scene_video_calls\t1\t0\t1")!=std::string::npos);
    assert(tsv.find("scene_columns\t1\t0\t0")!=std::string::npos);
    assert(tsv.find("scene_indexed_writes\t18")!=std::string::npos);
    assert(tsv.find("scene_record\t1\t4\t445\t112\t80\t2\t62")!=
           std::string::npos);
    assert(tsv.find("scene_call\t1\t4\t445\t1\t80\tunresolved")!=
           std::string::npos);
    assert(tsv.find("scene_trace_failure\tunsupported original scene descriptor")!=
           std::string::npos);
    assert(tsv.find(
        "scene_call_trace_failure\t0\t1\t4\t445\tper-call RES3 shape lookup unavailable")!=
           std::string::npos);
    assert(tsv.find("scene_raster_failure\t1\t4\t445\tunsupported original scene descriptor")!=
           std::string::npos);
    std::fprintf(stderr,"capture-test: metadata verified\n");

    bool rejected=false;
    try {
        ExplorationFrameCapture::write(
            dir,{2U,22U,0,0,0U},frame,presentation,image,{});
    } catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
    assert(!fs::exists(dir/"frame_0002.idx"));
    std::fprintf(stderr,"capture-test: invalid-source rejection verified\n");

    // The new RES3 run telemetry must distinguish a long C029=1
    // source run from unsupported grouped raster-mask lookup.
    frame.scene_individual_columns_mode=true;
    frame.source_coordinate_a=0x00041234U;
    frame.source_coordinate_b=0x00054321U;
    frame.source_facing=261U;
    frame.world_scene_render_plans[0].source_unresolved=false;
    frame.world_scene_render_plans[0].visible_runs.push_back(
        Type7VisibleRun{32U,0U,128U,12U});
    presentation.type7_scene_outcomes[0]=
        Type7SceneRenderOutcome::TraceUnavailable;
    ExplorationFrameCapture::write(
        dir,{3U,22U,4,5,261U},frame,presentation,image,code2,
        "abcdef0123456789");
    std::ifstream third(dir/"frame_0003.tsv",std::ios::binary);
    assert(third);
    const std::string third_tsv{
        std::istreambuf_iterator<char>(third),
        std::istreambuf_iterator<char>()};
    assert(third_tsv.find("source_revision\tabcdef0123456789")!=
           std::string::npos);
    assert(third_tsv.find("scene_individual_columns_mode\t1")!=
           std::string::npos);
    assert(third_tsv.find("source_coordinate_fixed\t266804\t344865")!=
           std::string::npos);
    assert(third_tsv.find("source_facing_raw\t261")!=
           std::string::npos);
    assert(third_tsv.find("scene_run\t0\t32\t0\t128\t12\tindividual\tsupported")!=
           std::string::npos);
    assert(third_tsv.find("scene_outcome\t0\t1\t4\t445\ttrace-unavailable")!=
           std::string::npos);
    third.close();
    assert(!third.is_open());

    // Regression: the failure sidecar does not depend on successful
    // frame export or the 16 indexed-frame cap. Preserve the actual
    // failing pose and exception text, not the last rendered pose.
    ExplorationFrameCapture::write_failure(
        dir,{22U,1263210U,294912U,270U,30,15,16U,
             "abcdef0123456789",
             "first-person sweep exceeded guard (ray=42,\tstate=Left)\n"});
    std::ifstream failure_file(dir/"failure.tsv",std::ios::binary);
    assert(failure_file);
    const std::string failure_text{
        std::istreambuf_iterator<char>(failure_file),
        std::istreambuf_iterator<char>()};
    assert(failure_text.find(
        "format\trloft-native-exploration-failure-v1")!=std::string::npos);
    assert(failure_text.find(
        "source_revision\tabcdef0123456789")!=std::string::npos);
    assert(failure_text.find("coordinate_fixed\t1263210\t294912")!=
           std::string::npos);
    assert(failure_text.find("cell\t19\t4")!=std::string::npos);
    assert(failure_text.find("facing\t270")!=std::string::npos);
    assert(failure_text.find("prior_frames_written\t16")!=
           std::string::npos);
    assert(failure_text.find(
        "exception\tfirst-person sweep exceeded guard (ray=42, state=Left) ")
        !=std::string::npos);
    assert(!fs::exists(dir/"frame_0017.idx"));
    failure_file.close();
    assert(!failure_file.is_open());

    // Unlike POSIX, Win32 file deletion cannot unlink these captures
    // while ifstream handles remain open. End all three read sessions
    // BEFORE cleaning the temporary test directory. This is test-only
    // resource lifetime handling; the capture writer is unchanged.
    indexed.close();
    pal.close();
    metadata.close();
    assert(!indexed.is_open() && !pal.is_open() && !metadata.is_open());
    assert(fs::remove_all(dir) == 8U); // directory + 2*(idx + pal + tsv) + failure
    assert(!fs::exists(dir));
    std::fprintf(stderr,"capture-test: all streams closed and directory removed\n");
}
