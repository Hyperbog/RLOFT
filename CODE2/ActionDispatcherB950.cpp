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
int ActionDispatcherB950::runUiStateBE28(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){
 int r=1;o.begin19CCC(0);s.bf38=0;
 o.event28110(o.resource258D4(0x2d),0xff,0,1);
 s.cf5a[0]=s.cf5a[1]=s.cf5a[2]=1;
 s.saved12ba0=s.d681;
 s.saved12ba4=s.d682;
 s.d682=0;
 s.d681=s.d682;
 s.cf5a[3]=1;
 o.refresh1F494(1);
 o.panel9B4C(ch,5,0);
 s.d681=static_cast<std::uint8_t>(s.saved12ba0);
 s.d682=static_cast<std::uint8_t>(s.saved12ba4);
 if(s.d681!=0)s.d681=2;
 if(s.d682!=0)s.d682=2;
 s.cf5a[0]=s.cf5a[1]=s.cf5a[2]=s.cf5a[3]=1;
 o.refresh1F494(1);
 for(int i=0;i<0x15;++i)o.f238(ch,i,1);
 s.characterCf5a(ch)=1;
 o.finish19CF4();s.bf38=1;return r;
}
int ActionDispatcherB950::runTarget51(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;r=o.effectA254(-1,0x51,3,-1,0x10,ch,1,0);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runTarget6E(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;r=o.effectA254(-1,0x6e,3,-1,0x11,ch,1,0);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runTarget78(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;r=o.effectA254(-1,0x78,3,-1,0x0e,ch,1,0);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runTarget64(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;r=o.effectA254(-1,0x64,4,-1,0x0f,ch,1,0);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runTarget71(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;r=o.effectA254(-1,0x71,4,-1,0x3c,ch,1,0);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runTarget68(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;r=o.effectA254(-1,0x68,4,-1,0x41,ch,1,0);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runTarget82(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;r=o.effectA254(-1,0x82,3,-1,0x26,ch,1,0);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runState7E(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;int v=o.value22E40(4,ch)+3;r=o.state9790(0x7e,ch,0x0a,1,0,v,0x438);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runState7F(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;r=o.state9790(0x7f,ch,0x47,2,1,1,0x2a30);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runState80_9790(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;int v=o.value22E40(4,ch);r=o.state9790(0x80,ch,0x0b,4,2,v,0x438);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runState81(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;r=o.state9790(0x81,ch,-1,-1,-1,-1,-1);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runState82_9790(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;r=o.state9790(0x82,ch,8,-1,-1,-1,-1);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runState83(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;r=o.state9790(0x83,ch,8,-1,-1,-1,-1);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runState84(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;r=o.state9790(0x84,ch,0x23,-1,-1,-1,-1);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runState85(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;int v=o.value22E40(3,ch);int half=(v-(v>>31))/2;r=o.state9790(0x85,ch,0x1f,0x80,7,half+4,0x438);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runState86(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;int v=o.value22E40(4,ch);r=o.state9790(0x86,ch,0x33,0x100,8,v,0x438);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runSelectedFlag80(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){
 int r=1;o.begin19CCC(0);s.bf38=0;
 int e=o.select95C0();
 if(e!=-1){
  o.event28110(o.resource258D4(0x43),0xff,0,1);
  const std::uint16_t type=o.entityType(e);
  if((o.metadataWord(type,0x31)&0x0100u)==0)o.entityByte(e,8)|=0x80;
 }
 o.finish19CF4();s.bf38=1;return r;
}
int ActionDispatcherB950::runSelectedTimed08(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){
 int r=1;o.begin19CCC(0);s.bf38=0;
 int e=o.select95C0();
 if(e!=-1){
  o.event28110(o.resource258D4(0x2a),0xff,0,1);
  const std::uint16_t type=o.entityType(e);
  if((o.metadataWord(type,0x31)&0x0102u)==0){
   o.entityByte(e,8)|=0x08;
   const int base=o.value22E40(4,ch);
   const int roll=o.dice1C328(4,0,0,0);
   o.entityWord(e,0x10)=static_cast<std::uint16_t>((roll+base)*0x438);
  }
 }
 o.finish19CF4();s.bf38=1;return r;
}
int ActionDispatcherB950::runSelectedTimed08x2(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){
 int r=1;o.begin19CCC(0);s.bf38=0;
 int e=o.select95C0();
 if(e!=-1){
  o.event28110(o.resource258D4(0x27),0xff,0,1);
  const std::uint16_t type=o.entityType(e);
  if((o.metadataWord(type,0x31)&0x0108u)==0){
   o.entityByte(e,8)|=0x08;
   const int v=o.value22E40(4,ch);
   o.entityWord(e,0x10)=static_cast<std::uint16_t>(v*0x870);
  }
 }
 o.finish19CF4();s.bf38=1;return r;
}
int ActionDispatcherB950::runSelectedTimed08Mode3(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){
 int r=1;o.begin19CCC(0);s.bf38=0;
 int e=o.select95C0();
 if(e!=-1){
  o.event28110(o.resource258D4(0x28),0xff,0,1);
  const std::uint16_t type=o.entityType(e);
  if((o.metadataWord(type,0x31)&0x0108u)==0){
   o.entityByte(e,8)|=0x08;
   const int v=o.value22E40(3,ch);
   o.entityWord(e,0x10)=static_cast<std::uint16_t>(v*0x870);
  }
 }
 o.finish19CF4();s.bf38=1;return r;
}
int ActionDispatcherB950::runSelectedTimed10(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){
 int r=1;o.begin19CCC(0);s.bf38=0;int e=o.select95C0();
 if(e!=-1){o.event28110(o.resource258D4(0x29),0xff,0,1);const std::uint16_t type=o.entityType(e);
  if((o.metadataWord(type,0x31)&0x0110u)==0){o.entityByte(e,8)|=0x08;const int v=o.value22E40(4,ch);o.entityWord(e,0x10)=static_cast<std::uint16_t>(v*0x438);}}
 o.finish19CF4();s.bf38=1;return r;
}
int ActionDispatcherB950::runDirectA668(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){
 int r=1;o.begin19CCC(0);s.bf38=0;o.directA668(ch);s.characterCf5a(ch)=1;o.finish19CF4();s.bf38=1;return r;
}
int ActionDispatcherB950::runCreate8A80(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch,int mode){
 int r=1;o.begin19CCC(0);s.bf38=0;
 const int direction=o.direction9418(mode==0?0:1);
 o.entityCreate8A80(mode,direction,ch,o.worldY255D4(),o.worldX255D0());
 o.finish19CF4();s.bf38=1;return r;
}
int ActionDispatcherB950::runStateA5(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;r=o.state9790(0xa5,ch,0x38,-1,-1,-1,-1);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runCreate8A80Mode6(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;const int d=o.direction9418(1);o.entityCreate8A80(6,d,ch,o.worldY255D4(),o.worldX255D0());o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runTarget42Event(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;o.event28110(o.resource258D4(0x3b),0xff,0,1);r=o.effectA254(0x4000,0x0e,2,0,0x42,ch,0,0);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runTarget7BEvent(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;o.event28110(o.resource258D4(0x3f),0xff,0,1);r=o.effectA254(0x7b,0x7b,1,0,0x42,ch,1,0);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runMapABC4(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;o.mapABC4(o.direction9418(0));o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runMapAC2C(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;o.mapAC2C(o.direction9418(0));o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runStatus16(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;o.event28110(o.resource258D4(0x16),0xff,0,1);o.statusAD24(-1);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runStatus17(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;o.event28110(o.resource258D4(0x17),0xff,0,1);o.statusAD24(-1);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runTarget57(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;r=o.effectA254(-1,0x57,3,-1,0x10,ch,1,0);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runCharacterTimed02(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){
 int r=1;o.begin19CCC(0);s.bf38=0;
 if((o.characterRawByte(ch,0x14)&2)==0)o.globalFd08()+=1;
 o.event28110(o.resource258D4(0x46),0xff,0,1);
 o.characterRawByte(ch,0x14)|=2;
 const std::uint32_t duration=static_cast<std::uint32_t>(o.value22E40(3,ch))*0x438u;
 const std::uint32_t expiry=static_cast<std::uint32_t>(o.clock19CC4())+duration;
 s.characterCf5a(ch)=1;
 o.characterRawDword(ch,0x5a)=expiry;
 o.finish19CF4();s.bf38=1;return r;
}
int ActionDispatcherB950::runPlayerShift(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;o.playerShiftAE30();o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runNearbyAED8(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;o.nearbyAED8(ch);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runHelper1C77C(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;o.event28110(o.resource258D4(0x15),0xff,0,1);o.helper1C77C(ch,0x31d);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runCharacterTimed10(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){int r=1;o.begin19CCC(0);s.bf38=0;auto &flags=o.characterRawByte(ch,0x14);if((flags&0x10)==0)++o.global6DAE();o.event28110(o.resource258D4(0x13),0xff,0,1);flags|=0x10;s.cf5a[0]=s.cf5a[1]=s.cf5a[2]=s.cf5a[3]=1;o.characterRawDword(ch,0x66)=static_cast<std::uint32_t>(o.clock19CC4()+0x2a30);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runNearbyB518(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){o.begin19CCC(0);s.bf38=0;const int flag=o.character218F5(ch)==1?1:0;o.nearbyB518(ch,flag);o.finish19CF4();s.bf38=1;return -2;}
int ActionDispatcherB950::runAttributeA8A0(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){o.begin19CCC(0);s.bf38=0;const int r=o.attributeA8A0(ch);o.finish19CF4();s.bf38=1;return r;}
int ActionDispatcherB950::runAttributeAA30(ActionDispatcherB950State&s,const ActionDispatcherB950Ops&o,int ch){o.begin19CCC(0);s.bf38=0;const int r=o.attributeAA30(ch);o.finish19CF4();s.bf38=1;return r;}
}
