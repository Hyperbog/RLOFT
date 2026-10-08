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
    const std::vector<std::uint8_t>& code2) {
    const auto& pixels=framebuffer.pixels();
    constexpr std::size_t kPixelCount=
        static_cast<std::size_t>(IndexedFramebuffer::kWidth)*
        IndexedFramebuffer::kHeight;
    if (pixels.size()!=kPixelCount ||
        code2.size()<kCode2DgroupBase+0xC6C8U+1U) {
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
    manifest<<"format\trloft-native-indexed-capture-v1\n"
            <<"provenance\tnative-preview-not-original-DOS\n"
            <<"dimensions\t320\t400\n"
            <<"palette\t256x3\tVGA-6bit-raw\n"
            <<"level\t"<<position.level<<"\n"
            <<"cell\t"<<position.cell_x<<"\t"<<position.cell_y<<"\n"
            <<"facing\t"<<position.facing<<"\n"
            <<"scene_video_page\t"<<unsigned(runtime.scene_video_page)<<"\n"
            <<"main_runs\t"<<coverage.sintab_main_runs_rasterized
            <<"\t"<<coverage.sintab_main_runs_pending
            <<"\t"<<coverage.sintab_main_runs_failed<<"\n"
            <<"prepass_runs\t"<<coverage.sintab_prepass_runs_rasterized
            <<"\t"<<coverage.sintab_prepass_runs_pending
            <<"\t"<<coverage.sintab_prepass_runs_failed<<"\n"
            <<"mask_origin\timmutable CODE.2 object-2, not DOS live RAM\n"
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

} // namespace rloft
