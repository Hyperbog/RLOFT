#include "PackedModeDrivers5C84.h"
namespace ravenloft::game {
namespace{std::int16_t sw(std::span<std::uint8_t>b,std::size_t p){return std::int16_t(std::uint16_t(b[p])|(std::uint16_t(b[p+1])<<8));}}
int PackedModeDrivers5C84::mode0(PackedModeDrivers5C84State&s,const PackedModeDrivers5C84Ops&o){
 int chosen=0,object=0,sel=-1;for(;object<4;++object)if((sel=o.select50DC(0,object))>=0)break;if(sel<0||object==4)return 0;
 do{auto b=std::size_t(object)*0x248;sel=o.select50DC(0,object);int level=sw(s.objects,b+0x20+sel*2)-1;auto table=std::span<const std::uint8_t>(s.objects.data()+((std::int8_t)s.objects[b+0x15]==1?0x6350:0x62B0),8);o.build5BDC(0,object,level,table);chosen=o.controller5AA4(0,object,s.resource258DC+0x30,10,s.limits6208.data(),0x92,0);object=chosen;}while(chosen>=0);return 1;
}
int PackedModeDrivers5C84::mode1(PackedModeDrivers5C84State&s,const PackedModeDrivers5C84Ops&o){
 int chosen=0,object=0,sel=-1;for(;object<4;++object)if((sel=o.select50DC(1,object))>=0)break;if(sel<0||object==4)return 0;
 do{auto b=std::size_t(object)*0x248;sel=o.select50DC(1,object);int level=sw(s.objects,b+0x20+sel*2)-1;auto table=std::span<const std::uint8_t>(s.objects.data()+0x6210,8);o.build5BDC(1,object,level,table);if(o.callF2D4(object,0x48))s.flag620A=std::uint8_t(s.flag620A<<1);chosen=o.controller5AA4(1,object,s.resource258DC+0x32,11,s.limits6208.data(),0x42,0);object=chosen;}while(chosen>=0);return 1;
}}
