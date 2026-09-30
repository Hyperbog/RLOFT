#pragma once
#include <cstdint>
#include <functional>
#include <string>
namespace ravenloft::game {
struct SharedLoad7938State { std::uint8_t d681=0,d682=0; std::string currentPath; std::string suffix398,suffix39C; };
struct SharedLoad7938Ops { std::function<int(const char*)> open312A0; std::function<int(int)> sharedLoad6CAD; };
class SharedLoad7938 { public: static int run(SharedLoad7938State&,const SharedLoad7938Ops&); };
}
