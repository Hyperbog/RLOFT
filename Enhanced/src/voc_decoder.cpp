#include "rloft/voc_decoder.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <vector>

namespace rloft {
namespace {

std::uint16_t u16(const std::uint8_t* p) {
    return static_cast<std::uint16_t>(p[0]) |
           (static_cast<std::uint16_t>(p[1]) << 8U);
}

std::uint32_t u24(const std::uint8_t* p) {
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1]) << 8U) |
           (static_cast<std::uint32_t>(p[2]) << 16U);
}

std::uint32_t u32(const std::uint8_t* p) {
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1]) << 8U) |
           (static_cast<std::uint32_t>(p[2]) << 16U) |
           (static_cast<std::uint32_t>(p[3]) << 24U);
}

void set_or_check_format(
    PcmSound& out,
    std::uint32_t rate,
    std::uint16_t channels) {
    if (rate == 0 || channels == 0 || channels > 2) {
        throw std::runtime_error("VOC has invalid PCM format");
    }

    if (out.sample_rate == 0) {
        out.sample_rate = rate;
        out.channels = channels;
        return;
    }

    if (out.sample_rate != rate || out.channels != channels) {
        throw std::runtime_error(
            "VOC changes sample format inside one sound");
    }
}

void append_pcm8(
    PcmSound& out,
    const std::uint8_t* data,
    std::size_t bytes,
    std::uint32_t rate,
    std::uint16_t channels) {
    set_or_check_format(out, rate, channels);
    out.samples.reserve(out.samples.size() + bytes);
    for (std::size_t i = 0; i < bytes; ++i) {
        const int centered = static_cast<int>(data[i]) - 128;
        out.samples.push_back(
            static_cast<std::int16_t>(centered << 8));
    }
}

void append_pcm16(
    PcmSound& out,
    const std::uint8_t* data,
    std::size_t bytes,
    std::uint32_t rate,
    std::uint16_t channels) {
    if ((bytes & 1U) != 0) {
        throw std::runtime_error("VOC 16-bit PCM payload has odd byte count");
    }
    set_or_check_format(out, rate, channels);
    out.samples.reserve(out.samples.size() + bytes / 2U);
    for (std::size_t i = 0; i < bytes; i += 2U) {
        out.samples.push_back(static_cast<std::int16_t>(
            static_cast<std::uint16_t>(data[i]) |
            (static_cast<std::uint16_t>(data[i + 1U]) << 8U)));
    }
}

}  // namespace

