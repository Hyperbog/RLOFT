#include "rloft/CINE04.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {
using rloft::Cine01CueKind;
using rloft::Cine04Edits;
using rloft::Cine04File;
using rloft::Cine04PlanarRun;

void require(bool condition, const char* description) {
    if (!condition) throw std::runtime_error(description);
}
template <class Action> void rejects(Action&& action) {
    bool failed = false;
    try { action(); } catch (const std::exception&) { failed = true; }
    require(failed, "CINE04 failed to reject a stale or invalid edit");
}
void push16(std::vector<std::uint8_t>& bytes, std::uint16_t n) {
    bytes.push_back(static_cast<std::uint8_t>(n));
    bytes.push_back(static_cast<std::uint8_t>(n >> 8U));
}
void push32(std::vector<std::uint8_t>& bytes, std::uint32_t n) {
    for (unsigned k = 0U; k < 4U; ++k)
        bytes.push_back(static_cast<std::uint8_t>(n >> (k * 8U)));
}
void add_chunk(std::vector<std::uint8_t>& bytes, std::uint16_t id,
               const std::vector<std::uint8_t>& payload) {
    push16(bytes, id);
    push32(bytes, static_cast<std::uint32_t>(payload.size()));
    bytes.insert(bytes.end(), payload.begin(), payload.end());
}
std::vector<std::uint8_t> solid_rle(std::uint8_t color) {
    std::vector<std::uint8_t> out;
    for (unsigned row = 0U; row < 400U; ++row)
        out.insert(out.end(), {0x81U,color,0x81U,color,0xC1U,color});
    return out;
}
std::vector<std::uint8_t> synthetic() {
    std::vector<std::uint8_t> out;
    push16(out,4U); push16(out,1U);
    push16(out,3U); // Frame 0: clear, DAC, full-frame.
    add_chunk(out,1U,{});
    add_chunk(out,0U,std::vector<std::uint8_t>(768U,6U));
    add_chunk(out,2U,solid_rle(9U));
    push16(out,2U); // Frame 1: planar + music.
    std::vector<std::uint8_t> pixels(320U*256U,17U);
    pixels[0U]=11U;
    pixels[4U]=12U;
    pixels[8U]=13U;
    pixels[320U]=14U;
    pixels[320U+3U]=22U;
    add_chunk(out,8U,rloft::Cine00File::encode_viewport(pixels));
    add_chunk(out,5U,{19U,0U,0U,0U});
    push16(out,2U); // Frame 2: another viewport + empty delta.
    add_chunk(out,8U,rloft::Cine00File::encode_viewport(
        std::vector<std::uint8_t>(320U*256U,42U)));
    add_chunk(out,3U,{0U,0U});
    push16(out,2U); // Frame 3: another DAC + sound.
    add_chunk(out,0U,std::vector<std::uint8_t>(768U,7U));
    add_chunk(out,9U,{35U,0U});
    return out;
}

