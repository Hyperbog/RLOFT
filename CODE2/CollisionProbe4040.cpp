#include "CollisionProbe4040.h"
namespace ravenloft::game {
int CollisionProbe4040::run(const CollisionProbe4040State&s,const CollisionProbe4040Ops&o){
 for(int i=0;i<0x168;++i){
  std::int32_t dx=0,dy=0; o.call2DF2C(0,0x5000,dx,dy);
  const std::int32_t x=s.posX255D0+dx,y=s.posY255D4+dy;
  const auto cell=static_cast<int>(static_cast<std::int16_t>(x>>16))+static_cast<int>(static_cast<std::int16_t>(y>>16))*s.height25619;
  const auto f=s.mapFlags198E0[cell];
  if((f&0x3F)==0 || ((f&0x80)!=0 && o.call1E930(cell)!=0x8000)){
    if(o.call2C8E4(x,y,-1)!=0)return 0;
    --i; // original retries same successful traversal count
  }
 }
 return 1;
}
}
