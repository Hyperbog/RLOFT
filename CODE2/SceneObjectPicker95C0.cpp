#include "SceneObjectPicker95C0.h"
namespace ravenloft::game {
int SceneObjectPicker95C0::run(SceneObjectPicker95C0State&s,const SceneObjectPicker95C0Ops&o){
 int freeSlot=0;while(freeSlot<=int(s.oppositeCount)&&o.slotOccupied(freeSlot))++freeSlot;if(freeSlot==int(s.oppositeCount)){o.prompt();o.call1C258();return -1;}
 o.prompt();o.call1C258();o.flip1A9F8();int picked=-1;bool cancel=false;
 for(;;){int ev=o.input1A1F4();if(ev&&o.key30B40()==0x1b){cancel=true;break;}if(!ev&&o.key30B40()==0x1b){cancel=true;break;}if(s.mouseButtons&2)break;
  int x=o.mouseX9E31(),y=o.mouseY9E33();if(x>=4&&x<0x104&&y>=0x20&&y<0x120){picked=o.hit94E0(x,y-0x20);if(picked!=-1)break;}}
 o.call1C2CC();o.call22D70();return cancel?-1:picked;
}}