void synthetic_test() {
    const auto original=synthetic();
    Cine04File file(original);
    require(file.encode()==original && file.stats().frames==4U &&
            file.stats().chunks==9U && file.stats().by_id[8U]==2U &&
            file.viewport_pixel(1U,0U,0U,0U)==11U &&
            file.viewport_pixel(1U,0U,3U,1U)==22U &&
            file.snapshot(2U).pixels[51U*320U]==42U,
            "CINE04 synthetic fixture/planar decoding failed");

    // Construct a different variant through the existing pixel-level codec.
    rloft::Cine03File target(original);
    rloft::Cine03Edits target_edits{};
    target_edits.pixels={
        {1U,0U,0U,0U,11U,63U}, {1U,0U,4U,0U,12U,64U},
        {1U,0U,8U,0U,13U,65U}, {1U,0U,0U,1U,14U,66U},
        {1U,0U,3U,1U,22U,67U}, {2U,0U,319U,255U,42U,4U}
    };
    target.apply(target_edits);
    Cine04File variant(target.encode());
    const auto spans=file.planar_runs_to(variant);
    std::size_t changed_bytes=0U;
    for (const auto& span : spans) {
        changed_bytes+=span.expected.size();
        require(span.expected.size()==span.replacement.size() &&
                span.plane < 4U && span.row < 256U &&
                span.byte_column+span.expected.size()<=80U,
                "CINE04 span crossed a native scanline or plane");
    }
    require(changed_bytes==6U && spans.size()==4U &&
            spans[0U].plane==0U && spans[0U].row==0U &&
            spans[0U].byte_column==0U && spans[0U].expected.size()==3U &&
            spans[0U].expected==std::vector<std::uint8_t>({11U,12U,13U}) &&
            spans[0U].replacement==std::vector<std::uint8_t>({63U,64U,65U}),
            "CINE04 contiguous byte runs do not align with VGA planes");
    require(variant.planar_runs_to(file).size()==4U &&
            file.planar_runs_to(Cine04File(original)).empty(),
            "CINE04 bidirectional/deterministic comparison failed");

    Cine04Edits edit{};
    edit.runs=spans;
    edit.base.base.palette={{0U,1U,7U,6U,8U}};
    edit.base.base.base.cues={{1U,1U,Cine01CueKind::MusicSequence,
                                19U,0U,20U,0U}};
    edit.base.base.base.packets.expected_wait=1U;
    edit.base.base.base.packets.replacement_wait=2U;
    file.apply(edit);
    require(file.payload(1U,0U)==variant.payload(1U,0U) &&
            file.payload(2U,0U)==variant.payload(2U,0U) &&
            file.snapshot(2U).pixels[306U*320U+319U]==4U &&
            file.stats().default_wait==2U &&
            file.payload(0U,1U)[7U]==8U && file.cues()[0U].first==20U,
            "CINE04 combined span/DAC/music/timing update failed");
    const auto checkpoint=file.encode();
    rejects([&]{file.apply(edit);}); // Source state has moved.
    Cine04Edits bad{};
    bad.runs={spans[0U]};
    rejects([&]{file.apply(bad);}); // Stale original bytes.
    auto undo_first=spans[0U];
    std::swap(undo_first.expected,undo_first.replacement);
    bad.runs={undo_first,undo_first};
    rejects([&]{file.apply(bad);}); // Overlapping addresses.
    bad.runs={undo_first};
    bad.runs[0U].byte_column=79U;
    rejects([&]{file.apply(bad);}); // Crosses native row boundary.
    bad.runs={undo_first}; bad.runs[0U].plane=4U;
    rejects([&]{file.apply(bad);});
    bad.runs={undo_first}; bad.runs[0U].row=256U;
    rejects([&]{file.apply(bad);});
    bad.runs={undo_first}; bad.runs[0U].expected.clear();
    rejects([&]{file.apply(bad);});
    bad.runs={undo_first}; bad.runs[0U].replacement.pop_back();
    rejects([&]{file.apply(bad);});
    bad.runs={undo_first}; bad.runs[0U].frame=99U;
    rejects([&]{file.apply(bad);});
    bad.runs={undo_first}; bad.runs[0U].frame=0U;
    bad.runs[0U].chunk=1U;
    rejects([&]{file.apply(bad);}); // Palette is not a viewport.
    bad.runs={undo_first};
    bad.base.pixels={{1U,0U,0U,0U,63U,12U}};
    rejects([&]{file.apply(bad);}); // Existing pixel API aliases span packet.
    bad.base.pixels.clear();
    bad.base.base.base.packets.chunks={{1U,0U,file.payload(1U,0U),
                                         variant.payload(1U,0U)}};
    rejects([&]{file.apply(bad);}); // Raw packet aliases span packet.
    bad.base.base.base.packets.chunks.clear();
    bad.base.base.palette={{0U,1U,7U,6U,9U}};
    rejects([&]{file.apply(bad);}); // Valid span but stale DAC.
    require(file.encode()==checkpoint,
            "CINE04 rejected edit modified live cinematic bytes");

    Cine04Edits inverse{};
    for (auto span : spans) {
        std::swap(span.expected,span.replacement);
        inverse.runs.push_back(std::move(span));
    }
    inverse.base.base.palette={{0U,1U,7U,8U,6U}};
    inverse.base.base.base.cues={{1U,1U,Cine01CueKind::MusicSequence,
                                   20U,0U,19U,0U}};
    inverse.base.base.base.packets.expected_wait=2U;
    inverse.base.base.base.packets.replacement_wait=1U;
    file.apply(inverse);
    require(file.encode()==original,
            "CINE04 synthetic exact inverse did not restore input");
    file.apply({});
    require(file.encode()==original,"CINE04 empty transaction changed input");
    Cine04File changed_palette(original);
    Cine04Edits palette_edit{};
    palette_edit.base.base.palette={{0U,1U,7U,6U,8U}};
    changed_palette.apply(palette_edit);
    rejects([&]{(void)file.planar_runs_to(changed_palette);});
    auto shortened=original; shortened.pop_back();
    rejects([&]{(void)Cine04File(shortened);});
}

