#pragma once
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>
#include <vector>

namespace ravenloft::data {
class ResourceArchive {
public:
    explicit ResourceArchive(const std::filesystem::path& path);
    std::size_t count() const { return offsets_.size(); }
    std::span<const std::uint8_t> record(std::size_t index) const;
    const std::vector<std::uint8_t>& bytes() const { return bytes_; }
private:
    std::vector<std::uint8_t> bytes_;
    std::vector<std::uint32_t> offsets_;
};
}
