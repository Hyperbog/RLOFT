#include "DirectionPicker9418.h"
namespace ravenloft::game {
int DirectionPicker9418::run(DirectionPicker9418State&s,const DirectionPicker9418Ops&o,bool relative){
 if(s.c02f){return relative?s.facingC658:0x80;}o.message();o.call1C258();o.flip1A9F8();
 for(;;){while(o.input1A1F4()){}while(!o.input1A1F4()){}int x=o.mouseX9E31(),y=o.mouseY9E33();if(x<4||x>=0x104||y<0x20||y>=0x120)continue;int v;
  if(relative){int q=(y-0xa0)/3;v=s.facingC658-q;if(v<0)v+=0x168;if(v>=0x168)v-=0x168;}else v=y-0x20;
  o.call1C2CC();o.call22D70();return v;}
}}
