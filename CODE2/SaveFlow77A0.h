#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct SaveFlow77A0State { std::int32_t c6c8=-1; std::uint8_t a79d=0; char path6690[4]={'0','0',0,0}; };
struct SaveFlow77A0Ops { std::function<void(int)> call171FC,call1C21C,call2F580,call803C; std::function<int(int)> save66E8; std::function<int(const char*)> exists33A5A; std::function<void(const char*)> call33A38; std::function<void()> disk7378; std::function<int(const char*,const char*)> copy73A0; };
class SaveFlow77A0 { public: static void run(SaveFlow77A0State&,const SaveFlow77A0Ops&,const char*currentPath,const char*auxPath,int msgA,int msgB,int msgC); };
}
