#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct ObjectFlagProbe6C68State { std::int16_t x255D2=0,y255D6=0; std::int32_t cf6C=0; };
struct ObjectFlagProbe6C68Ops { std::function<const std::uint8_t*(int,int)> lookup1E8C8; };
class ObjectFlagProbe6C68 { public: static void run(ObjectFlagProbe6C68State&,const ObjectFlagProbe6C68Ops&); };
}
