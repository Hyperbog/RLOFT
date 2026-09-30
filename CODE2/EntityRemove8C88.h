#pragma once
#include <cstdint>
#include <cstring>
#include <functional>
#include <span>
namespace ravenloft::game {
struct EntityRemove8C88State { std::span<std::uint8_t> entities; std::int32_t count=0; };
struct EntityRemove8C88Ops { std::function<void(int)> message2F580; };
class EntityRemove8C88 { public: static void run(EntityRemove8C88State&,const EntityRemove8C88Ops&,int index); };
}
