#include "rloft/CINE08.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {
using rloft::Cine00File;
using rloft::Cine07File;
using rloft::Cine07Edits;
using rloft::Cine08File;

void require(bool result, const char* msg) {
    if (!result) throw std::runtime_error(msg);
}
template<class Action> void rejects(Action&& action) {
    bool failed = false;
    try { action(); } catch (const std::exception&) { failed = true; }
    require(failed, "CINE08 accepted an invalid transaction or segment");
}
void push16(std::vector<std::uint8_t>& b, std::uint16_t v) {
    b.push_back(static_cast<std::uint8_t>(v));
    b.push_back(static_cast<std::uint8_t>(v >> 8U));
}
void push32(std::vector<std::uint8_t>& b, std::uint32_t v) {
    for (unsigned s = 0U; s < 32U; s += 8U)
        b.push_back(static_cast<std::uint8_t>(v >> s));
}
void packet(std::vector<std::uint8_t>& b, std::uint16_t id,
            const std::vector<std::uint8_t>& p) {
    push16(b,id); push32(b,static_cast<std::uint32_t>(p.size()));
    b.insert(b.end(),p.begin(),p.end());
}
std::vector<std::uint8_t> fixture() {
    std::vector<std::uint8_t> out;
    push16(out,7U); push16(out,1U);
    push16(out,3U); // Frame 0: blackout, palette, full screen.
    packet(out,1U,{});
    packet(out,0U,std::vector<std::uint8_t>(768U,19U));
    std::vector<std::uint8_t> pixels(320U*400U);
    for (std::size_t i=0U;i<pixels.size();++i)
        pixels[i]=static_cast<std::uint8_t>((i*17U+7U)&255U);
    packet(out,2U,Cine08File::encode_full_frame(pixels));
    push16(out,2U); // Frame 1: subtitle and music.
    packet(out,7U,{37U,0U,5U,0U});
    packet(out,5U,{13U,0U,0U,0U});
    push16(out,2U); // Frame 2: native planar image and sound.
    std::vector<std::uint8_t> planar(320U*256U,39U);
    for (std::size_t i=0U;i<planar.size();i+=17U) planar[i]=52U;
    packet(out,8U,Cine00File::encode_viewport(planar));
    packet(out,9U,{35U,0U});
    push16(out,1U); // Frame 3: nonempty sparse-delta update.
    packet(out,3U,{1U,0U,3U,0U,1U,0U,4U,0U,3U,0U,70U,71U,72U});
    push16(out,1U); // Frame 4: palette change, subtitle retained.
    packet(out,0U,std::vector<std::uint8_t>(768U,31U));
    push16(out,2U); // Frame 5: subtitle clear, queued sound.
    packet(out,7U,{0U,0U,0U,0U});
    packet(out,6U,{8U,0U});
    push16(out,2U); // Frame 6: new subtitle and image.
    packet(out,7U,{40U,0U,5U,0U});
    packet(out,8U,Cine00File::encode_viewport(
        std::vector<std::uint8_t>(320U*256U,55U)));
    return out;
}

void verify_segment(const Cine08File& source, std::size_t first,
                    std::size_t last, std::uint16_t image_wait = 6U) {
    const auto info=source.segment_info(first,last);
    require(info.first==first && info.last==last &&
            info.original_start_tick==source.frame_index(first).start_tick,
            "CINE08 segment original boundary metadata mismatch");
    const auto output=source.extract_visual_segment(first,last);
    Cine07File extracted(output,image_wait);
    require(extracted.stats().frames==last-first &&
            extracted.stats().default_wait==source.stats().default_wait &&
            extracted.total_ticks()==info.duration_ticks,
            "CINE08 segment changes original frame count or timing");
    const Cine07File original(source.encode(),image_wait);
    if (first==0U) {
        require(info.prefix_packets==0U && !info.restored_subtitle &&
                output==original.extract_range(first,last),
                "CINE08 first-frame extraction must stay byte-identical");
    } else {
        const auto before=original.snapshot(first-1U);
        const auto original_first=original.frame(first);
        const auto extracted_first=extracted.frame(0U);
        const auto prefix=info.prefix_packets;
        require(prefix==(info.restored_subtitle?3U:2U) &&
                extracted_first.packets.size()==original_first.packets.size()+prefix &&
                extracted_first.packets[0U].id==0U &&
                extracted_first.packets[1U].id==2U &&
                extracted_first.packets[0U].payload==
                    std::vector<std::uint8_t>(before.palette.begin(),before.palette.end()),
                "CINE08 initial palette or prefix packet count differs");
        Cine00File raw(output);
        require(raw.full_frame(0U,1U)==before.pixels &&
                extracted_first.packets[1U].payload.size()==129200U,
                "CINE08 literal row-RLE does not reproduce boundary image");
        if (info.restored_subtitle) {
            require(extracted_first.packets[2U].id==7U &&
                    extracted_first.packets[2U].payload.size()==4U &&
                    (extracted_first.packets[2U].payload[0U] ||
                     extracted_first.packets[2U].payload[1U]),
                    "CINE08 did not reconstruct active subtitle command");
        }
        for (std::size_t i=0U;i<original_first.packets.size();++i) {
            const auto& a=original_first.packets[i];
            const auto& b=extracted_first.packets[i+prefix];
            require(a.id==b.id && a.payload==b.payload,
                    "CINE08 modified an original first-frame packet");
        }
        require(extracted.frame_bytes(1U,last-first)==
                original.frame_bytes(first+1U,last),
                "CINE08 modified a subsequent original serialized frame");
    }
    for (std::size_t f=first;f<last;++f) {
        const auto expected=original.snapshot(f);
        const auto actual=extracted.snapshot(f-first);
        require(expected.palette==actual.palette &&
                expected.pixels==actual.pixels &&
                expected.wait_ticks==actual.wait_ticks,
                "CINE08 segment differs from original image/palette timeline");
    }
    // Extra subtitle priming must not emit additional music/SFX packets.
    for (std::uint16_t audio_id:{5U,6U,9U}) {
        std::size_t expected=0U,actual=0U;
        for (std::size_t f=first;f<last;++f)
            for (const auto& p:original.frame(f).packets)
                expected+=(p.id==audio_id?1U:0U);
        for (std::size_t f=0U;f<extracted.stats().frames;++f)
            for (const auto& p:extracted.frame(f).packets)
                actual+=(p.id==audio_id?1U:0U);
        require(expected==actual,"CINE08 extraction inserted extra sound/music command");
    }
}

