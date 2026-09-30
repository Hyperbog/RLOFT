#include "SaveSlotController79E0.h"
namespace ravenloft::game {
int SaveSlotController79E0::run(SaveSlotController79E0State&s,const SaveSlotController79E0Ops&o,char*labels,const char*cur,const char*aux,int msgAux,int msgCopy){
 o.draw71B8(int(s.resource258DC+0x41));while(o.input1A1F4()){}
 for(;;){while(!o.input1A1F4()){}if((o.buttons9E35()&2)!=0)return 2;int x=o.mouseX9E31(),y=o.mouseY9E33();if(x<0x15||x>=0x17b||y<0x14||y>=0x5c)continue;int q=(x-0x15)/0x25,r=(x-0x15)%0x25;if(r>0x19)continue;
  int px=q*0x25+0x15;o.call291B0(std::uint8_t(s.c050));o.draw2975C(q*2+1,px,0x14,s.resource258DC+0x41,0);o.present1A8C8(s.c050,px,0x14,0x19,0x48);
  char*label=labels+q*0x32;if(!o.edit75CC(label,q))goto cleanup;int slot=q+1;if(o.save66E8(slot)!=0){cleanup: for(auto p:s.surfaces)o.free30040(int(p));return 1;}
  s.path6690[0]=char(slot/10+'0');s.path6690[1]=char(slot%10+'0');s.path6690[2]=0;o.call33A38(s.path6690);if(o.exists33A5A(s.path6690))goto cleanup;
  o.disk7378();if(o.exists33A5A(aux))o.call2F580(msgAux);int rc=o.copy73A0(cur,s.path6690);s.path6690[2]='\\';for(auto p:s.surfaces)o.free30040(int(p));return rc;
 }}
}
