#include "ConfigPanel5EEC.h"
namespace ravenloft::game {
void ConfigPanel5EEC::draw(const ConfigPanel5EECState&s,const ConfigPanel5EECOps&o){
 o.call291B0(static_cast<std::uint8_t>(s.c050));
 int x=0x17,slot=0x0F;
 for(int i=0;i<10;++i,x+=0x1E,slot+=0x1E){
  o.call3060C(x,0x0F,s.art6110[i],1);
  const int y=s.resource258DC+0x40;
  int a=0,b=slot,c=y,d=0;
  switch(i){
   case 0:a=0;b=slot;c=0x78+int(s.c025)*10;break;
   case 1:a=int(s.c026)+2;b=slot;c=0x78;break;
   case 2:a=int(s.c027)+2;b=slot;c=0x78;break;
   case 3:a=int(s.c028)+2;b=slot;c=0x78;break;
   case 4:a=int(s.c029)+2;b=slot;c=0x78;break;
   case 5:a=0;b=slot;c=0x78+int(s.c02a);break;
   case 6:a=int(s.c02b)+2;b=slot;c=0x78;break;
   case 7:a=int(s.c02c)+2;b=slot;c=0x78;break;
   case 8:a=0;b=slot;c=0x78+int(s.c02e)*10;break;
   case 9:a=int(s.c02f)+2;b=slot;c=0x78;break;
  }
  o.call2975C(a,b,c,y,d);
 }
}
}
