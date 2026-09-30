#include "MainMenu80A8.h"
namespace ravenloft::game {
void MainMenu80A8::run(MainMenu80A8State&s,const MainMenu80A8Ops&o){
 if(o.mouseX9E31()>=0x40){s.a6c2=0;return;}
 s.a79d=1;o.call19CCC(0);o.flip1A9F8();auto oldD680=s.d680;s.d680=3;o.call291B0(std::uint8_t(s.c050));
 s.surface=o.alloc30070(0,0,0x40,0x140,0xff);bool redraw=true;
 while(o.mouseX9E31()<0x40){
  int selected=o.mouseY9E33()/0x28;
  if(redraw){o.call291B0(std::uint8_t(s.c050));for(int i=0;i<8;++i){int y=i*0x28;o.draw2975C(i==selected,y,0,s.resource258DC+0x49,0);o.draw30508(0x1b,y,0x28,int(s.labels60F0[i]),1);}o.present1A8C8(s.c050,0,0,0x40,0x140);redraw=false;}
  while(!o.input1A1F4()&&o.mouseX9E31()<0x40){int n=o.mouseY9E33()/0x28;if(n==selected)continue;o.call291B0(std::uint8_t(s.c050));int oy=selected*0x28,ny=n*0x28;o.draw2975C(0,oy,0,s.resource258DC+0x49,0);o.draw30508(0x1b,oy,0x28,int(s.labels60F0[selected]),1);o.draw2975C(1,ny,0,s.resource258DC+0x49,0);o.draw30508(0x1b,ny,0x28,int(s.labels60F0[n]),1);o.present1A8C8(s.c050,oy,0,0x40,0x28);o.present1A8C8(s.c050,ny,0,0x40,0x28);selected=n;}
  if(!o.input1A1F4())continue;int n=o.mouseY9E33()/0x28;if(unsigned(n)>7)continue;
  switch(n){
   case 0:o.call461C();while(o.input1A1F4()){}break;
   case 1:{s.flag6460=0;auto y=s.value8600;auto b=s.value8602;o.call1A344(-1,0);o.call5C84();o.call1A344(y,b);redraw=true;break;}
   case 2:{s.flag6460=0;auto y=s.value8600;auto b=s.value8602;o.call1A344(-1,0);o.call5D34();o.call1A344(y,b);redraw=true;break;}
   case 3:o.call3634(0);redraw=true;break;
   case 4:o.load7B94();o.call1F8FC();redraw=true;break;
   case 5:s.a79d=0;o.call171FC(s.c6c8);s.a79d=1;o.save79E0();o.call1F8FC();redraw=true;break;
   case 6:if(o.confirm7D44(0)){o.call205B4(-1);}break;
   case 7:o.config6548();o.config5DD0();o.call1F8FC();redraw=true;break;
  }
 }
 o.call291B0(std::uint8_t(s.c050));o.call2939C(s.surface,0,0,0);o.free30040(int(s.surface));o.present1A8C8(s.c050,0,0,0x40,0x140);o.call22D70();s.a79d=0;s.a6c2=0;s.d680=oldD680;o.call19CF4();s.value1457c=0;
}}
