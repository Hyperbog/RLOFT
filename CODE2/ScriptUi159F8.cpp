#include <array>
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct ScriptUiOps{
 std::function<std::uint16_t()> next;std::function<std::uint32_t()> random;
 std::function<void(int,int,int,int)> draw;std::function<void(int,int,int,int)> drawAlt;
 std::function<void(int)> resourceOn,resourceOff;std::function<int()> poll;
 std::function<void(int,int)> select;std::function<void()> refresh,cleanup;
};
struct TimedScriptEntry{std::int32_t id=-1;std::int32_t interval=0;std::uint32_t due=0;};
void timedEntry159F8(const ScriptUiOps&o,TimedScriptEntry*e){int n=o.next();e[n].id=o.next();e[n].interval=o.next();e[n].due=e[n].interval+o.random();if(e[n].id==0xffff)e[n].id=-1;}
void drawScript15A82(const ScriptUiOps&o,int base){int off=o.next(),x=o.next(),y=o.next();o.draw(x,y,base+off,9);}
void drawScript15ADF(const ScriptUiOps&o,int base){o.resourceOn(1);int off=o.next(),x=o.next(),y=o.next();o.drawAlt(x,y,base+off,9);o.resourceOff(1);}
// 15B75-15E36 is the family of script-driven draw/resource opcodes. Each
// consumes WORD operands directly from the 0x13020 stream and returns zero.
void resourceOpcode15B75(const ScriptUiOps&o,int base,bool alternate){int off=o.next(),x=o.next(),y=o.next();if(alternate)o.drawAlt(x,y,base+off,9);else o.draw(x,y,base+off,9);}
struct ScriptRunnerOps{
 std::function<int()> step;std::function<void()> service,redraw,cleanup;
 std::function<void(int,int)> select;std::function<int()> mouseX,mouseY,buttons;
 std::uint32_t& ip;std::uint16_t* menuPairs;int& menuCount;
};
// 15E38-16089 maintains the four-entry script menu and the selected pair.
void selectMenu15E38(const ScriptRunnerOps&o,int index,int mode){if(index<0||index>=o.menuCount)return;o.select(index,mode);}
// 1608C-1624A: modal script menu. Input is drained before entry, mouse rows
// map to the 0x7FD8 pairs, and selecting a row temporarily switches the script
// instruction pointer to that row's target before restoring presentation state.
void modalMenu1608C(const ScriptRunnerOps&o){
 int selected=-1,last=-1;while(o.service(),o.buttons()){}
 for(;;){o.service();int b=o.buttons();if(b&2)break;int x=o.mouseX(),y=o.mouseY();int n=-1;
  if(x>=0xdd&&o.menuCount>0){n=(y-0x17)/0x16;if(n<0||n>=o.menuCount||o.menuPairs[n*2]==0xffff)n=-1;}
  if(n!=last){o.select(n,1);o.redraw();last=n;}
  if(b&&n>=0){o.select(n,1);o.ip=o.menuPairs[n*2+1];while(o.step()==0){}o.redraw();last=-1;}
 }
 o.cleanup();
}
struct ObjectSweepOps{std::function<void(int,int,int,int,int,int,int)> spawn;std::function<void(void*,const void*,int)> move;};
// 1624C-1635B: when a 0x16-byte world record has type 0x4E, emit the five
// original object ids and compact the remaining record array by one element.
void sweepObjects1624C(const ObjectSweepOps&o,std::uint8_t*records,int&count){for(int i=0;i<count;++i){auto*r=records+i*0x16;if(*reinterpret_cast<std::uint16_t*>(r+0x0e)!=0x4e)continue;auto x=*reinterpret_cast<std::int32_t*>(r),y=*reinterpret_cast<std::int32_t*>(r+4);for(int id:{0x52,0x5c,0x5f,0x66,0x69})o.spawn(0x80,id,0,y,x,0,0);o.move(r,r+0x16,(count-i+1)*0x16);return;}}
struct ScriptController1635COps{std::function<int()> step;std::function<void()> resetUi,modal,objectSweep;std::function<int(int)> flag;std::uint32_t&ip;int scene;};
void controller1635C(const ScriptController1635COps&o){o.ip=0;int r;do r=o.step();while(r!=2&&r!=3);if(r!=3){auto save=o.ip;o.resetUi();o.modal();o.ip=save;while(o.step()!=3){}}if(o.scene==0x13&&o.flag(0x2e)==1)o.objectSweep();}
}
