#include "rloft_native/native_code2_scene_order.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <vector>

using namespace rloft_native;
namespace {
NativeCode2SceneRecord22 record(unsigned height,unsigned index){
    NativeCode2SceneRecord22 b{};
    b.fill(static_cast<std::uint8_t>(index));
    b[2]=static_cast<std::uint8_t>(height);
    b[3]=static_cast<std::uint8_t>(height>>8U);
    b[10]=0xfe; b[11]=0xff;
    return b;
}
unsigned word(const NativeCode2SceneRecord22& b,unsigned offset){
    return unsigned(b[offset]) | (unsigned(b[offset+1U])<<8U);
}
template<typename Fn> void rejects(Fn fn){
    bool failed=false;
    try{fn();}catch(const std::invalid_argument&){failed=true;}
    assert(failed);
}
void verify(const std::vector<unsigned>& heights){
    std::vector<NativeCode2SceneRecord22> source;
    for(unsigned i=0;i<heights.size();++i)
        source.push_back(record(heights[i],i));
    const auto actual=order_native_code2_scene_records_2c3f1(source);
    std::vector<std::size_t> reference(heights.size());
    std::iota(reference.begin(),reference.end(),std::size_t{0});
    // This independent mathematical oracle is valid ONLY under the
    // explicitly enforced initial [0,0x7D00) original source bound.
    std::stable_sort(reference.begin(),reference.end(),
        [&](std::size_t a,std::size_t b){return heights[a]<heights[b];});
    assert(actual.source_indices==reference);
    assert(!actual.original_live_scene_generation_verified);
    assert(!actual.original_dos_frame_parity_verified);
    assert(actual.marked_records.size()==source.size());
    for(unsigned slot=0;slot<reference.size();++slot){
        const auto i=reference[slot];
        const auto& b=actual.marked_records[i];
        assert(word(b,2)==heights[i]+0x7D00U);
        assert(word(b,10)==slot);
        for(unsigned j=0;j<22;++j)
            if(j!=2 && j!=3 && j!=10 && j!=11)
                assert(b[j]==source[i][j]);
        assert(word(source[i],10)==0xfffeU);
        assert(word(source[i],2)==heights[i]);
    }
}
}
int main(){
    verify({});
    verify({0});
    verify({100,20,20,0,0x7cff});
    verify({0x7cff,0,0x7cfe,0x7cff,1,1});
    verify(std::vector<unsigned>(400,0));
    std::vector<unsigned> reverse(400);
    for(unsigned i=0;i<400;++i)reverse[i]=399-i;
    verify(reverse);
    // Regression at 0x7D00: source MIN initial value would not select
    // this record; never silently synthesize a choice.
    rejects([]{static_cast<void>(order_native_code2_scene_records_2c3f1(
        {record(0x7d00,0)}));});
    rejects([]{static_cast<void>(order_native_code2_scene_records_2c3f1(
        {record(0xffff,0)}));});
    rejects([]{static_cast<void>(order_native_code2_scene_records_2c3f1(
        std::vector<NativeCode2SceneRecord22>(401,record(0,0))));});
}
