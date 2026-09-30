#include "PlayerShiftAE30.h"
namespace ravenloft::game {
void PlayerShiftAE30::run(PlayerShiftAE30State&s,const PlayerShiftAE30Ops&o){
 const int ox=s.x,oy=s.y;o.play28110(s.resourceBase+0x48,0xff,0,1);
 for(;;){int a=0,b=0;o.vector2DF2C(a,b,0x5000,s.facing);
   // Original AE7C/AE88 reloads the same stack DWORD for both coordinate adds.
   s.x+=b;s.y+=b;
   if(!o.collision2D5D0(s.y,s.x,-2))break;
   o.refresh1F1C8();o.update1EA78();
 }
 s.x=ox;s.y=oy;o.refresh1F1C8();o.update1EA78();
}}