std::vector<std::uint8_t> load(const char* name) {
    std::ifstream stream(name,std::ios::binary);
    if(!stream) throw std::runtime_error("CINE04 cannot open original fixture");
    return {std::istreambuf_iterator<char>(stream),
            std::istreambuf_iterator<char>()};
}
void retail_test(const std::vector<std::uint8_t>& original) {
    Cine04File file(original);
    require(original.size()==7498347U && file.encode()==original &&
            file.stats().frames==128U && file.stats().chunks==131U &&
            file.stats().default_wait==1U &&
            file.stats().by_id==std::array<std::size_t,10>{
                {24U,1U,1U,9U,0U,1U,0U,2U,91U,2U}},
            "CINE04 original bytes/structure do not match audited source");
    const auto cues=file.cues();
    require(cues.size()==5U && cues[0U].frame==1U && cues[0U].first==19U &&
            cues[1U].frame==7U && cues[1U].first==34U &&
            cues[2U].frame==8U && cues[2U].first==34U &&
            cues[3U].frame==76U && cues[3U].first==35U &&
            cues[4U].frame==82U && cues[4U].first==35U,
            "CINE04 original audio/subtitle identity differs");
    std::size_t alternate=0U;
    for (std::size_t i=0U;i<file.stats().frames;++i)
        alternate+=file.frame_info(i).uses_image_wait ? 1U : 0U;
    require(alternate==100U &&
            file.snapshot(127U).last_palette_frame==127U &&
            file.snapshot(127U).last_image_frame==116U,
            "CINE04 original timing or frame provenance differs");
    const auto pixel=file.viewport_pixel(9U,0U,0U,0U);
    const auto palette=file.payload(0U,1U);
    const std::uint8_t changed=static_cast<std::uint8_t>(pixel ^ 1U);
    const std::uint8_t changed_dac=static_cast<std::uint8_t>(palette[6U] ^ 1U);
    Cine04Edits edit{};
    edit.runs={{9U,0U,0U,0U,0U,{pixel},{changed}}};
    edit.base.base.palette={{0U,1U,6U,palette[6U],changed_dac}};
    edit.base.base.base.cues={{cues[0U].frame,cues[0U].chunk,cues[0U].kind,
                               cues[0U].first,cues[0U].second,20U,0U}};
    edit.base.base.base.packets.expected_wait=1U;
    edit.base.base.base.packets.replacement_wait=2U;
    file.apply(edit);
    require(file.viewport_pixel(9U,0U,0U,0U)==changed &&
            file.payload(0U,1U)[6U]==changed_dac &&
            file.cues()[0U].first==20U && file.stats().default_wait==2U,
            "CINE04 original-file transaction did not commit");
    const auto checkpoint=file.encode();
    rejects([&]{file.apply(edit);});
    require(file.encode()==checkpoint,
            "CINE04 original-file stale edit was not atomic");
    Cine04Edits inverse{};
    inverse.runs={{9U,0U,0U,0U,0U,{changed},{pixel}}};
    inverse.base.base.palette={{0U,1U,6U,changed_dac,palette[6U]}};
    inverse.base.base.base.cues={{cues[0U].frame,cues[0U].chunk,cues[0U].kind,
                                  20U,0U,cues[0U].first,cues[0U].second}};
    inverse.base.base.base.packets.expected_wait=2U;
    inverse.base.base.base.packets.replacement_wait=1U;
    file.apply(inverse);
    require(file.encode()==original,
            "CINE04 original-file inverse did not restore all bytes");
}
void sibling_test(const std::vector<std::uint8_t>& from,
                  const std::vector<std::uint8_t>& to,
                  std::size_t expected_bytes, std::size_t expected_runs) {
    Cine04File source(from),target(to);
    const auto runs=source.planar_runs_to(target);
    std::size_t total=0U;
    std::set<std::pair<std::size_t,std::size_t>> packets;
    std::size_t first=128U,last=0U;
    for (const auto& run : runs) {
        total+=run.expected.size();
        packets.emplace(run.frame,run.chunk);
        first=std::min(first,run.frame);last=std::max(last,run.frame);
        require(source.info(run.frame,run.chunk).id==8U &&
                run.expected.size()==run.replacement.size() &&
                !run.expected.empty() && run.byte_column+run.expected.size()<=80U,
                "CINE04 original sibling run is not planar or source-backed");
        for (std::size_t i=0;i<run.expected.size();++i) {
            const auto x=static_cast<std::uint16_t>(
                (run.byte_column+i)*4U+run.plane);
            require(source.viewport_pixel(run.frame,run.chunk,x,run.row)==
                        run.expected[i] &&
                    target.viewport_pixel(run.frame,run.chunk,x,run.row)==
                        run.replacement[i],
                    "CINE04 sibling diff source bytes do not match pixels");
        }
    }
    require(total==expected_bytes && runs.size()==expected_runs &&
            packets.size()==35U && first==75U && last==116U,
            "CINE04 original sibling change counts differ");
    Cine04Edits apply{};apply.runs=runs;
    source.apply(apply);
    require(source.encode()==to && source.planar_runs_to(target).empty(),
            "CINE04 sibling runs did not reproduce target byte-for-byte");
    const auto checkpoint=source.encode();
    rejects([&]{source.apply(apply);});
    require(source.encode()==checkpoint,
            "CINE04 stale sibling edit modified target");
    Cine04Edits restore{};
    for (auto run : runs) {
        std::swap(run.expected,run.replacement);
        restore.runs.push_back(std::move(run));
    }
    source.apply(restore);
    require(source.encode()==from,
            "CINE04 sibling run undo did not reproduce original byte-for-byte");
}
} // namespace

int main(int argc,char** argv) {
    synthetic_test();
    if (argc != 1 && argc != 2 && argc != 4)
        throw std::invalid_argument(
            "usage: CINE04_tests [CINE04.ANM [CINE03.ANM CINE02.ANM]]");
    if (argc>=2) {
        const auto cine04=load(argv[1]);
        retail_test(cine04);
        if (argc==4) {
            sibling_test(load(argv[2]),cine04,1840U,1364U);
            sibling_test(load(argv[3]),cine04,4374U,2886U);
        }
    }
}
