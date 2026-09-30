#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct InteractionE5F0Ops{
 std::function<int(int)> chooseDB94;std::function<void(int)> statusE578;std::function<std::uint8_t()> global258A8;
 std::function<std::uint16_t(int)> mapFlags35,entityType;std::function<const std::uint8_t*(int)> metadata;
 std::function<int(int,const std::uint8_t*,int,int,int)> checkC80C;std::function<std::uint8_t(int,int)> charByte;
 std::function<int(int,int)> probe23EAC;
};
int interactionE5F0(const InteractionE5F0Ops&o,int ch,int flags,int mapIndex,int selected){
 if(selected==-1)selected=o.chooseDB94(flags);o.statusE578(ch);
 if(o.global258A8()==0xff&&selected!=-1&&(flags&2)){if(mapIndex==-1||((o.mapFlags35(mapIndex)&3u)!=2u))selected=-1;}
 if(selected==-1)return -1;const auto*meta=o.metadata(selected);if(!o.checkC80C(ch,meta,mapIndex,1,selected))return selected;
 const int cls=o.charByte(ch,0x113),v=o.charByte(ch,0x11f);int tier;
 bool ordinary=cls!=0x12;if(!ordinary){ordinary=true;for(int k:{1,5,0,2})if(o.probe23EAC(ch,k)){ordinary=false;break;}}
 if(ordinary)tier=static_cast<std::int8_t>(meta[0]);else tier=v<=0x4b?3:v<=0x5a?4:v<=0x63?5:6;
 (void)tier;return selected;
}
}
