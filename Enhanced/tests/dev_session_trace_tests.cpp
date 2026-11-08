#ifdef NDEBUG
#undef NDEBUG
#endif

#include "rloft/dev_session_trace.hpp"

#include <cassert>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>

namespace {
std::string read(const std::filesystem::path& p) {
    std::ifstream stream(p,std::ios::binary);
    return {std::istreambuf_iterator<char>(stream),
            std::istreambuf_iterator<char>()};
}
} // namespace

int main() {
    namespace fs=std::filesystem;
    const auto tag=std::to_string(
        std::chrono::steady_clock::now().time_since_epoch().count());
    const auto root=fs::temp_directory_path()/("rloft_dev_trace_test_"+tag);
    fs::remove_all(root);

    {
        rloft::DevSessionTrace trace(root,"source-revision-synthetic","loose");
        rloft::IndexedFramebuffer framebuffer{};
        framebuffer.set_draw_audit(
            [](void* context,const rloft::IndexedDrawAuditCall& draw) {
                static_cast<rloft::DevSessionTrace*>(
                    context)->indexed_draw(draw);
            },&trace);
        framebuffer.clear(13U);
        // Use the same source-neutral type-1 framebuffer entry point
        // as the real party portrait/equipment renderers.
        rloft::Type1Raster sprite{1,2,2,0xFFU,{1U,2U,3U,4U}};
        framebuffer.draw_frame(sprite,0,23,45);
        rloft::Palette6 palette{};
        palette.rgb[2]=42U;
        rloft::DevLevelSnapshot state{};
        state.level_id=22U;
        state.coordinate_a=0x00048000U;
        state.coordinate_b=0x00048000U;
        state.facing=270U;
        state.camera_hash=7U;
        state.party_hash=10U;
        state.inventory_hash=20U;
        state.world_hash=30U;
        state.flags_hash=40U;
        trace.observe("viewport",framebuffer,palette,state,100U);
        assert(trace.snapshot_count()==1U);
        assert(trace.marker_count()==0U);
        assert(fs::file_size(root/"frame_0001.idx")==128000U);
        assert(fs::file_size(root/"frame_0001.pal")==768U);
        assert(read(root/"frame_0001.idx")[1]==13);
        assert(static_cast<unsigned char>(
            read(root/"frame_0001.pal")[2])==42U);
        assert(read(root/"frame_0001.json").find(
            "\"screen\":\"viewport\"")!=std::string::npos);
        const auto draw_log=read(root/"frame_0001.draws.tsv");
        assert(draw_log.find("clear")!=std::string::npos);
        assert(draw_log.find("type1.frame\t0\t45\t23\t2\t2")
               !=std::string::npos);
        assert(draw_log.find("omitted\t0")!=std::string::npos);
        assert(read(root/"manifest.json").find(
            "source-revision-synthetic")!=std::string::npos);

        trace.observe("viewport",framebuffer,palette,state,101U);
        assert(trace.snapshot_count()==1U);
        trace.event("mouse.left.down",45,60);
        trace.observe("inventory",framebuffer,palette,state,102U);
        assert(trace.snapshot_count()==2U);
        assert(read(root/"frame_0002.json").find(
            "\"screen\":\"inventory\"")!=std::string::npos);
        state.inventory_hash=21U;
        trace.observe("inventory",framebuffer,palette,state,113U);
        assert(trace.snapshot_count()==3U);

        trace.request_marker();
        trace.observe("inventory",framebuffer,palette,state,114U);
        assert(trace.marker_count()==1U);
        assert(trace.snapshot_count()==3U);
        assert(fs::file_size(root/"marker_0001.idx")==128000U);
        assert(fs::file_size(root/"marker_0001.pal")==768U);
        assert(read(root/"marker_0001_recent.jsonl").find(
            "f12.marker.request")!=std::string::npos);
        assert(read(root/"marker_0001.json").find(
            "\"irq\":114")!=std::string::npos);

        trace.event("quote\"line\n",1,2);
        trace.observe("automap",framebuffer,palette,state,105U);
        assert(read(root/"events.jsonl").find(
            "quote\\\"line\\n")!=std::string::npos);
        for(std::size_t i=1U;i<rloft::DevSessionTrace::kMaxMarkers+4U;++i) {
            trace.request_marker();
            trace.observe("automap",framebuffer,palette,state,
                          static_cast<std::uint32_t>(105U+i));
        }
        assert(trace.marker_count()==rloft::DevSessionTrace::kMaxMarkers);
        assert(!fs::exists(root/"marker_0017.idx"));

        for(std::size_t i=0;i<200U;++i) {
            state.camera_hash=1000U+i;
            trace.observe("viewport",framebuffer,palette,state,
                          static_cast<std::uint32_t>(200U+i*10U));
        }
        // 64 viewport/hover screenshots maximum; retain 32 slots
        // for later inventory/automap/quit modals even on long sessions.
        assert(trace.snapshot_count()==
               rloft::DevSessionTrace::kMaxViewportSnapshots+3U);
        assert(!fs::exists(root/"frame_0068.idx"));
        // F12 marker slots remain available independent of auto budget.
        // Source draw metadata is bounded independently on EVERY frame.
        for(std::size_t i=0U;
            i<rloft::DevSessionTrace::kMaxDrawsPerPresent+6U;++i) {
            trace.indexed_draw({
                "type1.frame",0,3,4,16,12,0xFF,1});
        }
        trace.observe("quit_confirm",framebuffer,palette,state,3000U);
        assert(read(root/"frame_0068.draws.tsv").find(
            "omitted\t6")!=std::string::npos);
        assert(trace.snapshot_count()==
               rloft::DevSessionTrace::kMaxViewportSnapshots+4U);
        trace.error("build_frame","synthetic-error");
        // A source bank fault can happen before a frame is presented:
        // it must survive the normal session destructor/finish path.
        trace.error("party_modal.draw",
                    "Demo v1 has no loaded type-1 archive for requested bank");
        assert(read(root/"events.jsonl").find(
            "\"stage\":\"party_modal.draw\"")!=std::string::npos);
        assert(read(root/"events.jsonl").find(
            "no loaded type-1 archive")!=std::string::npos);
        trace.finish();
        assert(read(root/"events.jsonl").find(
            "\"event\":\"session.finish\"")!=std::string::npos);
        assert(read(root/"events.jsonl").find(
            "synthetic-error")!=std::string::npos);
    }
    fs::remove_all(root);
}
