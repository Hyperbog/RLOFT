#include <array>
#include <cstdint>
#include <functional>
namespace ravenloft::game {
// 13C62-13C87: nine-entry near-pointer dispatch table consumed by 13C88.
struct Transition13C88Ops{
 std::function<void()> closeInput,prepare,wait,flush,cleanupA,cleanupB;
 std::function<void(int)> event,loadMap,play;
 std::function<int()> pending;
 std::function<void(int,int,int)> sound;
 std::int16_t &partyX,&partyY;std::uint16_t &facingA,&facingB;std::int32_t &sceneState;
};
void transition13C88(const Transition13C88Ops&o,int id,int a,int b){
 o.closeInput();o.prepare();if(id==10||id==11)o.play(0xec1c);o.wait();o.flush();
 int sound=-1;
 switch(id){
 case 1:o.event(0x2b);break;
 case 2:o.partyX=0x16;o.partyY=0x0d;o.facingA=o.facingB=0x8000;o.sceneState=0;o.loadMap(12);sound=-2;break;
 case 3:o.partyX=0x17;o.partyY=0x17;o.facingA=o.facingB=0x8000;o.sceneState=0x10e;o.loadMap(12);sound=-2;break;
 case 4:o.partyX=2;o.partyY=0x41;o.facingA=o.facingB=0x8000;o.sceneState=0x10e;o.loadMap(11);o.event(0x25);o.play(0x6e);sound=-2;break;
 case 9:o.partyX=0x15;o.partyY=0x2b;o.sceneState=0xb4;break;
 default:break;}
 if(id&&id!=12){if(sound!=-2&&o.pending()!=-1)o.sound(sound,a,b);o.cleanupA();}o.cleanupB();
}
struct TextWrap13FD4Ops{std::function<int(int,int)> glyphWidth;std::function<void(const char*,int,int,int)> draw;};
int textWrap13FD4(const TextWrap13FD4Ops&o,char*text,int x,int y,int maxWidth,int font){
 int lines=0;char*p=text;while(*p){while(*p==' ')++p;char*line=p;char*last=line;int width=0,space=0;
  for(;;){char*c=p;int word=0;while(*p&&*p!=' '&&*p!=1){word+=o.glyphWidth((signed char)*p,font);++p;}
   if(width+space+word>maxWidth){last=c-1;break;}width+=space+word;if(!*p||*p==1){last=p;break;}space=o.glyphWidth(' ',font);++p;}
  char save=*last;*last=0;o.draw(line,x,y,font);++lines;*last=save;if(!save)break;y+=1;p=last+1;}
 return lines;
}
struct ScriptCondition1430COps{std::function<std::uint8_t(int)> byte;};
std::uint16_t condition1430C(const ScriptCondition1430COps&o,const std::uint16_t*script,std::uint32_t&ip,int ch){
 std::uint16_t result=0;bool latched=false;
 for(;;){auto op=script[ip++];if(op==0x21)return result;bool hit=false;
  switch(op){case 0:hit=o.byte(ch+0x1a)==0;break;case 1:hit=o.byte(ch+0x1a)==2;break;case 2:hit=o.byte(ch+0x1a)==4;break;
  case 3:hit=o.byte(ch+0x1a)==1;break;case 4:hit=o.byte(ch+0x1a)==3;break;case 5:hit=o.byte(ch+0x1a)==5;break;
  case 6:hit=o.byte(ch+0x3d)>0x10;break;case 7:hit=o.byte(ch+0x3d)<0x0a;break;case 8:hit=o.byte(ch+0x33)>0x10;break;case 9:hit=o.byte(ch+0x33)<0x0a;break;
  case 10:hit=o.byte(ch+0x39)>0x10;break;case 11:hit=o.byte(ch+0x39)<0x0a;break;case 12:hit=o.byte(ch+0x35)>0x10;break;case 13:hit=o.byte(ch+0x35)<0x0a;break;
  case 14:hit=o.byte(ch+0x37)>0x10;break;case 15:hit=o.byte(ch+0x37)<0x0a;break;case 16:hit=o.byte(ch+0x3b)>0x10;break;case 17:hit=o.byte(ch+0x3b)<0x0a;break;
  case 18:hit=o.byte(ch+0x14)==0;break;case 19:hit=o.byte(ch+0x14)==5;break;case 20:hit=o.byte(ch+0x14)==1;break;case 21:hit=o.byte(ch+0x14)==4;break;case 22:hit=o.byte(ch+0x14)==3;break;case 23:hit=o.byte(ch+0x14)==2;break;default:break;}
  std::uint16_t target=script[ip++];if(hit&&!latched){latched=true;result=target;}}
}
void modal146B8(const std::function<void(int)>&mode,const std::function<void(int)>&action,int v){mode(10);action(v);mode(6);}
}
