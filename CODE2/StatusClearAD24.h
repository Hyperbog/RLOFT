#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct StatusClearAD24State { std::uint8_t* characters=nullptr; };
struct StatusClearAD24Ops { std::function<void(int,int)> expire871C,probe86C4; std::function<void()> sweep893C; };
class StatusClearAD24 { public: static void run(StatusClearAD24State&,const StatusClearAD24Ops&,int character); };
}
