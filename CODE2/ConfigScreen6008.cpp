#include "ConfigScreen6008.h"
namespace ravenloft::game {
void ConfigScreen6008::draw(ConfigScreen6008State&s,const ConfigScreen6008Ops&o){
 o.call291B0(std::uint8_t(s.c050));
 for(int y=0;y<0x190;y+=0x10)for(int x=0;x<0x140;x+=8){auto r=o.call30CA2(y,x,s.resource258DC+0x35,y);o.call2975C(int((r*4u)>>15),x,s.resource258DC+0x35,y,0);}
 for(int x=8;x<0x138;x+=8){auto r=o.call30CA2(0,x,s.resource258DC+0x35,0);o.call2975C(int((r*2u)>>15)+9,x,s.resource258DC+0x35,0,0);r=o.call30CA2(0x180,x,s.resource258DC+0x35,0);o.call2975C(int((r*2u)>>15)+5,x,s.resource258DC+0x35,0x180,0);}
 for(int x=0x10;x<0x180;x+=0x10){auto r=o.call30CA2(x,0,s.resource258DC+0x35,0);o.call2975C(int((r*2u)>>15)+0x0c,x,s.resource258DC+0x35,0,0);r=o.call30CA2(x,0x138,s.resource258DC+0x35,0);o.call2975C(int((r*2u)>>15)+0x0e,x,s.resource258DC+0x35,0x138,0);}
 o.call2975C(8,0,0,s.resource258DC+0x35,0);o.call2975C(0x0b,0,0x138,s.resource258DC+0x35,0);o.call2975C(4,0x180,0,s.resource258DC+0x35,0);o.call2975C(7,0x180,0x138,s.resource258DC+0x35,0);
 o.call2E374(0x0f,0x78,0x18,0x89,0x3d);o.call2E374(0xa5,0x78,0x18,0xaf,0x3d);o.call2E374(0xff,0x78,0x18,0xbb,0x3d);
 o.call3060C(0x17,0x7c,int(s.art60E4),1);o.call3060C(0xad,0x7c,int(s.art60E4),1);o.call3060C(0x107,0x7c,int(s.art60E4),1);
 o.call305A8(0x17,0xfc,int(s.art60E8),1);o.call305A8(0xad,0x122,int(s.art60E8),1);o.call305A8(0x107,0x12d,int(s.art60E8),1);
 o.call5EEC();o.call1A510(s.c050);s.c050=(s.c050&~0xffu)|((s.c050^1u)&0xffu);o.call1A9F8();
}}
