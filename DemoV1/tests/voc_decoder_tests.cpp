#include "rloft/voc_decoder.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

void put16(std::vector<std::uint8_t>& out, std::size_t pos, std::uint16_t v) {
    out[pos] = static_cast<std::uint8_t>(v);
    out[pos + 1] = static_cast<std::uint8_t>(v >> 8U);
}

void put24(std::vector<std::uint8_t>& out, std::uint32_t v) {
    out.push_back(static_cast<std::uint8_t>(v));
    out.push_back(static_cast<std::uint8_t>(v >> 8U));
    out.push_back(static_cast<std::uint8_t>(v >> 16U));
}

std::vector<std::uint8_t> voc_header() {
    static constexpr char sig[] = "Creative Voice File";
    std::vector<std::uint8_t> out(26, 0);
    for (std::size_t i = 0; i < sizeof(sig) - 1U; ++i) {
        out[i] = static_cast<std::uint8_t>(sig[i]);
    }
    out[19] = 0x1A;
    put16(out, 20, 26);
    put16(out, 22, 0x0114);
    put16(out, 24, static_cast<std::uint16_t>(~0x0114U + 0x1234U));
    return out;
}

void test_type1_pcm8() {
    auto bytes = voc_header();
    bytes.push_back(1);
    put24(bytes, 6);
    bytes.push_back(156);  // 10,000 Hz
    bytes.push_back(0);    // unsigned 8-bit PCM
    bytes.push_back(0);
    bytes.push_back(128);
    bytes.push_back(255);
    bytes.push_back(64);
    bytes.push_back(0);

    const auto sound = rloft::decode_creative_voice(bytes);
    assert(sound.sample_rate == 10000);
    assert(sound.channels == 1);
    assert(sound.samples.size() == 4);
    assert(sound.samples[0] == -32768);
    assert(sound.samples[1] == 0);
    assert(sound.samples[2] == 32512);
    assert(sound.samples[3] == -16384);
}

void test_type9_pcm16_stereo() {
    auto bytes = voc_header();
    bytes.push_back(9);
    put24(bytes, 20);

    const std::uint32_t rate = 22050;
    bytes.push_back(static_cast<std::uint8_t>(rate));
    bytes.push_back(static_cast<std::uint8_t>(rate >> 8U));
    bytes.push_back(static_cast<std::uint8_t>(rate >> 16U));
    bytes.push_back(static_cast<std::uint8_t>(rate >> 24U));
    bytes.push_back(16);
    bytes.push_back(2);
    bytes.push_back(4);
    bytes.push_back(0);
    bytes.insert(bytes.end(), 4, 0);

    // Two stereo frames: (-32768, 32767), (0, -1).
    for (const std::uint16_t value :
         {std::uint16_t{0x8000}, std::uint16_t{0x7FFF},
          std::uint16_t{0x0000}, std::uint16_t{0xFFFF}}) {
        bytes.push_back(static_cast<std::uint8_t>(value));
        bytes.push_back(static_cast<std::uint8_t>(value >> 8U));
    }
    bytes.push_back(0);

    const auto sound = rloft::decode_creative_voice(bytes);
    assert(sound.sample_rate == 22050);
    assert(sound.channels == 2);
    assert(sound.samples.size() == 4);
    assert(sound.samples[0] == -32768);
    assert(sound.samples[1] == 32767);
    assert(sound.samples[2] == 0);
    assert(sound.samples[3] == -1);
}

}  // namespace

int main() {
    test_type1_pcm8();
    test_type9_pcm16_stereo();
    std::cout << "voc_decoder_tests: PASS\n";
}
