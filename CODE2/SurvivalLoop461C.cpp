#include "SurvivalLoop461C.h"
namespace ravenloft::game {
namespace {std::uint16_t rw(std::span<std::uint8_t>b,std::size_t p){return std::uint16_t(b[p])|(std::uint16_t(b[p+1])<<8);}void ww(std::span<std::uint8_t>b,std::size_t p,std::uint16_t v){b[p]=std::uint8_t(v);b[p+1]=std::uint8_t(v>>8);}}
void SurvivalLoop461C::run(SurvivalLoop461CState&s,const SurvivalLoop461COps&o){
 if(o.call4100()){o.callMessage(0x60AC,0);o.call1E74C(s.tickA7E4);return;}
 int timers[4]{},elapsed=0,pendingA=0,pendingB=0,pendingHeal=0,finish=0;
 for(int ch=0;ch<4;++ch){auto b=std::size_t(ch)*0x248;if(rw(s.objects218E0,b+0x10)&0x1000){s.dirtyCF5A[ch]=1;s.objects218E0[b+0x11]&=0xEF;}if((rw(s.objects218E0,b+0x10)&0xC000)==0){if(rw(s.objects218E0,b+0x1C)!=rw(s.objects218E0,b+0x1E))++pendingHeal;for(int r=0;r<8;++r)for(int c=0;c<10;++c){auto&a=s.objects218E0[b+0x6A+r*10+c];if((s.objects218E0[b+0x131]&2)&&(a&15)!=15&&(a&15)!=(a>>4)){if((a>>4)>(a&15))a=std::uint8_t((a&15)|((a&15)<<4));else pendingA+=(a&15)-(a>>4);}auto&d=s.objects218E0[b+0xBA+r*10+c];if((s.objects218E0[b+0x131]&5)&&(d&15)!=15&&(d&15)!=(d>>4)){if((d>>4)>(d&15))d=std::uint8_t((d&15)|((d&15)<<4));else{timers[ch]=0x2D0;++pendingB;}}}}}
 s.dirtyCF5A.fill(1);
 while(pendingHeal||pendingA||pendingB){++elapsed;s.tickA7E4+=0x438;s.tickCF4C+=0x445;o.call31F94();if(s.inputCF4E==0)for(int ch=0;ch<4;++ch){auto b=std::size_t(ch)*0x248;if((rw(s.objects218E0,b+0x10)&0xC000)==0){if((std::int8_t)s.objects218E0[b+0x15]==1)s.objects218E0[b+0xC1]=0x11;if(o.call50DC(0,ch)>=0)s.objects218E0[b+0xC2]=0x11;}}
  if(elapsed>=0x1E0)finish=1;o.call893C();if(s.active258A8!=0xFF&&((std::uint32_t(o.call30CA2())*100u)>>15)>0x5F&&o.call4228())break;
  for(int ch=0;ch<4;++ch){auto b=std::size_t(ch)*0x248;if(rw(s.objects218E0,b+0x10)>=0x4000)continue;if(rw(s.objects218E0,b+0x10)&0x0800){if((std::int16_t)rw(s.objects218E0,b+0x1C)==1){--pendingHeal;o.call43F4(ch,&pendingA,&pendingB);}o.callCD44(ch,1);s.dirtyCF5A[ch]=1;}if(rw(s.objects218E0,b+0x10)&0x0400){if((std::int16_t)rw(s.objects218E0,b+0x1C)<=3){--pendingHeal;o.call43F4(ch,&pendingA,&pendingB);}o.callCD44(ch,3);s.dirtyCF5A[ch]=1;}}
  if(pendingHeal&&elapsed%0x5A0==0)for(int ch=0;ch<4;++ch){auto b=std::size_t(ch)*0x248;if((rw(s.objects218E0,b+0x10)&0xC000)==0){auto cur=rw(s.objects218E0,b+0x1C),mx=rw(s.objects218E0,b+0x1E);if(cur!=mx){auto n=std::uint16_t(cur+3);if(n>=mx){n=mx;--pendingHeal;}ww(s.objects218E0,b+0x1C,n);s.dirtyCF5A[ch]=1;}}}
  o.call1F494(1);if(finish)for(int ch=0;ch<4;++ch){if((rw(s.objects218E0,std::size_t(ch)*0x248+0x10)&0xC000)!=0)continue;if(o.call50DC(0,ch)>=0){pendingB+=o.call44D0(ch,0,1,&timers[ch],&pendingB,0x51,elapsed);pendingB+=o.call44D0(ch,3,ch,&timers[ch],&pendingB,0x6E,elapsed);pendingB+=o.call44D0(ch,4,ch,&timers[ch],&pendingB,0x78,elapsed);}if(o.call23EAC(ch,1))o.call44D0(ch,7,ch,&timers[ch],&pendingB,0x57,elapsed);}if(o.call30B40()==0x1B)break;
 }
 if(o.call30B40()!=0)o.call30AEC();if(pendingA||pendingB){if(s.d681){s.d681=2;o.call231C0(s.d8A8,s.objects218E0[std::size_t(s.d8A8)*0x248+0x12F]);}if(s.d682){s.d682=2;o.call231C0(s.d8AC,s.objects218E0[std::size_t(s.d8AC)*0x248+0x12F]);}}o.callMessage(elapsed/60,elapsed%60);o.call1E74C(s.tickA7E4);
}}
