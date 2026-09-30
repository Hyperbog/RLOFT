#include "PartyStatusHelpersA5BC.h"
namespace ravenloft::game {
static std::uint16_t rw(const std::uint8_t*p,int n){return std::uint16_t(p[n])|(std::uint16_t(p[n+1])<<8);}
static std::uint32_t& dw(std::uint8_t*p,int n){return *reinterpret_cast<std::uint32_t*>(p+n);}
static void dirty(PartyStatusHelpersA5BCState&s){for(auto&v:s.refresh)v=1;}
void PartyStatusHelpersA5BC::saveUi(PartyStatusHelpersA5BCState&s,const PartyStatusHelpersA5BCOps&o){dirty(s);s.savedD681=s.d681;s.savedD682=s.d682;s.d681=s.d682=0;o.ui1F494(1);}
void PartyStatusHelpersA5BC::restoreUi(PartyStatusHelpersA5BCState&s,const PartyStatusHelpersA5BCOps&o){s.d681=s.savedD681?2:0;s.d682=s.savedD682?2:0;dirty(s);o.ui1F494(1);}
void PartyStatusHelpersA5BC::installParty20(PartyStatusHelpersA5BCState&s,const PartyStatusHelpersA5BCOps&o,int ch){
 o.playResource(s.resourceBase+0x3e,0xff,0,1);saveUi(s,o);o.render9B4C(ch,2,1);restoreUi(s,o);
 for(int i=0;i<4;++i){auto*c=s.characters+i*0x248;if(rw(c,0x10)&0x8000)continue;s.refresh[i]=1;dw(c,0x134)|=0x20;dw(c,0x18a)=std::uint32_t(o.clock()+o.value22E40(3,ch)*0x438);}
}
void PartyStatusHelpersA5BC::installAll20(PartyStatusHelpersA5BCState&s,const PartyStatusHelpersA5BCOps&o,int ch){
 o.playResource(s.resourceBase+0x30,0xff,0,1);saveUi(s,o);o.render9B4C(ch,2,1);restoreUi(s,o);
 auto*selected=s.characters+ch*0x248;
 for(int i=0;i<4;++i){auto*c=s.characters+i*0x248;if(rw(c,0x10)&0x8000)continue;s.refresh[i]=1;dw(selected,0x132)|=0x20;}
}
}