PcmSound decode_creative_voice(
    const std::vector<std::uint8_t>& voc) {
    static constexpr char kSignature[] = "Creative Voice File";
    if (voc.size() < 26 ||
        std::memcmp(voc.data(), kSignature, sizeof(kSignature) - 1U) != 0 ||
        voc[19] != 0x1A) {
        throw std::runtime_error("RES type-4 payload is not a Creative Voice File");
    }

    std::size_t pos = u16(voc.data() + 20);
    if (pos < 26 || pos > voc.size()) {
        throw std::runtime_error("VOC data offset is invalid");
    }

    PcmSound out;
    std::uint32_t current_rate = 0;
    std::uint16_t current_channels = 1;
    std::uint16_t current_codec = 0;

    bool have_extended = false;
    std::uint32_t extended_rate = 0;
    std::uint16_t extended_channels = 1;
    std::uint16_t extended_codec = 0;

    struct RepeatState {
        bool active{};
        std::uint16_t count{};
        std::size_t sample_start{};
    } repeat;

    while (pos < voc.size()) {
        const auto type = voc[pos++];
        if (type == 0) break;

        if (pos + 3 > voc.size()) {
            throw std::runtime_error("VOC block length is truncated");
        }
        const auto size = static_cast<std::size_t>(u24(voc.data() + pos));
        pos += 3;
        if (pos + size > voc.size()) {
            throw std::runtime_error("VOC block exceeds file size");
        }

        const auto* block = voc.data() + pos;

        switch (type) {
            case 1: {
                if (size < 2) {
                    throw std::runtime_error("VOC type-1 block is too small");
                }

                const auto time_constant = block[0];
                const auto block_codec = block[1];
                if (have_extended) {
                    current_rate = extended_rate;
                    current_channels = extended_channels;
                    current_codec = extended_codec;
                    have_extended = false;
                } else {
                    if (time_constant == 0x100) {
                        throw std::runtime_error("VOC invalid time constant");
                    }
                    current_rate =
                        1000000U / (256U - static_cast<unsigned>(time_constant));
                    current_channels = 1;
                    current_codec = block_codec;
                }

                if (current_codec != 0) {
                    throw std::runtime_error(
                        "VOC uses Creative ADPCM compression not yet observed in the certified active path");
                }

                append_pcm8(
                    out,
                    block + 2,
                    size - 2,
                    current_rate,
                    current_channels);
                break;
            }

            case 2:
                if (current_rate == 0) {
                    throw std::runtime_error(
                        "VOC continuation appears before sound-data format");
                }
                if (current_codec != 0) {
                    throw std::runtime_error(
                        "VOC continuation uses unsupported compressed codec");
                }
                append_pcm8(
                    out,
                    block,
                    size,
                    current_rate,
                    current_channels);
                break;

            case 3: {
                if (size != 3) {
                    throw std::runtime_error("VOC silence block size is invalid");
                }
                const auto duration = static_cast<std::uint32_t>(u16(block)) + 1U;
                const auto tc = block[2];
                const auto rate =
                    1000000U / (256U - static_cast<unsigned>(tc));
                set_or_check_format(out, rate, 1);
                out.samples.insert(
                    out.samples.end(),
                    duration,
                    static_cast<std::int16_t>(0));
                current_rate = rate;
                current_channels = 1;
                current_codec = 0;
                break;
            }

            case 4:
            case 5:
                // Marker and ASCII blocks carry no sample data.
                break;

            case 6:
                if (size != 2) {
                    throw std::runtime_error("VOC repeat-start size is invalid");
                }
                if (repeat.active) {
                    throw std::runtime_error("Nested VOC repeat blocks are unsupported");
                }
                repeat.active = true;
                repeat.count = u16(block);
                repeat.sample_start = out.samples.size();
                break;

            case 7:
                if (!repeat.active) {
                    throw std::runtime_error("VOC repeat-end without repeat-start");
                }
                if (repeat.count == 0xFFFFU) {
                    throw std::runtime_error("Infinite VOC repeat is not valid for one-shot SFX");
                } else {
                    const std::vector<std::int16_t> segment(
                        out.samples.begin() +
                            static_cast<std::ptrdiff_t>(repeat.sample_start),
                        out.samples.end());
                    // Creative VOC stores the number of additional repeats.
                    for (std::uint16_t i = 0; i < repeat.count; ++i) {
                        out.samples.insert(
                            out.samples.end(),
                            segment.begin(),
                            segment.end());
                    }
                }
                repeat = {};
                break;

            case 8: {
                if (size != 4) {
                    throw std::runtime_error("VOC extended block size is invalid");
                }
                const auto tc = u16(block);
                const auto codec = block[2];
                const auto mode = block[3];
                const auto channels =
                    static_cast<std::uint16_t>(mode == 0 ? 1 : 2);
                const auto denominator = 65536U - static_cast<unsigned>(tc);
                if (denominator == 0) {
                    throw std::runtime_error("VOC extended time constant is invalid");
                }
                extended_rate =
                    (256000000U / denominator) / channels;
                extended_channels = channels;
                extended_codec = codec;
                have_extended = true;
                break;
            }

            case 9: {
                if (size < 12) {
                    throw std::runtime_error("VOC new-sound block is too small");
                }
                const auto rate = u32(block);
                const auto bits = block[4];
                const auto channels = static_cast<std::uint16_t>(block[5]);
                const auto codec = u16(block + 6);
                const auto* pcm = block + 12;
                const auto pcm_bytes = size - 12;

                if (codec == 0 && bits == 8) {
                    append_pcm8(out, pcm, pcm_bytes, rate, channels);
                } else if (codec == 4 && bits == 16) {
                    append_pcm16(out, pcm, pcm_bytes, rate, channels);
                } else {
                    throw std::runtime_error(
                        "VOC new-sound block uses unsupported codec");
                }

                current_rate = rate;
                current_channels = channels;
                current_codec = codec;
                break;
            }

            default:
                // Unknown metadata blocks are length-delimited by the VOC
                // container; ignore them without losing stream alignment.
                break;
        }

        pos += size;
    }

    if (out.sample_rate == 0 || out.samples.empty()) {
        throw std::runtime_error("VOC contains no decodable PCM sound data");
    }
    if (repeat.active) {
        throw std::runtime_error("VOC repeat block is unterminated");
    }

    return out;
}

}  // namespace rloft
