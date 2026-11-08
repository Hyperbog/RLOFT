#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace rloft {

// Original CODE.2 1C368: seek to 4+index*4, load the record offset,
// read its 16-bit byte count, then XOR every payload byte with 0xDC.
// Input comes from the unchanged original game file "T".
class OriginalTTextArchive final {
public:
    explicit OriginalTTextArchive(std::vector<std::uint8_t> bytes)
        : bytes_(std::move(bytes)) {
        if(bytes_.size()<8 || count()==0 ||
           (static_cast<std::uint64_t>(count())+1U)*4U>bytes_.size())
            throw std::invalid_argument("invalid original T index table");
    }
    [[nodiscard]] std::uint32_t count() const {return read32(0);}
    [[nodiscard]] std::string decoded(std::uint32_t index) const {
        if(index>=count())throw std::out_of_range("original T index");
        const auto start=read32(4U+4U*static_cast<std::size_t>(index));
        const auto length=read16(start);
        const auto p=static_cast<std::size_t>(start)+2U;
        if(p+length>bytes_.size())throw std::out_of_range("truncated original T record");
        std::string value;
        value.reserve(length);
        for(std::size_t i=0;i<length;++i)
            value.push_back(static_cast<char>(bytes_[p+i]^0xDCU));
        return value;
    }
private:
    [[nodiscard]] std::uint16_t read16(std::size_t p) const {
        if(p+2U>bytes_.size())throw std::out_of_range("T read u16");
        return static_cast<std::uint16_t>(bytes_[p] | bytes_[p+1U]<<8U);
    }
    [[nodiscard]] std::uint32_t read32(std::size_t p) const {
        if(p+4U>bytes_.size())throw std::out_of_range("T read u32");
        return static_cast<std::uint32_t>(bytes_[p]) |
               (static_cast<std::uint32_t>(bytes_[p+1U])<<8U) |
               (static_cast<std::uint32_t>(bytes_[p+2U])<<16U) |
               (static_cast<std::uint32_t>(bytes_[p+3U])<<24U);
    }
    std::vector<std::uint8_t> bytes_;
};
} // namespace rloft
