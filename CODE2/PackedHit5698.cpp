#include "PackedHit5698.h"
namespace ravenloft::game {
int PackedHit5698::apply(PackedHit5698State&s,const PackedHit5698Ops&o,int mode,int mx,int my,int selected,int baseX,int baseY,std::uint8_t*limit){
 int rowOff=0,ptrOff=0;
 for(int row=0;row<6;++row,rowOff+=10,ptrOff+=4){
  const int x=s.x63F0[row]+0x2D;
  if(x>=mx||s.x63F0[row]+0xA5<=mx)continue;
  int delta=0;
  if(s.y63F4[row]+2<=my&&s.y63F4[row]+9>=my)delta=-1;
  if(s.y63F4[row]+0x0D<=my&&s.y63F4[row]+0x15>=my)delta=1;
  if(!delta)continue;
  const int cell=(mx-x)/12;
  const auto p=std::size_t(row)*20+cell*2;
  if(static_cast<std::int8_t>(s.buffers[p])==-1)continue;
  int sum=0;for(int i=0;i<s.counts6420[mode*8+row];++i){auto q=std::size_t(row)*20+i*2;if(static_cast<std::int8_t>(s.buffers[q])!=-1)sum+=static_cast<std::int8_t>(s.buffers[q+1]);}
  if(sum+delta>*limit)continue;
  if(delta==-1&&s.buffers[p+1]==0)return 0;
  s.buffers[p+1]=std::uint8_t(s.buffers[p+1]+delta);
  int bx=s.x63F0[row]+0x2D+cell*12,by=s.y63F4[row];
  o.call291B0(std::uint8_t(s.c050));o.call296C0(bx,by+1,bx+12,by+0x62);
  int art=(row<3)?o.call2F29C(baseX,0):o.call2F29C(baseY+1,0);
  o.call2E438(bx,0,12,art,bx);o.call296C0(0,0,0x18F,0x13F);o.call3060C(bx,by+2,0x380,1);
  o.call3060C(bx,by+0x19,o.call2E6F0(static_cast<std::int8_t>(s.buffers[p+1])),1);
  o.call1C368(0x759C,baseX+rowOff+static_cast<std::int8_t>(s.buffers[p]));
  o.call30630(bx,by+0x20,0x759C,1,0x44);
  o.call1A8C8(int(s.c050),bx,by,12,100);
  return 1;
 }
 return 0;
}
}
