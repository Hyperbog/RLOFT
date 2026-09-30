#include "ConfirmDialog7D44.h"
namespace ravenloft::game {
int ConfirmDialog7D44::run(ConfirmDialog7D44State&s,const ConfirmDialog7D44Ops&o,int text){
 o.call291B0(std::uint8_t(s.c050));int tile=s.resource258DC+0x35;
 for(int y=0xa0;y<0xf0;y+=0x10)for(int x=0x20;x<0x120;x+=8){auto r=o.call30CA2(y,tile,x,y);o.draw2975C(int((r*4u)>>15),x,tile,y,0);}
 for(int x=0x20;x<0x118;x+=8){auto r=o.call30CA2(0xa0,tile,x,0);o.draw2975C(int((r*2u)>>15)+9,x,tile,0xa0,0);r=o.call30CA2(0xe0,tile,x,0);o.draw2975C(int((r*2u)>>15)+5,x,tile,0xe0,0);}
 for(int y=0xb0;y<0xe0;y+=0x10){auto r=o.call30CA2(y,tile,0x20,0);o.draw2975C(int((r*2u)>>15)+0xc,y,tile,0x20,0);r=o.call30CA2(y,tile,0x118,0);o.draw2975C(int((r*2u)>>15)+0xe,y,tile,0x118,0);}
 o.draw2975C(8,0xa0,0x20,tile,0);o.draw2975C(0xb,0xa0,0x118,tile,0);o.draw2975C(4,0xe0,0x20,tile,0);o.draw2975C(7,0xe0,0x118,tile,0);
 o.draw30508(0xaa,0x20,0x100,text,1);o.draw2975C(0,0xc8,0x60,s.resource258DC+0x63,0);o.draw2975C(2,0xc8,0xb0,s.resource258DC+0x63,0);
 o.call1A510(s.c050);s.c050=(s.c050&~0xffu)|((s.c050^1u)&0xffu);o.flip1A9F8();o.call291B0(std::uint8_t(s.c050));
 while(o.input1A1F4()){}for(;;){while(!o.input1A1F4()){}int x=o.mouseX9E31(),y=o.mouseY9E33();if(x<0xc8||x>0xe0)continue;
  if(y>=0x60&&y<0x90){o.draw2975C(1,0xc8,0x60,s.resource258DC+0x63,0);o.call1A510(s.c050);s.c050=(s.c050&~0xffu)|((s.c050^1u)&0xffu);o.call291B0(std::uint8_t(s.c050));return 0;}
  if(y>=0xb0&&y<0xe0){o.draw2975C(3,0xc8,0xb0,s.resource258DC+0x63,0);o.call1A510(s.c050);s.c050=(s.c050&~0xffu)|((s.c050^1u)&0xffu);o.call291B0(std::uint8_t(s.c050));return 1;}
 }}
}
