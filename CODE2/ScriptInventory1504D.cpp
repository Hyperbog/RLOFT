#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct ScriptInventoryOps{
 std::function<int(int)> flagTest; std::function<void(int)> flagSet,flagClear,flagToggle;
 std::function<void(int,int)> detach; std::function<void(int,int,int)> setSlotBit;
 std::function<void(int,int,int,int,int,int,int)> spawn;
 std::function<bool(int)> contains; std::function<void(int)> removeAnywhere;
 std::function<void()> refresh; std::int16_t* partySlots; std::uint16_t* pairTable;
};
// 15050-15142. 0x90 is normalized to 0x94. Special ids 0x78/0x9b first
// consume their corresponding global flag, then the item is installed into the
// first free slot of an active party member or emitted through 2195C.
void giveItem15050(const ScriptInventoryOps&o,int item){
 if(item==0x90)item=0x94;
 if(item==0x78&&o.flagTest(0x58)==1)return; if(item==0x78)o.flagSet(0x58);
 if(item==0x9b&&o.flagTest(0x59)==1)return; if(item==0x9b)o.flagSet(0x59);
 for(int ch=0;ch<4;++ch){for(int s=0;s<12;++s)if(o.partySlots[ch*21+s]==-1){o.partySlots[ch*21+s]=(std::int16_t)item;o.setSlotBit(ch,s,0);return;}}
 o.spawn(0x80,item,0,0,1,0,0);
}
void spawnItem15144(const ScriptInventoryOps&o,int item){o.spawn(0x80,item,0,0,1,0,0);}
struct PairQueueOps{std::uint16_t* q;std::uint32_t& count;std::function<std::uint16_t()> nextWord;std::function<void()> refresh;};
void queueAdd15168(const PairQueueOps&o,bool block){if(!block){for(int i=0;i<4;++i)if(o.q[i*2]==0xffff){o.q[i*2]=o.nextWord();o.q[i*2+1]=o.nextWord();break;}}else{o.q[0]=o.nextWord();o.q[1]=o.nextWord();}o.refresh();}
void queueRemove1520C(const PairQueueOps&o){auto key=o.nextWord();for(int i=0;i<4;++i)if(o.q[i*2]==key){for(int j=i;j<3;++j){o.q[j*2]=o.q[(j+1)*2];o.q[j*2+1]=o.q[(j+1)*2+1];}o.q[6]=0xffff;--o.count;break;}o.refresh();}
// 15280 onward mutates the live 0x14D8B object records using one script word.
struct ScriptStateOps{std::function<std::uint16_t()> next;std::function<int(int)> test;std::function<void(int)> set,clear,toggle;std::function<bool(int)> contains;std::function<void(int)> remove;};
int branchIf153FC(const ScriptStateOps&o,bool condition){auto target=o.next();if(condition){return target;}return -1;}
void opSet15983(const ScriptStateOps&o){o.set(o.next());}
void opClear159AA(const ScriptStateOps&o){o.clear(o.next());}
void opToggle159D1(const ScriptStateOps&o){o.toggle(o.next());}
}
