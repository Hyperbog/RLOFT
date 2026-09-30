#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct PartyRestoreOps{
 std::function<std::uint16_t(std::uint32_t&)> word;
 std::function<void(int)> select,clearStatus,recalc;
 std::function<void(int,int)> applySlot;
 std::function<void(int,int,int,int,int,int,int)> spawn;
 std::function<int(int)> blocked;
 std::function<void(int,int)> copyTemplate;
 std::function<void()> rebuild;
 std::uint32_t&ip;std::uint16_t*flags;std::int16_t*slots;std::uint8_t*state;
};
void restoreCharacter148F0(const PartyRestoreOps&o,int ch){
 int src=o.word(o.ip);o.copyTemplate(ch,src);o.recalc(ch);o.state[ch*0x248+0x131]=0;
 if((signed char)o.state[ch*0x248+0x15]==1){o.state[ch*0x248+0x131]|=4;o.applySlot(ch,0);}
 for(int k=0;k<3;++k)if((signed char)o.state[ch*0x248+0x15+k]==3){o.state[ch*0x248+0x131]|=1;o.applySlot(ch,k);}
 o.flags[ch]|=0x80;
}
int findAvailable14A60(const PartyRestoreOps&o,int token){for(int ch=0;ch<4;++ch)if(!o.blocked(token)&&((o.flags[ch]&0x80)!=0))return ch;return -1;}
// 14AB2-14ADB is the ten-entry dispatch table used by 14ADC.
void partyAction14ADC(const PartyRestoreOps&o,int ch,const std::function<void(int)>&event,const std::function<void(int,int)>&action){
 for(int i=0;i<10;++i){if(o.blocked(i))continue;if((o.flags[ch]&0x4000)==0)action(ch,i);event(i+4);}
}
void spawnInventory14C40(const PartyRestoreOps&o,int ch){
 for(int i=0;i<21;++i){auto item=o.slots[ch*21+i];if(item!=-1&&item!=0xa6)o.spawn(0x80,item,i,ch,1,0,0);}
}
struct PartySelect14CA4Ops{
 std::function<int(int)> findAvailable;std::function<void()> begin,end;
 std::function<int()> pollX,pollY,pollButtons;
 std::function<void(int)> drawCharacter,restoreCharacter,clearStatus;
 std::function<void(int)> detach,reattach;
};
int selectCharacter14CA4(const PartySelect14CA4Ops&o){
 int chosen=o.findAvailable(0x6c8);o.begin();if(chosen<0)chosen=0;
 for(;;){o.drawCharacter(chosen);if(o.pollButtons()&2)break;int x=o.pollX(),y=o.pollY();
  if(x>=0x136&&x<0x166&&y>=0x0d&&y<0xad){int n=(y-0x0d)/0x50;if(n>=0&&n<4){chosen=n;break;}}}
 o.detach(chosen);o.clearStatus(chosen);o.restoreCharacter(chosen);o.reattach(chosen);o.end();return chosen;
}
struct RestoreInventory14E9COps{
 std::function<void(int,int,int)> install;std::function<void(int,int,int,int,int,int,int)> spawn;
 std::int16_t*saved;std::uint32_t savedFlags;
};
void restoreInventory14E9C(const RestoreInventory14E9COps&o,int ch){
 for(int i=0;i<21;++i){int item=o.saved[i];if(item==-1||item==0xa6)continue;int bit=(o.savedFlags>>i)&1;
  if(i<12)o.install(ch,i,bit);else o.spawn(0x80,item,bit,0,0,1,0);}
}
struct EnsureParty14FC0Ops{
 std::function<bool(int)> active;std::function<void(int)> restoreCharacter,clearStatus,restoreInventory;
 std::function<int()> selectCharacter;
 std::function<void(int)> markRedraw;
};
void ensureParty14FC0(const EnsureParty14FC0Ops&o){
 int ch=0;for(;ch<4;++ch)if(o.active(ch)){o.restoreCharacter(ch);break;}
 if(ch==4){ch=o.selectCharacter();o.clearStatus(ch);o.restoreCharacter(ch);o.restoreInventory(ch);o.markRedraw(ch);}
}
}
