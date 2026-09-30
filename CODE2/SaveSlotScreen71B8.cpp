#include "SaveSlotScreen71B8.h"
namespace ravenloft::game {
void SaveSlotScreen71B8::draw(SaveSlotScreen71B8State&s,const SaveSlotScreen71B8Ops&o,int listArg){
 o.call291B0(std::uint8_t(s.c050));int tile=s.resource258DC+0x35;
 for(int y=0;y<0x190;y+=0x10)for(int x=0;x<0x140;x+=8){auto r=o.call30CA2(y,tile,x,y);o.call2975C(int((r*4u)>>15),x,tile,y,0);}
 for(int x=8;x<0x138;x+=8){auto r=o.call30CA2(0,tile,x,0);o.call2975C(int((r*2u)>>15)+9,x,tile,0,0);r=o.call30CA2(0x180,tile,x,0);o.call2975C(int((r*2u)>>15)+5,x,tile,0x180,0);}
 for(int y=0x10;y<0x180;y+=0x10){auto r=o.call30CA2(y,tile,0,0);o.call2975C(int((r*2u)>>15)+0x0c,y,tile,0,0);r=o.call30CA2(y,tile,0x138,0);o.call2975C(int((r*2u)>>15)+0x0e,y,tile,0x138,0);}
 o.call2975C(8,0,0,tile,0);o.call2975C(0x0b,0,0x138,tile,0);o.call2975C(4,0x180,0,tile,0);o.call2975C(7,0x180,0x138,tile,0);
 o.build7070(listArg);o.call1A510(s.c050);s.c050=(s.c050&~0xffu)|((s.c050^1u)&0xffu);o.call1A9F8();o.call291B0(std::uint8_t(s.c050));
}}
