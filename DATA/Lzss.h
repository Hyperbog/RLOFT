#pragma once
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>
namespace ravenloft::data { std::vector<std::uint8_t> lzssDecode(std::span<const std::uint8_t> data,std::size_t expected,std::uint8_t fill=254); }
