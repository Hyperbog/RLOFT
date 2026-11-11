#include "rloft/CINE09.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {
using rloft::Cine07Edits;
using rloft::Cine09File;

void require(bool valid, const char* message) {
    if (!valid) throw std::runtime_error(message);
}
template<class Action> void rejects(Action&& action) {
    bool rejected = false;
    try { action(); } catch (const std::exception&) { rejected = true; }
    require(rejected, "CINE09 failed to reject malformed or out-of-range export");
}
void put16(std::vector<std::uint8_t>& b, std::uint16_t v) {
    b.push_back(static_cast<std::uint8_t>(v));
    b.push_back(static_cast<std::uint8_t>(v >> 8U));
}
void put32(std::vector<std::uint8_t>& b, std::uint32_t v) {
    for (unsigned i=0U; i<4U; ++i)
        b.push_back(static_cast<std::uint8_t>(v >> (8U*i)));
}
std::uint16_t get16(const std::vector<std::uint8_t>& b, std::size_t at) {
    return static_cast<std::uint16_t>(b.at(at) |
        (static_cast<unsigned>(b.at(at + 1U)) << 8U));
}
std::uint32_t get32(const std::vector<std::uint8_t>& b, std::size_t at) {
    return static_cast<std::uint32_t>(b.at(at)) |
        (static_cast<std::uint32_t>(b.at(at+1U))<<8U) |
        (static_cast<std::uint32_t>(b.at(at+2U))<<16U) |
        (static_cast<std::uint32_t>(b.at(at+3U))<<24U);
}
void add(std::vector<std::uint8_t>& out, std::uint16_t type,
         const std::vector<std::uint8_t>& payload) {
    put16(out, type); put32(out, static_cast<std::uint32_t>(payload.size()));
    out.insert(out.end(),payload.begin(),payload.end());
}
std::vector<std::uint8_t> solid_rle(std::uint8_t pixel) {
    std::vector<std::uint8_t> b;
    for (std::size_t row=0U; row<400U; ++row)
        b.insert(b.end(),{0x81U,pixel,0x81U,pixel,0xC1U,pixel});
    return b;
}
std::vector<std::uint8_t> initial_palette() {
    std::vector<std::uint8_t> palette(768U);
    for (std::size_t i=0U; i<256U; ++i) {
        palette[i*3U]=31U;
        palette[i*3U+1U]=16U;
        palette[i*3U+2U]=63U;
    }
    return palette;
}
std::vector<std::uint8_t> synthetic() {
    std::vector<std::uint8_t> b;
    put16(b,4U);put16(b,1U);
    put16(b,3U);
    add(b,1U,{});
    add(b,0U,initial_palette());
    add(b,2U,solid_rle(17U));
    put16(b,2U);
    add(b,8U,rloft::Cine00File::encode_viewport(
        std::vector<std::uint8_t>(320U*256U,29U)));
    add(b,7U,{30U,0U,11U,0U});
    put16(b,3U);
    add(b,3U,{1U,0U, 4U,0U,1U,0U, 0U,0U,2U,0U, 99U,100U});
    add(b,9U,{30U,0U});
    add(b,6U,{19U,0U});
    put16(b,3U);
    add(b,3U,{0U,0U});
    add(b,0U,std::vector<std::uint8_t>(768U,0U));
    add(b,7U,{0U,0U,0U,0U});
    return b;
}
void check_bmp(const std::vector<std::uint8_t>& bmp) {
    require(bmp.size() == 129078U && bmp.at(0U)=='B' && bmp.at(1U)=='M' &&
            get32(bmp,2U)==bmp.size() && get32(bmp,10U)==1078U &&
            get32(bmp,14U)==40U && get32(bmp,18U)==320U &&
            get32(bmp,22U)==400U && get16(bmp,26U)==1U &&
            get16(bmp,28U)==8U && get32(bmp,30U)==0U &&
            get32(bmp,34U)==128000U && get32(bmp,46U)==256U,
            "CINE09 BMP header or native geometry is invalid");
}
std::size_t bmp_pixel(std::size_t x, std::size_t y) {
    return 1078U + (399U-y)*320U+x;
}
void test_synthetic() {
    const auto original=synthetic();
    Cine09File file(original);
    require(file.encode()==original && file.stats().frames==4U &&
            file.stats().chunks==11U && file.total_ticks()==19U,
            "CINE09 synthetic source and timing mismatch");
    const auto s0=file.sample_frame(0U),s1=file.sample_tick(1U);
    const auto s2=file.sample_tick(7U),s3=file.sample_tick(13U);
    require(s0.duration_ticks==1U && s0.start_tick==0U &&
            s0.pixels.size()==128000U && s0.pixels[51U*320U]==17U &&
            s1.frame==1U && s1.duration_ticks==6U &&
            s1.pixels[51U*320U]==29U && s1.pixels[0U]==17U &&
            s1.active_subtitle_frame==1U &&
            s2.frame==2U && s2.pixels[4U]==99U &&
            s2.pixels[320U+4U]==100U &&
            s2.pixels[51U*320U]==29U &&
            s3.frame==3U && !s3.active_subtitle_frame &&
            s3.dac6[0U]==0U && s3.pixels[4U]==99U,
            "CINE09 synthetic snapshots, deltas or subtitle provenance differ");
    const auto cues=file.timed_cues();
    require(cues.size()==4U && cues[0U].start_tick==1U &&
            cues[0U].command.first==30U && cues[0U].command.second==11U &&
            cues[1U].start_tick==7U && cues[1U].command.first==30U &&
            cues[2U].command.first==19U &&
            cues[3U].start_tick==13U && cues[3U].subtitle_clear &&
            file.cue_csv().find("1,1,1,6,7,30,11,0,0\n")!=std::string::npos,
            "CINE09 source-tick cue mapping differs");

    const auto bmp0=file.bmp_at_frame(0U),bmp1=file.bmp_at_tick(1U);
    check_bmp(bmp0);check_bmp(bmp1);
    // BGRA palette entry 0: original DAC [31,16,63] maps to [255,65,125].
    require(bmp0[54U]==255U && bmp0[55U]==65U &&
            bmp0[56U]==125U && bmp0[57U]==0U &&
            bmp0[bmp_pixel(0U,0U)]==17U &&
            bmp1[bmp_pixel(0U,0U)]==17U &&
            bmp1[bmp_pixel(0U,51U)]==29U &&
            file.bmp_at_frame(3U)[54U]==0U,
            "CINE09 source DAC6 conversion, BMP row order or pixels differ");
    rejects([&]{(void)file.sample_frame(4U);});
    rejects([&]{(void)file.sample_tick(19U);});
    rejects([&]{(void)file.bmp_at_tick(19U);});
    auto corrupt=s0;corrupt.dac6[0U]=64U;
    rejects([&]{(void)Cine09File::encode_indexed_bmp(corrupt);});
    corrupt=s0;corrupt.pixels.pop_back();
    rejects([&]{(void)Cine09File::encode_indexed_bmp(corrupt);});

    Cine07Edits change{};
    change.expected_wait=1U;change.replacement_wait=2U;
    file.apply(change);
    require(file.stats().default_wait==2U && file.total_ticks()==20U &&
            file.bmp_at_frame(1U)==bmp1,
            "CINE09 synthetic time edit affected image pixels");
    const auto after=file.encode();
    rejects([&]{file.apply(change);});
    require(file.encode()==after,"CINE09 failed wait edit mutated source");
    change.expected_wait=2U;change.replacement_wait=1U;
    file.apply(change);
    require(file.encode()==original,"CINE09 synthetic exact undo failed");
}

