#pragma once
#include <functional>
namespace ravenloft::game {
struct RestoreFlow7874State { char path6690[4]={'0','0',0,0}; };
struct RestoreFlow7874Ops { std::function<int(const char*)> exists33A5A; std::function<void(int)> call1C21C,call2F580; std::function<void()> disk7378; std::function<int(const char*,const char*)> copy73A0; };
class RestoreFlow7874 { public: static void run(RestoreFlow7874State&,const RestoreFlow7874Ops&,const char*currentPath,const char*auxPath,int msgMissing,int msgAux,int msgCopy); };
}
