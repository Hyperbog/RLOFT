#include "PackedOverlay5984.h"
namespace ravenloft::game {
void PackedOverlay5984::show(PackedOverlay5984State&s,const PackedOverlay5984Ops&o,int a,int b){
 if(s.visible6460)return;s.visible6460=1;o.call325B0(0xC054,0,a,0x390,0x100);o.call291B0(std::uint8_t(s.c050));o.call2E510(0,0,b,0);o.call2E510(0xC8,0,b+1,0);o.call1A510(s.c050);s.c050=(s.c050&~0xffu)|((s.c050^1u)&0xffu);o.call29234(0xC054);
}
void PackedOverlay5984::hide(PackedOverlay5984State&s,const PackedOverlay5984Ops&o,int a){
 if(!s.visible6460)return;std::uint8_t tmp[4]{0x73,0x3A,0};s.visible6460=0;o.call325B0(0xC054,0,a,0x390,0x100);o.call32468(0xC054,0,0xEC,6,3,tmp);o.call29234(0xC054);o.call1A5B0(s.c050,s.v8600,s.v8602);s.c050=(s.c050&~0xffu)|((s.c050^1u)&0xffu);
}
}
