#include "AttributeBoostA8A0.h"
namespace ravenloft::game {
static std::uint32_t& d(std::uint8_t*p,int n){return *reinterpret_cast<std::uint32_t*>(p+n);}
static void common(AttributeBoostA8A0State&s,const AttributeBoostA8A0Ops&o,int ch,int resource){
 o.play28110(s.resourceBase+resource,0xff,0,1);for(auto&v:s.refresh)v=1;auto a=s.d681,b=s.d682;s.d681=s.d682=0;o.ui1F494(1);o.panel9B4C(ch,2,0);s.d681=a?2:0;s.d682=b?2:0;for(auto&v:s.refresh)v=1;o.ui1F494(1);
}
int AttributeBoostA8A0::first(AttributeBoostA8A0State&s,const AttributeBoostA8A0Ops&o,int ch){
 auto*c=s.characters+ch*0x248;if(c[0x133]&0x10)return 0;common(s,o,ch,0x1a);c[0x133]|=0x10;
 int v=o.value22E40(3,ch);d(c,0x166)=std::uint32_t(o.clock()+v*0x438);int add=v/3+1,cur=c[0x32],cap=c[0x14]==2?0x11:0x12;if(cur+add>cap)add=cap-cur;d(c,0x123)=std::uint32_t(o.adjust1202C(ch,add));return 1;
}
int AttributeBoostA8A0::second(AttributeBoostA8A0State&s,const AttributeBoostA8A0Ops&o,int ch){
 auto*c=s.characters+ch*0x248;if(c[0x133]&0x20)return 0;common(s,o,ch,0x19);c[0x133]|=0x20;
 int v=o.value22E40(3,ch);d(c,0x16a)=std::uint32_t(o.clock()+v*0x438);int add=v/3+1,cur=c[0x35],cap=(c[0x14]==2||c[0x14]==5)?0x13:0x12;if(cur+add>cap)add=cap-cur;d(c,0x127)=std::uint32_t(add);c[0x35]=std::uint8_t(cur+add);return 1;
}}
