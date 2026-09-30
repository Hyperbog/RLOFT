#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct StreamCommandOps{
 std::function<void(void*,int,int)> read;
 std::function<void(int)> loadBlock,event,sound,resource;
 std::function<void()> clear,present,tick;
 std::function<void(int,int,int)> setPosition;
 std::function<void(int)> setPalette;
 std::function<void(int)> animate;
 std::function<void()> timed;
};
void command135C4(const StreamCommandOps&o,std::uint16_t op,std::int16_t a,std::int16_t b,int context){
 switch(op){
 case 0:o.loadBlock(0x300);break;
 case 1:o.clear();break;
 case 2:o.present();break;
 case 3:o.tick();break;
 case 4:o.timed();break;
 case 5:o.present();break;
 case 6:{int v=a;if(v==0x15&&context==0xb)v=0x30;o.setPosition(v,b+1,0x9c4);break;}
 case 7:if(a!=3&&a!=0xf&&a!=0x88)o.setPalette(a);break;
 case 8:o.animate(a);break;
 case 9:if(a==0)o.clear();else o.resource(a+0x324);break;
 default:break;
 }
}
struct Playback13834Ops{
 std::function<std::uint16_t()> frameCount,commandCount;
 std::function<void()> command,prepare,finish;
 std::function<int()> key;
 std::function<void()> pump,flip;
};
int playback13834(const Playback13834Ops&o){
 int abort=0;for(std::uint16_t f=0;f<o.frameCount();++f){o.prepare();for(std::uint16_t i=0;i<o.commandCount();++i){o.command();o.pump();}
  o.flip();if(o.key()==0x1b){abort=1;break;}}
 o.finish();return abort;
}
struct Scene139ACOps{
 std::function<void(int)> configure;
 std::function<void()> start;
 std::function<int()> openStream;
 std::function<void(int)> allocStream;
};
void scene139AC(const Scene139ACOps&o,int scene){
 o.configure(scene);o.start();int h=o.openStream();(void)h;o.allocStream(0x1f400);
}
}
