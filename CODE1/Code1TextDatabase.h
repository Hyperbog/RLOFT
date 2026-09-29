#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace ravenloft::game {

class Code1TextDatabase {
public:
    static Code1TextDatabase decode(
        std::span<const std::uint8_t> encodedTextFile);

    std::size_t count() const noexcept { return entries_.size(); }
    const std::string& at(std::size_t index) const { return entries_.at(index); }

private:
    std::vector<std::string> entries_;
};

} // namespace ravenloft::game
