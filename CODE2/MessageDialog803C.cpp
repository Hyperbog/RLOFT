#include "MessageDialog803C.h"
namespace ravenloft::game {
void MessageDialog803C::show(MessageDialog803CState&s,const MessageDialog803COps&o,int text){
 o.flip1A9F8();o.clear1C40C(s.c050,0x0f,0x40,0x32,0xc0);o.text13FD4(0,0x14,0x46,0x12,0xb4,text,1);o.call1A510(s.c050);s.c050=(s.c050&~0xffu)|((s.c050^1u)&0xffu);
}}
