#include "rloft/CINE05.hpp"
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
using rloft::Cine05File;
using rloft::Cine05Edits;
using rloft::Cine05RegionPatch;
void need(bool ok,const char* msg){if(!ok)throw std::runtime_error(msg);}
template<class F>void rejects(F f){bool yes=false;try{f();}catch(const std::exception&){yes=true;}need(yes,"invalid CINE05 operation accepted");}
void w16(std::vector<std::uint8_t>& b,unsigned x){b.push_back(static_cast<std::uint8_t>(x));b.push_back(static_cast<std::uint8_t>(x>>8U));}
void w32(std::vector<std::uint8_t>& b,unsigned x){for(unsigned i=0;i<4;++i)b.push_back(static_cast<std::uint8_t>(x>>(i*8U)));}
void chunk(std::vector<std::uint8_t>& b,unsigned id,const std::vector<std::uint8_t>& p){w16(b,id);w32(b,static_cast<unsigned>(p.size()));b.insert(b.end(),p.begin(),p.end());}
std::vector<std::uint8_t> synthetic(){
 std::vector<std::uint8_t>b;w16(b,3);w16(b,1);
 w16(b,2);chunk(b,1,{});chunk(b,0,std::vector<std::uint8_t>(768,11));
 w16(b,2);std::vector<std::uint8_t> px(320U*256U,22);chunk(b,8,rloft::Cine00File::encode_viewport(px));chunk(b,5,{19,0,0,0});
 w16(b,2);chunk(b,8,rloft::Cine00File::encode_viewport(px));chunk(b,3,{0,0});return b;
}
std::vector<std::uint8_t> load(const char* name){std::ifstream f(name,std::ios::binary);if(!f)throw std::runtime_error("CINE05 fixture unavailable");return{std::istreambuf_iterator<char>(f),std::istreambuf_iterator<char>()};}
void synthetic_test(){
 const auto src=synthetic();Cine05File f(src);need(f.encode()==src&&f.stats().frames==3&&f.stats().by_id[8]==2,"synthetic codec mismatch");
 rloft::Cine04File other(src);rloft::Cine04Edits diff{};
 // Three scattered changes in one rectangle and a second image.
 rloft::Cine04PlanarRun a{1,0,0,2,2,{22},{31}},b{1,0,3,3,3,{22},{32}},c{2,0,1,255,79,{22},{33}};
 diff.runs={a,b,c};other.apply(diff);Cine05File target(other.encode());
 const auto regions=f.regions_to(target);
 need(regions.size()==2&&regions[0].x==8&&regions[0].y==2&&regions[0].width==8&&regions[0].height==2&&
      regions[1].x==317&&regions[1].y==255&&regions[1].width==1&&regions[1].height==1,"region boundaries wrong");
 need(f.regions_to(Cine05File(src)).empty(),"identical archives differed");
 Cine05Edits changes{};changes.regions=regions;
 changes.base.base.base.palette={{0,1,5,11,12}};
 changes.base.base.base.base.cues={{1,1,rloft::Cine01CueKind::MusicSequence,19,0,20,0}};
 changes.base.base.base.base.packets.expected_wait=1;
 changes.base.base.base.base.packets.replacement_wait=2;
 f.apply(changes);
 need(f.payload(1,0)==target.payload(1,0)&&f.payload(2,0)==target.payload(2,0)&&
      f.stats().default_wait==2&&f.cues()[0].first==20&&f.payload(0,1)[5]==12,"combined region transaction failed");
 const auto checkpoint=f.encode();rejects([&]{f.apply(changes);});
 Cine05Edits bad{};auto r=regions[0];std::swap(r.expected,r.replacement);
 bad.regions={r,r};rejects([&]{f.apply(bad);});
 bad.regions={r};bad.regions[0].x=319;rejects([&]{f.apply(bad);});
 bad.regions={r};bad.regions[0].y=255;rejects([&]{f.apply(bad);});
 bad.regions={r};bad.regions[0].replacement.pop_back();rejects([&]{f.apply(bad);});
 bad.regions={r};bad.regions[0].expected[0]^=1;rejects([&]{f.apply(bad);});
 bad.regions={r};bad.regions[0].frame=0;bad.regions[0].chunk=1;rejects([&]{f.apply(bad);});
 bad.regions={r};bad.base.runs={{1,0,0,2,2,{31},{22}}};rejects([&]{f.apply(bad);});
 bad.regions={r};bad.base.base.base.base.packets.chunks={{1,0,f.payload(1,0),f.payload(1,0)}};rejects([&]{f.apply(bad);});
 bad.regions={r};bad.base.base.base.palette={{0,1,5,11,13}};rejects([&]{f.apply(bad);});
 need(f.encode()==checkpoint,"invalid edit partially committed");
 Cine05Edits undo{};for(auto p:regions){std::swap(p.expected,p.replacement);undo.regions.push_back(std::move(p));}
 undo.base.base.base.palette={{0,1,5,12,11}};
 undo.base.base.base.base.cues={{1,1,rloft::Cine01CueKind::MusicSequence,20,0,19,0}};
 undo.base.base.base.base.packets.expected_wait=2;
 undo.base.base.base.base.packets.replacement_wait=1;
 f.apply(undo);need(f.encode()==src,"synthetic inverse not bit-exact");
 f.apply({});need(f.encode()==src,"empty edit mutated file");
 auto bad_src=src;bad_src.pop_back();rejects([&]{Cine05File x(bad_src);});
}
void retail_test(const std::vector<std::uint8_t>& src,const std::vector<std::uint8_t>& b2,const std::vector<std::uint8_t>& b3,const std::vector<std::uint8_t>& b4){
 Cine05File target(src);
 need(src.size()==7498347U&&target.encode()==src&&target.stats().frames==128&&target.stats().chunks==131&&target.stats().default_wait==1&&
      target.stats().by_id==std::array<std::size_t,10>{{24,1,1,9,0,1,0,2,91,2}},"CINE05 retail inventory mismatch");
 need(target.cues().size()==5&&target.cues()[0].frame==1&&target.cues()[0].first==19&&
      target.cues()[1].frame==7&&target.cues()[2].frame==8&&target.cues()[3].frame==76&&target.cues()[4].frame==82,
      "CINE05 source audiovisual timeline mismatch");
 const std::array<std::vector<std::uint8_t>,3> originals{{b2,b3,b4}};
 const std::array<std::size_t,3> expected_bytes{{6022,3488,1648}};
 const std::array<std::size_t,3> expected_packets{{35,35,33}};
 for(std::size_t q=0;q<3;++q){
  Cine05File f(originals[q]);const auto changes=f.regions_to(target);
  need(changes.size()==expected_packets[q],"CINE05 changed packet count mismatch");
  for(const auto& p:changes){
   need(p.frame>=75&&p.frame<=116&&p.expected.size()==static_cast<std::size_t>(p.width)*p.height&&p.replacement.size()==p.expected.size(),"invalid rectangular region");
  }
  // Count only truly different pixels; rectangular bounding boxes may include unchanged values.
  std::size_t actual=0;for(const auto& p:changes)for(std::size_t i=0;i<p.expected.size();++i)actual+=p.expected[i]!=p.replacement[i];
  need(actual==expected_bytes[q],"CINE05 original pixel difference count mismatch");
  Cine05Edits edits{};edits.regions=changes;f.apply(edits);
  need(f.encode()==src,"CINE05 regional conversion is not byte exact");
  Cine05Edits undo{};for(auto p:changes){std::swap(p.expected,p.replacement);undo.regions.push_back(std::move(p));}
  f.apply(undo);need(f.encode()==originals[q],"CINE05 reverse conversion is not byte exact");
 }
}
} // namespace
int main(int argc,char** argv){
 synthetic_test();
 if(argc!=1&&argc!=5)throw std::invalid_argument("CINE05_tests [CINE05.ANM CINE02.ANM CINE03.ANM CINE04.ANM]");
 if(argc==5)retail_test(load(argv[1]),load(argv[2]),load(argv[3]),load(argv[4]));
}
