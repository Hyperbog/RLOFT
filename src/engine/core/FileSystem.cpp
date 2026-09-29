#include "engine/core/FileSystem.h"
#include <fstream>
#include <stdexcept>

namespace ssi::core {
std::vector<std::uint8_t> readFile(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) throw std::runtime_error("Cannot open file: " + path.string());
    const auto end = file.tellg();
    if (end < 0) throw std::runtime_error("Cannot size file: " + path.string());
    std::vector<std::uint8_t> bytes(static_cast<std::size_t>(end));
    file.seekg(0, std::ios::beg);
    if (!bytes.empty()) file.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
    if (!file && !bytes.empty()) throw std::runtime_error("Cannot read file: " + path.string());
    return bytes;
}
}
