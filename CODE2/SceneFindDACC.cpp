#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct SceneEntityDACC{std::uint16_t value0,value2;std::uint16_t link6;std::uint16_t state8;};
struct FindDACCOps{
 std::function<std::uint32_t()> page;
 std::function<std::uint16_t(int)> count;
 std::function<const SceneEntityDACC&(int,int)> record;
 std::function<std::uint8_t(int)> linkedFlags;
};
int findDACC(const FindDACCOps&o,int mode){
 const int p=static_cast<int>((o.page()^1u)&0xffu);
 for(int wanted=static_cast<int>(o.count(p))-1;wanted>=0;--wanted){
  int idx=0;for(;idx<o.count(p);++idx)if(o.record(p,idx).link6==wanted)break;
  const auto&r=o.record(p,idx);if(r.state8!=0)continue;if(o.linkedFlags(r.link6)&2u)continue;
  if(mode==0&&r.value0>0x80u)continue;if(mode==1&&r.value0<=0x80u)continue;
  const int band=(static_cast<int>(r.value2)-0x7d00)>>3;if(band<0x3b)continue;
  if(o.linkedFlags(r.link6)&0x10u)return r.link6;
 }
 return -1;
}
void chooseDB94(const FindDACCOps&o,int flags){
 if((flags&1)==0){if(findDACC(o,0)==-1)findDACC(o,1);}
 else if((flags&1)==1){if(findDACC(o,1)==-1)findDACC(o,0);}
}
}
