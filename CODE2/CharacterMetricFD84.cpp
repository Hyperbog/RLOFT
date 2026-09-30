#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct CharacterMetricOps{std::function<std::int16_t(int,int)> slot;std::function<std::uint8_t(int,int)> itemByte,charByte;std::function<int(int)> weightF5F8;std::function<std::uint16_t(int,int)> table7E8C,table7E94;};
int carriedFD84(const CharacterMetricOps&o,int ch){int sum=0;for(int s=0;s<21;++s){int item=o.slot(ch,s);if(item==-1)continue;sum+=o.itemByte(item,0x30);if(o.itemByte(item,0x2f)==1)sum+=o.weightF5F8(static_cast<std::int8_t>(o.itemByte(item,0x31)));}return (sum-(sum>>31))/2;}
int classTableFDE4(const CharacterMetricOps&o,int ch){int base=o.charByte(ch,0x112),v=o.charByte(ch,0x11e);if(v>=1&&v<=0x32)base+=1;else if(v<=0x4b)base+=2;else if(v<=0x5a)base+=3;else if(v<=0x63)base+=4;else if(v==0x64)base+=5;return o.table7E94(base-2,0);}
int loadTierFE44(const CharacterMetricOps&o,int ch){int cls=o.charByte(ch,0x113),v=o.charByte(ch,0x11f);if(cls==0x12){if(v>=1&&v<=0x32)cls+=1;else if(v<=0x4b)cls+=2;else if(v<=0x5a)cls+=3;else if(v<=0x63)cls+=4;else if(v==0x64)cls+=5;}const int load=carriedFD84(o,ch);for(int i=0;i<5;++i)if(load<=o.table7E8C(cls-2,i))return i;return 4;}
}
