#include "ConfigMenu6548.h"
namespace ravenloft::game {
void ConfigMenu6548::run(ConfigMenu6548State&s,const ConfigMenu6548Ops&o){
 while(o.input1A1F4()){} while(o.input1A1F4()){}
 while((o.buttons9E35()&2)==0){int y=o.mouseY9E33(),x=o.mouseX9E31();if(y<0x78)continue;int dx=x-0x0f,rem=dx%0x1e,col=dx/0x1e;if(rem>=0x19||unsigned(col)>9)continue;
  switch(col){case 0:o.slider62AC();break;case 1:if(y<0xa8)s.c026^=1;break;case 2:if(y<0xa8)s.c027^=1;break;case 3:if(y<0xa8)s.c028^=1;break;case 4:if(y<0xa8)s.c029^=1;break;case 5:o.slider6444();break;case 6:if(y<0xa8){s.c02b^=1;o.call28098(s.c02b);}break;case 7:if(y<0xa8){s.c02c^=1;o.call290A8(s.c02c);if(s.c02c)o.call28E58(s.value258B4,1,0x9c4);}break;case 8:o.slider6354();break;case 9:if(y<0xa8)o.action20994();break;}
  o.draw5EEC();o.call1A510(s.c050);o.flip1A9F8();s.c050=(s.c050&~0xffu)|((s.c050^1u)&0xffu);
 }
 for(int i=0;i<9;++i)o.free30040(s.surfaces[i]);
}}
