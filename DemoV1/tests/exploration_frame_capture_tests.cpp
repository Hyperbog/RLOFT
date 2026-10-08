#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/exploration_frame_capture.hpp"

#include <cassert>
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
    ExplorationPresentedFrame presentation{};
    presentation.palette.rgb[3U]=42U;
    presentation.surface_coverage.sintab_main_runs_pending=1U;
    presentation.surface_coverage.sintab_main_runs_failed=1U;
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

    const auto basepath=dir/"frame_0001";
    std::ifstream indexed(basepath.string()+".idx",std::ios::binary);
    assert(indexed);
    const std::vector<std::uint8_t> bytes{
        std::istreambuf_iterator<char>(indexed),
        std::istreambuf_iterator<char>()};
    assert(bytes.size()==128000U);
    assert(bytes.front()==0x39U && bytes.back()==0x39U);
    std::ifstream pal(basepath.string()+".pal",std::ios::binary);
    assert(pal);
    const std::vector<std::uint8_t> palette{
        std::istreambuf_iterator<char>(pal),
        std::istreambuf_iterator<char>()};
    assert(palette.size()==768U && palette[3]==42U);
    std::ifstream metadata(basepath.string()+".tsv",std::ios::binary);
    assert(metadata);
    const std::string tsv{
        std::istreambuf_iterator<char>(metadata),
        std::istreambuf_iterator<char>()};
    assert(tsv.find("provenance\tnative-preview-not-original-DOS")!=
           std::string::npos);
    assert(tsv.find("mask_origin\timmutable CODE.2 object-2")!=
           std::string::npos);
    assert(tsv.find("run\tmain\t3\t9\t3\t33\t15\t5\t243\t64\t578\t7\t2")!=
           std::string::npos);
    assert(tsv.find("failure\tmain\t578\t3\tunsupported original wide run")!=
           std::string::npos);

    bool rejected=false;
    try {
        ExplorationFrameCapture::write(
            dir,{2U,22U,0,0,0U},frame,presentation,image,{});
    } catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
    assert(!fs::exists(dir/"frame_0002.idx"));
    fs::remove_all(dir);
}
