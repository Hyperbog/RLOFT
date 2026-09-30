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
int ActionDispatcherB950::runTarget03(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){
 int result=1;o.begin19CCC(0);s.bf38=0;result=o.effectA254(1,0,0,0,3,ch,0,0x438);o.finish19CF4();s.bf38=1;return result;
}
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
int ActionDispatcherB950::runState04(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){
 int result=1;o.begin19CCC(0);s.bf38=0;o.characterByte(ch,0x12)|=2;int v=o.value22E40(4,ch);std::int32_t duration=((v*7)*32+v)*16;duration=duration*3;o.characterDword(ch,0x1a)=o.clock19CC4()+duration;o.characterCf5a(ch)=1;o.event28110(o.resource258D4(0x34),0xff,0,1);o.refresh31F38(0xf91c);o.finish19CF4();s.bf38=1;return result;
}
int ActionDispatcherB950::runState03(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){
 int result=1;o.begin19CCC(0);s.bf38=0;o.characterByte(ch,0x13)|=4;int v=o.value22E40(3,ch)+10;std::int32_t duration=((v*7)*32+v)*16;duration=duration*3;o.characterDword(ch,0x3e)=o.clock19CC4()+duration;o.characterCf5a(ch)=1;o.event28110(o.resource258D4(0x35),0xff,0,1);o.refresh31F38(0xf91c);o.finish19CF4();s.bf38=1;return result;
}
int ActionDispatcherB950::runStateTimed(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){
 int result=1;o.begin19CCC(0);s.bf38=0;o.characterByte(ch,0x14)|=4;o.characterDword(ch,0x5e)=o.clock19CC4()+0x438;++s.f03c;o.characterCf5a(ch)=1;o.event28110(o.resource258D4(0x14),0xff,0,1);o.finish19CF4();s.bf38=1;return result;
}
int ActionDispatcherB950::runState80(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){
 int result=1;o.begin19CCC(0);s.bf38=0;o.characterByte(ch,0x13)|=0x80;o.characterCf5a(ch)=1;o.event28110(o.resource258D4(0x22),0xff,0,1);o.finish19CF4();s.bf38=1;return result;
}
int ActionDispatcherB950::runTarget42(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;int v=o.value22E40(4,ch);r=o.effectA254(4,2,0,v*5,0x42,ch,0,0x438);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runTarget06(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;int v=o.value22E40(4,ch)+3;r=o.effectA254(8,3,1,v,6,ch,0,0x438);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runTarget25(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;int v=o.value22E40(4,ch)+3;r=o.effectA254(16,4,1,v,0x25,ch,0,0x438);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runTarget18(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;int v=o.value22E40(4,ch)*10;r=o.effectA254(0x80,7,0,v,0x18,ch,0,0x2a30);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runTarget45(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;r=o.effectA254(0x40,6,0,0,0x45,ch,0,0x438);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runTarget05(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;r=o.effectA254(0x100,8,2,6,5,ch,0,0x438);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runTarget30(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;r=o.effectA254(0x200,9,1,6,0x30,ch,0,0x438);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runA794(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;o.directA794(ch);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runTarget44(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;int v=o.value22E40(3,ch);r=o.effectA254(0x10000,0x10,4,v,0x44,ch,0,0x438);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runTarget02(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;int v=o.value22E40(3,ch)+1;r=o.effectA254(0x800,0x0b,3,v,2,ch,0,0x438);o.finish19CF4();s.bf38=1;return r;}
}