std::vector<std::uint8_t> read_file(const char* path) {
    std::ifstream f(path,std::ios::binary);
    if (!f) throw std::runtime_error(std::string("CINE09 cannot open: ")+path);
    return {std::istreambuf_iterator<char>(f),std::istreambuf_iterator<char>()};
}
void test_original(const std::vector<std::uint8_t>& original) {
    Cine09File file(original);
    const auto stats=file.stats();
    require(original.size()==5485203U && stats.frames==112U &&
            stats.chunks==114U && stats.default_wait==1U &&
            stats.by_id==std::array<std::size_t,10>{{12U,1U,1U,30U,0U,0U,1U,2U,66U,1U}} &&
            file.encode()==original && file.total_ticks()==592U,
            "CINE09 uploaded original source inventory or roundtrip mismatch");
    const auto cues=file.timed_cues();
    require(cues.size()==4U &&
            cues[0U].command.frame==17U && cues[0U].start_tick==72U &&
            cues[0U].command.first==30U && cues[0U].command.second==11U &&
            cues[1U].command.frame==18U && cues[1U].start_tick==73U &&
            cues[1U].command.first==30U &&
            cues[2U].command.frame==93U && cues[2U].start_tick==518U &&
            cues[2U].command.first==19U && !cues[2U].command.queue_suppressed &&
            cues[3U].command.frame==94U && cues[3U].start_tick==519U &&
            cues[3U].subtitle_clear &&
            file.sample_tick(591U).frame==111U &&
            file.sample_tick(72U).frame==17U &&
            file.sample_tick(518U).frame==93U,
            "CINE09 uploaded original source cue/tick timeline mismatch");
    const auto s0=file.sample_frame(0U),s5=file.sample_frame(5U);
    const auto s82=file.sample_frame(82U),s111=file.sample_frame(111U);
    require(s0.pixels[51U*320U]==118U &&
            s0.pixels[306U*320U+319U]==4U &&
            s0.pixels[200U*320U+200U]==166U &&
            s5.dac6[3U]==1U && s5.dac6[5U]==2U &&
            s82.pixels[51U*320U]==27U &&
            s82.pixels[200U*320U+200U]==17U &&
            s111.pixels[51U*320U]==29U &&
            s111.dac6[3U]==0U &&
            s111.last_palette_frame==111U,
            "CINE09 uploaded source-backed pixel/palette values differ");
    const auto bmp5=file.bmp_at_frame(5U);
    check_bmp(bmp5);
    require(bmp5[54U+4U]==8U && bmp5[54U+5U]==4U &&
            bmp5[54U+6U]==4U &&
            bmp5[bmp_pixel(0U,51U)]==118U,
            "CINE09 original palette expansion or image export differs");
    require(file.bmp_at_frame(82U)[bmp_pixel(0U,51U)]==27U,
            "CINE09 source frame 82 BMP mismatches native pixels");
    const auto previous_bmp82 = file.bmp_at_frame(82U);
    Cine07Edits change{};
    change.expected_wait=1U;change.replacement_wait=2U;
    file.apply(change);
    require(file.total_ticks()==608U && file.stats().frames==112U &&
            file.bmp_at_frame(82U)==previous_bmp82,
            "CINE09 uploaded source time edit broke frame conversion");
    const auto modified=file.encode();
    rejects([&]{file.apply(change);});
    require(file.encode()==modified,"CINE09 stale edit failed atomicity");
    change.expected_wait=2U;change.replacement_wait=1U;
    file.apply(change);
    require(file.encode()==original && file.total_ticks()==592U,
            "CINE09 uploaded source byte-exact undo failed");
}
void check_generic(const char* path) {
    const auto bytes=read_file(path);
    const auto name=std::string(path);
    const auto special=name.find("CINE12.ANM")!=std::string::npos;
    Cine09File file(bytes, special?2U:6U);
    require(file.encode()==bytes,"CINE09 generic codec changed source bytes");
    if(file.stats().frames==0U)return;
    const auto count=file.stats().frames;
    for(const auto frame : {std::size_t{0U},count/2U,count-1U}) {
        const auto s=file.sample_frame(frame);
        const auto bmp=Cine09File::encode_indexed_bmp(s);
        check_bmp(bmp);
        for(const auto pos : std::array<std::size_t,5U>{
                0U,101U,320U*51U,320U*200U+117U,320U*399U+319U}) {
            const auto y=pos/320U,x=pos%320U;
            require(s.pixels[pos]==bmp[bmp_pixel(x,y)],
                    "CINE09 corpus indexed BMP pixel differs");
        }
    }
    if(file.total_ticks()!=0U) {
        const auto first=file.sample_tick(0U);
        const auto last=file.sample_tick(file.total_ticks()-1U);
        require(first.frame<count && last.frame<count,
                "CINE09 corpus timing selector outside source");
    }
}
} // namespace

int main(int argc,char** argv) {
    test_synthetic();
    if(argc>=2)test_original(read_file(argv[1]));
    for(int i=2;i<argc;++i)check_generic(argv[i]);
}