void exercise(const std::vector<std::uint8_t>& bytes, bool original) {
    Cine08File file(bytes);
    const auto old=file.stats();
    require(file.encode()==bytes && file.bytes()==bytes,
            "CINE08 constructor failed byte-exact original preservation");
    if (original) {
        require(bytes.size()==9255472U && old.frames==182U &&
                old.chunks==184U && old.default_wait==1U &&
                old.by_id==std::array<std::size_t,10>{
                    {11U,1U,1U,49U,0U,0U,1U,5U,112U,4U}} &&
                file.total_ticks()==987U,
                "CINE08 uploaded original structure differs from source research");
        for (const auto& pair:std::vector<std::pair<std::size_t,std::size_t>>{
            {0U,1U},{0U,182U},{1U,2U},{7U,9U},{38U,40U},
            {99U,101U},{140U,142U},{180U,182U}})
            verify_segment(file,pair.first,pair.second);
    } else {
        require(old.frames==7U && old.chunks==13U &&
                file.total_ticks()==22U,
                "CINE08 synthetic timing/inventory differs");
        for (std::size_t f=0U;f<7U;++f)
            for (std::size_t last=f+1U;last<=7U;++last)
                verify_segment(file,f,last);
        require(file.segment_info(3U,4U).restored_subtitle &&
                !file.segment_info(6U,7U).restored_subtitle,
                "CINE08 subtitle boundary tracking differs");
    }
    require(file.encode()==bytes,"CINE08 extraction mutated original bytes");
    rejects([&]{(void)file.extract_visual_segment(0U,0U);});
    rejects([&]{(void)file.extract_visual_segment(old.frames,old.frames);});
    rejects([&]{(void)file.extract_visual_segment(5U,2U);});
    rejects([&]{(void)file.extract_visual_segment(0U,old.frames+1U);});
    rejects([&]{(void)Cine08File::encode_full_frame({1U,2U});});

    // Existing atomic frame-range editor remains functional through facade.
    Cine07File base(bytes);
    const auto idx=original?8U:2U;
    auto frame=base.frame(idx);
    std::size_t modified=0U;
    for(auto& p:frame.packets)if(p.id==8U){p.payload[0U]^=7U;modified++;break;}
    require(modified==1U,"CINE08 fixture has no sample planar image to edit");
    Cine07Edits edits{};
    edits.expected_frame_count=old.frames;
    edits.ranges={{idx,idx+1U,base.frame_bytes(idx,idx+1U),{frame}}};
    file.apply(edits);
    require(file.encode()!=bytes && file.stats().frames==old.frames,
            "CINE08 inherited frame range edit failed");
    const auto checkpoint=file.encode();
    rejects([&]{file.apply(edits);});
    require(file.encode()==checkpoint,"CINE08 rejected stale edit mutated bytes");
    Cine07File changed(checkpoint);
    Cine07Edits undo{};
    undo.expected_frame_count=old.frames;
    undo.ranges={{idx,idx+1U,changed.frame_bytes(idx,idx+1U),
                  {base.frame(idx)}}};
    file.apply(undo);
    require(file.encode()==bytes,"CINE08 inherited edit did not undo byte-exactly");
}
} // namespace

int main(int argc,char** argv) {
    if(argc>2)throw std::invalid_argument("usage: CINE08_tests [CINE08.ANM]");
    exercise(fixture(),false);
    if(argc==2) {
        std::ifstream in(argv[1],std::ios::binary);
        if(!in)throw std::runtime_error("cannot open CINE08 original");
        const std::vector<std::uint8_t> bytes{
            std::istreambuf_iterator<char>(in),std::istreambuf_iterator<char>()};
        exercise(bytes,true);
    }
}
