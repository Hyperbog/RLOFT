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
int ActionDispatcherB950::runSelectedEntity(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){
 int result=1;o.begin19CCC(0);s.bf38=0;int i=o.select95C0();if(i==-1)result=0;else{o.e578(ch);int r=o.dice1C328(3,10,0,0);o.effectB02C(-180,180,7,r,0x900,1,9,0x2c,o.entityY(i),o.entityX(i));}o.finish19CF4();s.bf38=1;return result;
}
}
int ActionDispatcherB950::run0A(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){
 int result=1;o.begin19CCC(0);s.bf38=0;int a=o.direction9418(1);o.e578(ch);int r=o.dice1C328(3,6,0,0);int d=a-s.facing;o.effectB02C(d,d,20,r,0x2100,0,8,0,s.playerX,s.playerY);o.finish19CF4();s.bf38=1;return result;
}
int ActionDispatcherB950::run1F(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){
 int result=1;o.begin19CCC(0);s.bf38=0;int i=o.select95C0();if(i!=-1)o.burstB410(i);else result=0;o.finish19CF4();s.bf38=1;return result;
}
int ActionDispatcherB950::run35(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){
 int result=1;o.begin19CCC(0);s.bf38=0;int a=o.direction9418(1);o.e578(ch);int r=o.dice1C328(3,6,0,0);int d=a-s.facing;o.effectB02C(d,d,20,r,0x900,0,9,0x3d,s.playerX,s.playerY);o.finish19CF4();s.bf38=1;return result;
}
}
