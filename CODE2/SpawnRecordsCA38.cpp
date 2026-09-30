#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct SpawnRecordsCA38Ops{
 std::function<std::int16_t(int,int)> characterWord;
 std::function<std::int16_t()> activeCount;
 std::function<void(std::uint8_t*,const std::uint8_t*,int)> copy30CD4;
 std::function<std::int16_t()> mapY;
 std::function<std::int16_t()> mapX;
 std::function<std::uint16_t()> mapWidth;
 std::function<std::int32_t()> worldY;
 std::function<std::int32_t()> worldX;
 std::function<std::uint8_t*()> nextRecord;
 std::function<void()> incrementActiveCount;
};
void spawnRecordsCA38(const SpawnRecordsCA38Ops&o,int ch){
 for(int off=0;off<0x2a;off+=2){
  const auto id=o.characterWord(ch,0x120+off);
  if(id==-1)continue;
  const int n=o.activeCount();
  auto*r=o.nextRecord();
  o.copy30CD4(r,nullptr,n*0x16);
  const std::uint16_t cell=static_cast<std::uint16_t>(o.mapY()*o.mapWidth()+o.mapX());
  *reinterpret_cast<std::uint16_t*>(r+0x0e)=static_cast<std::uint16_t>(id);
  *reinterpret_cast<std::int32_t*>(r)=o.worldY();
  *reinterpret_cast<std::int32_t*>(r+4)=o.worldX();
  *reinterpret_cast<std::uint16_t*>(r+0x14)&=0xfe07u;
  *reinterpret_cast<std::uint16_t*>(r+0x0c)=cell;
  if(n<0x64)o.incrementActiveCount();
 }
}
}
