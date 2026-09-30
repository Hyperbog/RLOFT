#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct MessageDialog803CState { std::uint32_t c050=0; };
struct MessageDialog803COps { std::function<void()> flip1A9F8; std::function<void(std::uint32_t,int,int,int,int)> clear1C40C; std::function<void(int,int,int,int,int,int,int)> text13FD4; std::function<void(std::uint32_t)> call1A510; };
class MessageDialog803C { public: static void show(MessageDialog803CState&,const MessageDialog803COps&,int textResource); };
}
