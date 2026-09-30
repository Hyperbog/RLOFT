#pragma once
#include <cstdint>
#include <span>
namespace ravenloft::game {
class PackedSelector50DC {
public:
 static int select(std::span<const std::uint8_t> objects218E0,int mode,int object);
};
}
