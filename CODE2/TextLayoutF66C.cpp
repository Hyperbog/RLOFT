#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct TextLayoutF66COps{std::function<int(const char*,int)> measure30288;std::function<std::uint32_t()> random30CA2;std::function<void(int,int,int,int,int)> image2975C;std::function<void(int,const char*,int,int)> text30508;};
void layoutF66C(const TextLayoutF66COps&o,const char*text){
 int w=o.measure30288(text,1)+0x10;w=((w-(w>>31))/2+3)&0xfffc;int left=0xa0-w;if(left%8==0)left-=4;if((left+4)%0x50==0)left-=8;
 const int right=0x140-left,stop=0x138-left;for(int x=left,phase=left+4;x<right;x+=8,phase+=8){int tile;if(x==left)tile=0;else if(x>=stop)tile=5;else if(phase%0x50==0)tile=1;else tile=static_cast<int>(((o.random30CA2()*3u)>>15)+2u);o.image2975C(tile,0x62,x,3,2);}o.text30508(0x65,0,0x140,text);
}
}
