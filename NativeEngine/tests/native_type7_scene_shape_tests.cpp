#include "rloft_native/native_type7_scene_shape.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

using namespace rloft_native;
namespace {
void u16(std::vector<std::uint8_t>& b,std::size_t at,unsigned n) {
    b.at(at)=static_cast<std::uint8_t>(n);
    b.at(at+1)=static_cast<std::uint8_t>(n>>8U);
}
void u32(std::vector<std::uint8_t>& b,std::size_t at,unsigned n) {
    for(unsigned k=0;k<4;++k)
        b.at(at+k)=static_cast<std::uint8_t>(n>>(8U*k));
}
template<class F> void rejects(F&& fn) {
    bool threw=false;
    try{fn();}catch(const std::exception&){threw=true;}
    assert(threw);
}
std::vector<std::uint8_t> shape_body() {
    // Exactly three source columns. Two share a list beginning at an
    // UNALIGNED byte offset; a third points to an empty terminated list.
    std::vector<std::uint8_t> body(27,0);
    u16(body,0,64);u16(body,2,0);u16(body,4,2);
    u16(body,6,0);u16(body,8,127);
    u16(body,10,17);u16(body,12,17);u16(body,14,25);
    body[16]=0xA5; // one byte of padding intentionally creates odd offset
    u16(body,17,2);u16(body,19,0xFFFC);u16(body,21,4);
    u16(body,23,0);u16(body,25,0);
    return body;
}
std::vector<std::uint8_t> compressed_shape(const std::vector<std::uint8_t>& body) {
    std::vector<std::uint8_t> eh{'E','H',7,0,0};
    eh.push_back(static_cast<std::uint8_t>(body.size()));
    eh.push_back(static_cast<std::uint8_t>(body.size()>>8U));
    // Every token is a literal. Each flag controls eight bytes and the
    // final group may have fewer than eight bytes.
    for(std::size_t i=0;i<body.size();) {
        eh.push_back(0xff);
        for(unsigned k=0;k<8 && i<body.size();++k,++i)
            eh.push_back(body[i]);
    }
    return eh;
}
std::vector<std::uint8_t> fake_init() {
    std::vector<std::uint8_t> init(0x24+2+175*7+2+12*7,0);
    // 3/2/2/1 shapes in banks class 0/1/2/3.
    u16(init,0x06,0);u16(init,0x08,3);
    u16(init,0x0a,5);u16(init,0x0c,7);
    u16(init,0x24,175);
    for(unsigned i=0;i<175;++i) {
        const auto p=0x26U+i*7U;
        u16(init,p,0);
        init[p+4]=2;
        init[p+5]=1;
    }
    const auto class3_at=0x24U+2U+175U*7U;
    u16(init,class3_at,12);
    for(unsigned i=0;i<12;++i) {
        const auto p=class3_at+2U+i*7U;
        init[p+4]=1;init[p+5]=1;
    }
    return init;
}
std::vector<std::uint8_t> fake_res(const std::vector<std::uint8_t>& eh,
                                   unsigned count=8) {
    std::vector<std::uint8_t> res(4U+count*4U,0);
    u32(res,0,count);
    for(unsigned i=0;i<count;++i) {
        u32(res,4U+i*4U,static_cast<unsigned>(res.size()));
        res.insert(res.end(),eh.begin(),eh.end());
    }
    return res;
}
void run() {
    const auto original_body=shape_body();
    const auto shape=decode_native_type7_scene_shape(
        compressed_shape(original_body));
    assert(shape.structure_verified);
    assert(shape.source_width==64 && shape.y_max==127);
    assert(shape.x_min==0 && shape.x_max==2);
    assert(shape.decoded_body_bytes==27);
    assert(shape.columns.size()==3);
    assert(shape.unique_lists==2);
    assert(shape.total_descriptors==2); // repeated list is drawn twice
    assert(shape.columns[0].original_list_offset==17);
    assert(shape.columns[1].original_list_offset==17);
    assert(shape.columns[2].descriptors.empty());
    assert(shape.columns[0].descriptors.size()==1);
    assert(shape.columns[0].descriptors[0].stop_selector==2);
    assert(shape.columns[0].descriptors[0].signed_adjustment==-4);
    assert(shape.columns[0].descriptors[0].entry_selector==4);

    auto bad=original_body;
    bad[17]=3; // odd stop selector (not a legal SINTAB offset)
    rejects([&]{static_cast<void>(decode_native_type7_scene_shape(
        compressed_shape(bad)));});
    bad=original_body;bad[21]=5; // odd entry selector
    rejects([&]{static_cast<void>(decode_native_type7_scene_shape(
        compressed_shape(bad)));});
    bad=original_body;u16(bad,10,16); // table padding is not a list
    rejects([&]{static_cast<void>(decode_native_type7_scene_shape(
        compressed_shape(bad)));});
    bad=original_body;u16(bad,10,500); // beyond decoded EOF
    rejects([&]{static_cast<void>(decode_native_type7_scene_shape(
        compressed_shape(bad)));});
    bad=original_body;u16(bad,4,64); // x_max >= source_width
    rejects([&]{static_cast<void>(decode_native_type7_scene_shape(
        compressed_shape(bad)));});
    bad=original_body;u16(bad,6,0xFFFF); // signed negative y_min
    rejects([&]{static_cast<void>(decode_native_type7_scene_shape(
        compressed_shape(bad)));});
    bad=original_body;u16(bad,14,26); // final word not terminator
    rejects([&]{static_cast<void>(decode_native_type7_scene_shape(
        compressed_shape(bad)));});
    auto bad_eh=compressed_shape(original_body);
    bad_eh.push_back(0xf0); // trailing compressed garbage rejected
    rejects([&]{static_cast<void>(decode_native_type7_scene_shape(bad_eh));});
    bad_eh=compressed_shape(original_body);
    bad_eh[2]=6; // EH6 is a different format
    rejects([&]{static_cast<void>(decode_native_type7_scene_shape(bad_eh));});
    bad_eh=compressed_shape(original_body);
    bad_eh.pop_back(); // truncated final literal
    rejects([&]{static_cast<void>(decode_native_type7_scene_shape(bad_eh));});

    auto init=fake_init();
    const auto banks=decode_native_res3_scene_banks(init,8);
    assert((banks.starts==std::array<std::uint16_t,4>{0,3,5,7}));
    assert((banks.lengths==std::array<std::size_t,4>{3,2,2,1}));
    assert(banks.absolute_id(0,2)==2);
    assert(banks.absolute_id(1,0)==3);
    assert(banks.absolute_id(2,1)==6);
    assert(banks.absolute_id(3,0)==7);
    assert(!banks.contains(3,1));
    rejects([&]{static_cast<void>(banks.absolute_id(1,2));});
    rejects([&]{static_cast<void>(banks.absolute_id(4,0));});
    assert(banks.init_descriptors[0].size()==175);
    assert(banks.init_descriptors[1].size()==12);
    const NativeResArchive archive(fake_res(compressed_shape(original_body)));
    const auto corpus=audit_native_res3_scene_corpus(init,archive);
    assert(corpus.banks.res3_entries==8);
    assert(corpus.type7_shapes==8);
    assert(corpus.source_class_shapes[0]==3);
    assert(corpus.source_class_shapes[1]==2);
    assert(corpus.source_class_shapes[2]==2);
    assert(corpus.source_class_shapes[3]==1);
    assert(corpus.width_histogram[0]==8);
    assert(corpus.unique_lists==16);
    assert(corpus.raster_descriptors==16);
    const auto json=native_res3_scene_corpus_json(corpus);
    assert(json.find("\"decoded_eh7_shapes\":8")!=std::string::npos);
    assert(json.find("\"sintab_x86_code_executed\":false")!=std::string::npos);
    assert(json.find("\"source_class_lengths\":[3,2,2,1]")!=std::string::npos);
    assert(json.find("\"original_scene_frame_parity_verified\":false")!=std::string::npos);

    auto changed=init;u16(changed,0x0A,3); // duplicate bank start
    rejects([&]{static_cast<void>(
        decode_native_res3_scene_banks(changed,8));});
    changed=init;u16(changed,0x24,174);
    rejects([&]{static_cast<void>(
        decode_native_res3_scene_banks(changed,8));});
    changed=init;changed[0x26+5]=0; // zero variant count
    rejects([&]{static_cast<void>(
        decode_native_res3_scene_banks(changed,8));});
    changed=init;changed[0x26+4]=3; // class1 allocation overrun
    rejects([&]{static_cast<void>(
        decode_native_res3_scene_banks(changed,8));});
    changed=init;
    changed.resize(0x24+2+175*7); // missing class 3
    rejects([&]{static_cast<void>(
        decode_native_res3_scene_banks(changed,8));});
    rejects([&]{static_cast<void>(
        decode_native_res3_scene_banks(init,7));});
}
}
int main(){run();}
