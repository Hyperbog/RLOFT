#include "ConfigIo5DD0.h"
namespace ravenloft::game {
void ConfigIo5DD0::save(ConfigIo5DD0State&s,const ConfigIo5DD0Ops&o){
 o.rewind31380(s.handle6138);
 const int pos=o.tell312C8(s.handle6138);o.close312DC(pos);
 const int h=o.create312B4();
 if(h==-1){o.error2F5F4(s.resource60DC,0x36);return;}
 std::uint8_t bytes[14]{};bytes[6]=s.c02b;bytes[7]=s.c02c;bytes[9]=s.c02e;
 o.write31304(bytes,14,h);o.close312DC(h);
}
void ConfigIo5DD0::load(ConfigIo5DD0State&s,const ConfigIo5DD0Ops&o){
 const int h=o.create312B4();if(h==-1)return;
 std::uint8_t bytes[14]{};o.read312EC(bytes,14,h);o.close312DC(h);
 s.c02b=bytes[6];s.c02c=bytes[7];s.c02e=bytes[9];
 o.blit325B0(0xC054,0,s.c034,0x390,0x100);o.restore29234(0xC054);o.call28098(s.c02b);o.call290A8(s.c02c);
}
}
