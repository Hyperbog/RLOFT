#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_f790_item_description_source.hpp"
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

int main() {
    using Source=rloft::OriginalF790ItemDescriptionSource;
    Source::Record rec{};
    rec[0]='I';rec[1]='T';rec[2]=0;
    std::vector<int> ds_calls,table_calls;
    int recursive_calls=0,group_calls=0,variant_calls=0;
    const auto ds=[&](std::uint32_t v)->std::string {
        ds_calls.push_back(static_cast<int>(v));
        switch(v) {
        case 0x610:return "+";
        case 0x614:return ":";
        case 0x618:return "[";
        case 0x61c:return "..";
        case 0x620:return ";";
        case 0x624:return ".5";
        case 0x6d98:return "#";
        default:throw std::out_of_range("unknown literal string");
        }
    };
    const auto text=[&](std::uint32_t v) {
        table_calls.push_back(static_cast<int>(v));
        return "T"+std::to_string(v);
    };
    const auto nested=[&](int n)->std::uint32_t {
        ++recursive_calls;assert(n==2);return 9U;
    };
    const auto a=[&](int n)->std::uint8_t {
        ++group_calls;assert(n==2);return 7U;
    };
    const auto b=[&](int n)->std::uint8_t {
        ++variant_calls;assert(n==2 || n==0);return 11U;
    };
    const auto run=[&](int category,bool known=false,int id=0) {
        rec[0x2f]=static_cast<std::uint8_t>(category);
        ds_calls.clear();table_calls.clear();
        return Source::compose(rec,id,known,ds,text,nested,a,b);
    };
    rec[0x37]=4;rec[0x34]=0xfe;
    for(int cat=2;cat<=4;++cat) {
        assert(run(cat,true)=="IT+4:T64[2:");
        assert((table_calls==std::vector<int>{0x40}));
        assert((ds_calls==std::vector<int>{0x610,0x614,0x618,0x614}));
        assert(run(cat)=="IT:T64[2:");
    }
    rec[0x34]=0x80;
    assert(run(2)=="IT:T64[128:");
    rec[0x38]=2;rec[0x39]=6;rec[0x3a]=3;
    rec[0x3b]=4;rec[0x3c]=8;rec[0x3d]=1;
    for(int cat=9;cat<=11;++cat) {
        assert(run(cat)=="IT:T62[5..15;5..33");
        assert((table_calls==std::vector<int>{0x3e}));
        assert((ds_calls==std::vector<int>{0x614,0x618,0x61c,0x620,0x61c}));
    }
    rec[0x30]=5;rec[0x31]=0;rec[0x37]=0;
    for(int cat=5;cat<=8;++cat) {
        assert(run(cat)=="IT:T612.5");
        assert((table_calls==std::vector<int>{0x3d}));
    }
    assert(run(0)=="IT:T612.5");
    assert(run(12)=="IT:T612.5");
    rec[0x31]=2;
    assert(run(5,true)=="IT:T612.5#11");
    assert(variant_calls==1);
    rec[0x31]=0;
    assert(run(5,true,0x4a)=="IT:T612.5#11");
    rec[0x31]=2;
    assert(run(1)=="IT:T617:T65[7");
    assert(recursive_calls==1 && group_calls==1);
    const auto sentinel=[](int)->std::uint8_t{return 0x7fU;};
    assert(Source::compose(rec,1,false,ds,text,nested,sentinel,b)=="IT:T617");
    const auto large=[](int)->std::uint32_t {return 65534U;};
    assert(Source::compose(rec,1,false,ds,text,large,sentinel,b)=="IT:T611.5");
    Source::Record invalid{}; invalid.fill('A');
    bool rejected=false;
    try { (void)Source::compose(invalid,0,false,ds,text,nested,a,b); }
    catch(const std::out_of_range&) { rejected=true; }
    assert(rejected);
}
