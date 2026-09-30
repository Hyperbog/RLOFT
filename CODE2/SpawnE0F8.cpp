#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct SpawnE0F8Ops{
 std::function<std::uint16_t()> seed30700;std::function<void(std::uint16_t)> seed290D0;std::function<std::uint32_t()> random30CA2;
 std::function<std::int16_t()> width,height,partyX,partyY,activeCount;std::function<std::uint16_t(int)> cell;
 std::function<std::uint8_t(int)> entityFlags;std::function<std::uint16_t(int)> entityCell,entityType;
 std::function<void(int,std::int16_t,std::int16_t,std::uint16_t)> initEntity;
};
int spawnE0F8(const SpawnE0F8Ops&o,int index){
 o.seed290D0(o.seed30700());
 int x=0,y=0,tries=0;
 for(;tries<3;++tries){x=static_cast<int>(((o.random30CA2()*(o.width()-2))>>15)+1);y=static_cast<int>(((o.random30CA2()*(o.height()-2))>>15)+1);const int cell=y*o.width()+x;
  if(o.cell(cell)&0x7fu)continue;if(std::abs(x-o.partyX())<15||std::abs(y-o.partyY())<15)continue;
  int i=0;for(;i<o.activeCount();++i)if((o.entityFlags(i)&2u)==0&&o.entityCell(i)==cell)break;if(i<o.activeCount())continue;break;
 }
 if(tries==3)return 0;o.initEntity(index,static_cast<std::int16_t>(x),static_cast<std::int16_t>(y),o.entityType(index));return 1;
}
}
