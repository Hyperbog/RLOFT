#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct EntityEffectE284Ops{
 std::function<int(std::uint16_t)> valid2ACDC;std::function<int(int)> probe1C1DC,countE0BC;std::function<int(int,int,int,int)> dice;
 std::function<void(int)> event1C124,mapClearE028,dispatch02A8,delay28DCC;std::function<void()> refresh1F1C8,refresh1EA78,wait28EF0;
 std::function<void(int)> mode16524;std::function<void(int,int,int,int,int,int,int)> create2195C;
 std::function<void(std::int32_t,int,int,int)> event28110;std::function<std::int32_t()> resource258BC;std::function<void(int)> progressDDA4;
 std::function<int(int)> spawnE0F8;
};
struct EntityE284{std::int32_t y,x;std::uint8_t pad8[3];std::uint8_t flagsB;std::uint16_t cell,type,state,value,flags14;};
void effectE284(const EntityEffectE284Ops&o,EntityE284&e,const std::uint8_t*meta,int amount,int mode){
 if(meta[0x1c])e.flags14|=6;e.state=mode?1:2;
 if(e.type!=0x0f){if(e.type==8&&((e.flags14&0xc000u)!=0xc000u)&&e.value>10){e.value=static_cast<std::uint16_t>(e.value-amount);if(static_cast<std::int16_t>(e.value)<=0)e.value=10;}else if(e.type!=8)e.value=static_cast<std::uint16_t>(e.value-amount);}
 if(o.valid2ACDC(e.type)){e.flagsB&=0xcf;e.flagsB|=0x10;}if(static_cast<std::int16_t>(e.value)>0)return;
 switch(e.type){case 0x21:o.mapClearE028(reinterpret_cast<intptr_t>(&e));break;case 0x12:o.event1C124(0x20);break;case 0x50:o.event1C124(0x43);break;case 8:o.event1C124(0x51);o.create2195C(0x80,0x74,0,e.x,e.y,1,0);o.dispatch02A8(0x1f);o.dispatch02A8(0x20);o.dispatch02A8(0x21);o.delay28DCC(0x9c4);o.wait28EF0();break;default:break;}
 const std::uint16_t msg=*reinterpret_cast<const std::uint16_t*>(meta+0x43);o.event28110(o.resource258BC()+msg,0xff,0,1);
 o.progressDDA4(*reinterpret_cast<const std::uint16_t*>(meta+0x23));
}
}
