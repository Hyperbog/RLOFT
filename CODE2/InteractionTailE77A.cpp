#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct InteractionTailE77AOps{
 std::function<int(int,int,int,int)> dice;std::function<int(int,int)> value22E40;std::function<void(std::int32_t,int,int,int)> event28110;
 std::function<std::int32_t()> resource258D4;std::function<int(int,int,int)> gateC910;std::function<void(int,int,int,int,int)> effectE284;
 std::function<std::uint8_t(int,int)> mapByte;std::function<std::int8_t(int)> table67EC;std::function<void(int)> dec67EC;
 std::function<void(int,int,int,int)> timing24B48;std::function<std::int32_t()> clock19CC4;std::function<std::uint32_t(int)> charFlags;
 std::function<void(int,int,int)> storeTimed;
};
int interactionTailE77A(const InteractionTailE77AOps&o,int ch,int item,int target,int tier,int hit,const std::uint8_t*meta){
 int amount=0,msg=-1;
 if(item==-1){amount=o.dice(1,2,0,0);}
 else if(item>=0x7e&&item<=0x86){switch(item){case 0x7e:amount=o.dice(1,8,0,0);break;case 0x7f:amount=o.dice(2,8,1,0);msg=9;break;case 0x80:amount=o.dice(3,8,3,0);msg=9;break;case 0x81:amount=o.dice(1,4,0,0);break;case 0x82:amount=o.dice(1,4,4,2);break;case 0x83:amount=o.dice(2,6,0,0);msg=12;break;case 0x84:amount=o.dice(1,4,0,0);break;case 0x85:amount=o.dice(1,10,0,0);break;case 0x86:amount=o.dice(1,6,(o.value22E40(ch,4)+1)/2,0);break;}}
 else amount=o.dice(meta[0x38],meta[0x39],meta[0x3a]+tier,0);
 if(msg!=-1)o.event28110(o.resource258D4()+msg,0xff,0,0);
 if(item!=0x4a)amount=o.gateC910(amount,hit,item);
 o.effectE284(target,reinterpret_cast<intptr_t>(meta),amount,0,0);
 int delay=(item==-1||item>=0x7e&&item<=0x86)?18:((o.mapByte(item,0x35)>>12)&0xf)*9;
 if(o.charFlags(ch)&0x1000010u)delay=(delay-(delay>>31))/2;
 if(hit)o.storeTimed(ch,target,o.clock19CC4()+((delay-(delay>>31))/2));
 o.timing24B48(ch,target,0,delay);return hit;
}
}
