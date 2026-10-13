#include "rloft_native/native_media.hpp"
#include "rloft_native/native_startup_visuals.hpp"
#include "rloft/anm_cinematic.hpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
namespace {
std::uint64_t hash_frame(const std::vector<std::uint8_t>& frame) {
    std::uint64_t h=14695981039346656037ULL;
    for(auto b:frame){h^=b;h*=1099511628211ULL;}
    return h;
}
struct Scan {
    int id{};
    unsigned frames{};
    unsigned commands{};
    unsigned subtitle_frames{};
    unsigned distinct_frames{};
    std::uint64_t composite_hash{};
};
Scan scan(int id, const rloft_native::NativeMedia& original,
          const rloft::StartupVisualSource& visuals) {
    const auto name=id==0?"CINE00.ANM":"CINE12.ANM";
    rloft::AnmCinematic anim(original.read(name),id,visuals);
    Scan result{};
    result.id=id;
    std::uint64_t previous=0;
    result.frames=unsigned(anim.frame_count());
    if(result.frames==0||result.frames>10000)
        throw std::runtime_error(std::string(name)+" has invalid frame count");
    anim.reset();
    while(!anim.finished()) {
        const auto frame=anim.render_next();
        const auto& raster=anim.framebuffer().pixels();
        if(raster.size()!=320U*400U)
            throw std::runtime_error("original ANM not 320x400");
        if(frame.index<0||unsigned(frame.index)>=result.frames||
           frame.wait_ticks<0||frame.wait_ticks>32767)
            throw std::runtime_error("original ANM timing index invalid");
        const auto hash=hash_frame(raster);
        if(frame.index==0||hash!=previous)++result.distinct_frames;
        previous=hash;
        result.composite_hash^=hash+0x9E3779B97F4A7C15ULL+
                               (result.composite_hash<<6U)+
                               (result.composite_hash>>2U);
        result.commands+=unsigned(frame.audio.size());
    }
    return result;
}
}
int main(int argc,char** argv) {
    if(argc!=2) {
        std::cerr<<"usage: rloft_native_anm_audit <original-CD-media-dir-or-ISO>\n";
        return 2;
    }
    try {
        const auto media=rloft_native::NativeMedia::open(argv[1]);
        if(!media.contains("CINE00.ANM")||!media.contains("CINE12.ANM"))
            throw std::runtime_error("CD original CINE00.ANM and CINE12.ANM required");
        rloft_native::NativeStartupVisualSource visuals(media);
        const auto a=scan(0,media,visuals);
        const auto b=scan(12,media,visuals);
        std::cout<<"{\"schema\":\"native-original-cinematic-v1\","
                 <<"\"cine00_frames\":"<<a.frames<<","
                 <<"\"cine12_frames\":"<<b.frames<<","
                 <<"\"cine00_distinct\":"<<a.distinct_frames<<","
                 <<"\"cine12_distinct\":"<<b.distinct_frames<<","
                 <<"\"audio_commands_source_traced\":"
                 <<(a.commands+b.commands)<<","
                 <<"\"audio_playback_integrated\":false,"
                 <<"\"frame_hashes\":["<<a.composite_hash<<","
                 <<b.composite_hash<<"],"
                 <<"\"original_media_writes\":0,"
                 <<"\"dos_frame_parity_claim\":false}\n";
    } catch(const std::exception& e) {
        std::cerr<<"ANM source audit failure: "<<e.what()<<'\n';
        return 1;
    }
}
