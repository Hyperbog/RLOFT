#include "ActionDispatcherB950.h"
namespace ravenloft::game {
int ActionDispatcherB950::run01(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){
 int result=1;
 o.begin19CCC(0);
 s.bf38=0;
 o.e578(ch);
 const int v=o.value22E40(4,ch);
 const int extra=o.dice1C328(1,3,0,0);
 o.effectB02C(-60,60,7,extra+2*v,0x2100,1,8,7,s.playerX,s.playerY);
 o.finish19CF4();
 s.bf38=1;
 return result;
}
}
