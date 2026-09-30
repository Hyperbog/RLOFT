#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct PageDispatchOps{std::function<std::uint8_t&()> page;std::function<std::int32_t&()> globalBF44;std::function<void(int,int,int)> call1A5B0;std::function<void(int,int,int,int,int,int,int)> call1A724;};
void dispatch100C8(const PageDispatchOps&o,int id,std::uint8_t mode){if(static_cast<unsigned>(id-0xa7)<=0x22u)o.globalBF44()=0x3a;o.call1A5B0(o.page(),static_cast<std::int16_t>(id),mode);o.page()^=1;}
void dispatch10108(const PageDispatchOps&o,int id,std::uint8_t mode,int a,int b,int c,int d){if(static_cast<unsigned>(id-0xa7)<=0x22u)o.globalBF44()=0x3a;o.call1A724(o.page(),static_cast<std::int16_t>(id),mode,a,b,c,d);o.page()^=1;}
}
