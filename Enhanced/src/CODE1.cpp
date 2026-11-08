#include "rloft/CODE1.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kModule = 0x25D24U;
constexpr std::size_t kCode1Size = 0x43D1BU;
constexpr std::size_t kDataStart = 0x3E724U;
constexpr std::size_t kFirst = 0x06D8U;
constexpr std::size_t kSecond = 0x06FCU;

std::uint32_t read32(const std::vector<std::uint8_t>& b, std::size_t at) {
    if (at > b.size() || b.size() - at < 4U) {
        throw std::runtime_error("CODE.1 LE field exceeds file");
    }
    return static_cast<std::uint32_t>(b[at]) |
           (static_cast<std::uint32_t>(b[at + 1U]) << 8U) |
           (static_cast<std::uint32_t>(b[at + 2U]) << 16U) |
           (static_cast<std::uint32_t>(b[at + 3U]) << 24U);
}

void verify_code1_layout(const std::vector<std::uint8_t>& b) {
    if (b.size() != kCode1Size || b[0] != 'M' || b[1] != 'Z' ||
        b[kModule] != 'M' || b[kModule + 1U] != 'Z') {
        throw std::runtime_error("CODE.1 bound executable identity mismatch");
    }
    const auto le = kModule + static_cast<std::size_t>(
        read32(b, kModule + 0x3CU));
    if (le > b.size() || b.size() - le < 0x84U) {
        throw std::runtime_error("CODE.1 nested LE header exceeds file");
    }
    if (le != 0x27BDCU || b[le] != 'L' || b[le + 1U] != 'E' ||
        read32(b, le + 0x14U) != 0x18U ||
        read32(b, le + 0x18U) != 1U ||
        read32(b, le + 0x1CU) != 0xCC1CU ||
        read32(b, le + 0x28U) != 0x1000U ||
        read32(b, le + 0x44U) != 2U ||
        read32(b, le + 0x80U) != 0x6A00U) {
        throw std::runtime_error("CODE.1 LE header differs from certified build");
    }
    const auto object_table = le + static_cast<std::size_t>(
        read32(b, le + 0x40U));
    if (read32(b, object_table) != 0x11302U ||
        read32(b, object_table + 12U) != 1U ||
        read32(b, object_table + 16U) != 18U ||
        read32(b, object_table + 24U) != 0x9E90U ||
        read32(b, object_table + 36U) != 19U ||
        read32(b, object_table + 40U) != 6U) {
        throw std::runtime_error("CODE.1 LE object map differs from certified build");
    }
    const auto data_page = kModule + static_cast<std::size_t>(
        read32(b, le + 0x80U));
    const auto data = data_page + 18U * 0x1000U;
    if (data != kDataStart ||
        b.size() - data < kSecond + 216U) {
        throw std::runtime_error("CODE.1 data-object location is invalid");
    }
}
}  // namespace

bool Code1ClassRules::allows_first(int race, int candidate) const noexcept {
    return race >= 0 && race < 6 && candidate >= 0 && candidate < 6 &&
           first[static_cast<std::size_t>(race) * 6U +
                 static_cast<std::size_t>(candidate)] != 0U;
}

bool Code1ClassRules::allows_second(int race, int primary,
                                    int candidate) const noexcept {
    return race >= 0 && race < 6 && primary >= 0 && primary < 6 &&
           candidate >= 0 && candidate < 6 &&
           second[(static_cast<std::size_t>(race) * 6U +
                   static_cast<std::size_t>(primary)) * 6U +
                  static_cast<std::size_t>(candidate)] != 0U;
}

Code1ClassRules Code1ClassRules::vanilla() noexcept {
    // Original CODE.1 DGROUP bytes, also matched against the uploaded file
    // in CODE1_tests. Keep one canonical default, not another gameplay table.
    constexpr std::uint8_t kFirst[6][6] = {
        {1,1,1,1,1,1}, {1,0,1,1,1,1}, {1,0,0,1,0,1},
        {1,0,0,1,0,1}, {1,0,0,1,0,1}, {1,0,1,1,1,1}
    };
    constexpr std::uint8_t kSecond[6][6][6] = {
        {{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},
         {0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}},
        {{0,0,0,1,1,1},{0,0,0,0,0,0},{0,0,0,1,0,0},
         {1,0,1,0,1,0},{1,0,0,1,0,1},{1,0,0,0,1,0}},
        {{0,0,0,0,0,1},{0,0,0,0,0,0},{0,0,0,0,0,0},
         {0,0,0,0,0,0},{0,0,0,0,0,0},{1,0,0,0,0,0}},
        {{0,0,0,1,0,1},{0,0,0,0,0,0},{0,0,0,0,0,0},
         {1,0,0,0,0,1},{0,0,0,0,0,0},{1,0,0,1,0,0}},
        {{0,0,0,1,0,1},{0,0,0,0,0,0},{0,0,0,0,0,0},
         {1,0,0,0,0,0},{0,0,0,0,0,0},{1,0,0,0,0,0}},
        {{0,0,0,0,1,1},{0,0,0,0,0,0},{0,0,0,0,0,0},
         {0,0,0,0,0,0},{1,0,0,0,0,1},{1,0,0,0,1,0}}
    };
    Code1ClassRules result{};
    for (std::size_t r = 0; r < 6U; ++r) {
        for (std::size_t c = 0; c < 6U; ++c) {
            result.first[r * 6U + c] = kFirst[r][c];
            for (std::size_t d = 0; d < 6U; ++d) {
                result.second[(r * 6U + c) * 6U + d] = kSecond[r][c][d];
            }
        }
    }
    return result;
}

Code1ClassRules Code1ClassRules::from_code1(
    const std::vector<std::uint8_t>& bytes) {
    verify_code1_layout(bytes);
    Code1ClassRules rules{};
    for (std::size_t i = 0; i < rules.first.size(); ++i) {
        rules.first[i] = bytes[kDataStart + kFirst + i];
        if (rules.first[i] > 1U) {
            throw std::runtime_error("CODE.1 invalid first-class availability byte");
        }
    }
    for (std::size_t i = 0; i < rules.second.size(); ++i) {
        rules.second[i] = bytes[kDataStart + kSecond + i];
        if (rules.second[i] > 1U) {
            throw std::runtime_error("CODE.1 invalid second-class availability byte");
        }
    }
    return rules;
}

} // namespace rloft
