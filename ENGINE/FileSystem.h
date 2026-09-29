#pragma once
#include <cstdint>
#include <filesystem>
#include <vector>

namespace ssi::core {
std::vector<std::uint8_t> readFile(const std::filesystem::path& path);
}
