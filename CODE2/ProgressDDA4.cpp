#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct ProgressDDA4Ops{
 std::function<std::uint16_t(int)> stateF0;
 std::function<std::int8_t(int,int)> cls;
 std::function<std::int16_t&(int,int)> level;
 std::function<std::int32_t&(int,int)> xp;
 std::function<std::uint16_t(int,int)> xpThreshold;
 std::function<int(int,int)> selector50DC;
 std::function<void(int,int)> normalize3EA4,normalize3DE4;
 std::function<void(int,int,int)> restoreDC7C;
 std::function<int(int)> metricDBD4;
};
void progressDDA4(const ProgressDDA4Ops&o,int bonus){
 int alive=0;for(int ch=0;ch<4;++ch)if((o.stateF0(ch)&0xf240u)==0)++alive;
 int share=(alive/2+bonus)/alive;if(share==0)share=1;
 for(int ch=0;ch<4;++ch){if(o.stateF0(ch)&0xf240u)continue;int n=1;if(o.cls(ch,1)!=-1)n=2;if(o.cls(ch,2)!=-1)++n;const int per=(n/2+share)/n;int bestSlot=0;
  for(int slot=0;slot<n;++slot){o.xp(ch,slot)+=per;int lv=0;while(lv<20&&o.xpThreshold(o.cls(ch,slot),lv)<=static_cast<std::uint32_t>(o.xp(ch,slot))/1000u)++lv;
   if(o.level(ch,slot)!=lv){o.level(ch,slot)=static_cast<std::int16_t>(lv);if(o.selector50DC(ch,0)>=0){if(o.cls(ch,slot)==1)o.normalize3EA4(ch,lv);else o.normalize3DE4(ch,lv);}o.restoreDC7C(ch,slot,lv);}
   if(o.level(ch,slot)>o.level(ch,bestSlot))bestSlot=slot;
  }
  (void)bestSlot;(void)o.metricDBD4(ch);
 }
}
}
