#include "rloft/RES11.hpp"
#include "rloft/RES4.hpp"
#include "rloft/resource_assets.hpp"
#include "rloft/voc_decoder.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
using rloft::Res11Archive;
using rloft::Res11EntryPatch;
using rloft::Res11Family;

void need(bool condition, const char* why) {
    if (!condition) throw std::runtime_error(why);
}
template<class F> void rejects(F op) {
    bool failed=false;
    try {op();} catch(const std::exception&) {failed=true;}
    need(failed,"RES11 accepted invalid edit");
}
void write16(std::vector<std::uint8_t>& b,std::size_t at,unsigned n) {
    b.at(at)=static_cast<std::uint8_t>(n);
    b.at(at+1U)=static_cast<std::uint8_t>(n>>8U);
}
void write32(std::vector<std::uint8_t>& b,std::size_t at,std::uint32_t n) {
    for(unsigned i=0;i<4U;++i)b.at(at+i)=static_cast<std::uint8_t>(n>>(8U*i));
}
std::vector<std::uint8_t> literal_lzss(const std::vector<std::uint8_t>& b) {
    std::vector<std::uint8_t> out;
    for(std::size_t i=0;i<b.size();i+=8U) {
        const auto n=std::min<std::size_t>(8U,b.size()-i);
        out.push_back(static_cast<std::uint8_t>((1U<<n)-1U));
        out.insert(out.end(),b.begin()+static_cast<std::ptrdiff_t>(i),
                   b.begin()+static_cast<std::ptrdiff_t>(i+n));
    }
    return out;
}
std::vector<std::uint8_t> resource(unsigned type,const std::vector<std::uint8_t>& payload,
                                   unsigned declared=0U) {
    std::vector<std::uint8_t> out{'E','H',static_cast<std::uint8_t>(type),0,0};
    if(type==6U||type==7U) {
        out.resize(7U);write16(out,5U,declared);
    } else {
        out.resize(9U);write32(out,5U,static_cast<unsigned>(payload.size()));
    }
    out.insert(out.end(),payload.begin(),payload.end());
    return out;
}
std::vector<std::uint8_t> valid_voc() {
    std::vector<std::uint8_t> v(26U,0U);
    constexpr char sig[]="Creative Voice File";
    std::copy_n(sig,19,v.begin()); v[19U]=0x1AU;
    write16(v,20U,26U);write16(v,22U,0x010AU);write16(v,24U,0x1129U);
    v.insert(v.end(),{1U,5U,0U,0U,0xA6U,0U,0x80U,0x81U,0x7FU,0U});
    return v;
}
std::vector<std::uint8_t> synthetic() {
    // Type7: 64-column source, one visible column, a terminated display
    // list, and two producer-owned trailing bytes preserved in the body.
    std::vector<std::uint8_t> shape(16U,0U);
    write16(shape,0U,64U);write16(shape,4U,0U);
    write16(shape,8U,128U);write16(shape,10U,12U);
    shape[14U]=0xA5U;shape[15U]=0x5AU;
    std::vector<std::uint8_t> texels(4096U,0x7FU);
    const std::vector<std::uint8_t> xdir{
        'F','O','R','M',0U,0U,0U,4U,'X','D','I','R'};
    const std::vector<std::vector<std::uint8_t>> entries{
        resource(7U,literal_lzss(shape),static_cast<unsigned>(shape.size())),
        resource(6U,literal_lzss(texels),static_cast<unsigned>(texels.size())),
        resource(3U,xdir),resource(4U,valid_voc())};
    std::vector<std::uint8_t> b(4U+4U*entries.size(),0U);
    write32(b,0U,static_cast<std::uint32_t>(entries.size()));
    for(std::size_t i=0;i<entries.size();++i) {
        write32(b,4U+4U*i,static_cast<std::uint32_t>(b.size()));
        b.insert(b.end(),entries[i].begin(),entries[i].end());
    }
    return b;
}
void synthetic_test() {
    const auto original=synthetic();
    Res11Archive file(original);
    const auto stats=file.stats();
    need(stats.entries==4U && stats.shapes==1U && stats.textures==1U &&
         stats.xmidi==1U && stats.voices==1U &&
         stats.anomalous_voices==0U && file.encode()==original,
         "RES11 synthetic composite inventory changed");
    const auto result=file.verify_all();
    need(result.decoded_shapes==1U && result.decoded_textures==1U &&
         result.decoded_xmidi==1U && result.validated_voices==1U &&
         result.shape_body_bytes==16U && result.texture_texel_bytes==4096U,
         "RES11 synthetic decoder results changed");
    need(file.range(Res11Family::SceneShape).first==0 &&
         file.range(Res11Family::Texture).first==1 &&
         file.range(Res11Family::Xmidi).first==2 &&
         file.range(Res11Family::Voice).first==3,
         "RES11 bank partition failed");
    auto shape=file.shape_body(0);shape.back()^=1U;
    auto texture=file.texture(1);texture.texels_column_major[128U]^=1U;
    auto xdir=file.xmidi(2);xdir.push_back(0U);
    auto voice=file.voc(3);voice[32U]^=1U;
    const std::vector<Res11EntryPatch> edits{
        file.shape_body_patch(0,shape),file.texture_patch(1,texture),
        file.xmidi_patch(2,xdir),file.voc_patch(3,voice)};
    file.apply(edits);
    need(file.shape_body(0)==shape &&
         file.texture(1).texels_column_major==texture.texels_column_major &&
         file.xmidi(2)==xdir && file.voc(3)==voice &&
         file.pcm16(3).samples.size()==3U,
         "RES11 synthetic combined editing/decoding failed");
    const auto committed=file.encode();
    rejects([&]{file.apply(edits);});
    rejects([&]{file.apply({{0,{},edits[0].replacement_entry}});});
    rejects([&]{file.apply({edits[0],edits[0]});});
    auto bad=file.entry(0);bad[2U]=6U;
    rejects([&]{file.apply({{0,file.entry(0),bad}});});
    bad=file.entry(0);bad.pop_back();
    rejects([&]{file.apply({{0,file.entry(0),bad}});});
    bad=file.entry(3);bad[5U]^=1U;
    rejects([&]{file.apply({{3,file.entry(3),bad}});});
    need(file.encode()==committed,"rejected RES11 edit partially committed");
    std::vector<Res11EntryPatch> undo;
    for (const auto& edit:edits) {
        undo.push_back({edit.index,file.entry(edit.index),edit.expected_entry});
    }
    file.apply(undo);
    need(file.encode()==original,"RES11 synthetic inverse is not byte-exact");
    file.apply({});need(file.encode()==original,"empty RES11 edit mutated bytes");
    rejects([&]{(void)file.entry(-1);});
    rejects([&]{(void)file.entry(4);});
    rejects([&]{(void)file.texture(0);});
    rejects([&]{(void)file.pcm16(2);});
    auto broken=original; broken.pop_back();
    rejects([&]{(void)Res11Archive(broken);});
}
void original_test(const std::vector<std::uint8_t>& original) {
    need(original.size()==15589639U,"RES11 original file size differs");
    Res11Archive file(original);
    const auto st=file.stats();
    need(file.entry_count()==5114 && st.shapes==4128U &&
         st.textures==652U && st.xmidi==74U && st.voices==260U &&
         st.anomalous_voices==1U && file.encode()==original,
         "RES11 original composite/archive identity differs");
    need(file.range(Res11Family::SceneShape).first==0 &&
         file.range(Res11Family::Texture).first==4128 &&
         file.range(Res11Family::Xmidi).first==4780 &&
         file.range(Res11Family::Voice).first==4854,
         "RES11 original archive boundaries changed");
    const auto anomaly=file.entry_info(4877);
    need(anomaly.family==Res11Family::Voice &&
         anomaly.legacy_length_anomaly &&
         anomaly.declared_bytes==0x11669U &&
         anomaly.stored_bytes==5746U && file.voc(4877).size()==5737U,
         "original RES11:4877 truncated VOC was repaired or lost");
    rejects([&]{(void)file.pcm16(4877);});
    const auto full=file.verify_all();
    need(full.decoded_shapes==4128U && full.decoded_textures==652U &&
         full.decoded_xmidi==74U && full.validated_voices==259U &&
         full.preserved_anomalies==1U &&
         full.texture_texel_bytes==5066752ULL,
         "RES11 original deep decode incomplete");
    // Edit first narrow texture, choose a valid alternative XDIR source,
    // and alter one source PCM8 byte in a well-formed VOC. Keep the anomalous
    // asset unmodified across both the forward transaction and inverse.
    const auto texture_id=4713; // 4128 + 585: first legacy 32x128 texture.
    auto tex=file.texture(texture_id);
    need(tex.width==32 && tex.texels_column_major.size()==4096U,
         "RES11 narrow texture offset changed");
    tex.texels_column_major[0U]^=1U;
    auto voice=file.voc(4854);
    need(voice.size()>35U && voice[26U]==1U && voice[30U]==0xA6U,
         "RES11 first voice payload differs");
    voice[32U]^=1U;
    const auto other_music=file.xmidi(4781);
    const std::vector<Res11EntryPatch> edits{
        file.texture_patch(texture_id,tex),
        file.xmidi_patch(4780,other_music),
        file.voc_patch(4854,voice)};
    file.apply(edits);
    need(file.texture(texture_id).texels_column_major==tex.texels_column_major &&
         file.xmidi(4780)==other_music && file.voc(4854)==voice,
         "RES11 edit content mismatch");
    // Preserve the anomaly and an unrelated original entry exactly.
    const auto edited=file.encode();
    Res11Archive after(edited);
    need(after.entry(4877)==Res11Archive(original).entry(4877) &&
         after.entry(3)==Res11Archive(original).entry(3),
         "RES11 edit mutated untouched original entry");
    rejects([&]{file.apply(edits);});
    const auto checkpoint=file.encode();
    auto invalid=file.entry(4877);invalid[5U]^=1U;
    rejects([&]{file.apply({{4877,file.entry(4877),invalid}});});
    need(file.encode()==checkpoint,"invalid VOC anomaly patch changed state");
    std::vector<Res11EntryPatch> inverse;
    for(const auto& e:edits)
        inverse.push_back({e.index,file.entry(e.index),e.expected_entry});
    file.apply(inverse);
    need(file.encode()==original,"RES11 original source not restored byte-for-byte");
}
} // namespace

int main(int argc,char** argv) {
    synthetic_test();
    if(argc>2)throw std::invalid_argument("usage: RES11_tests [RES11]");
    if(argc==2) {
        std::ifstream input(argv[1],std::ios::binary);
        if(!input)throw std::runtime_error("cannot open uploaded RES11");
        const std::vector<std::uint8_t> original{
            std::istreambuf_iterator<char>(input),std::istreambuf_iterator<char>()};
        original_test(original);
    }
}
