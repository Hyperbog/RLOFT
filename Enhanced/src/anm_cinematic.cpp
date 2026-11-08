#include "rloft/anm_cinematic.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>

namespace rloft {
namespace {

std::uint16_t read_u16(
    const std::vector<std::uint8_t>& bytes, std::size_t& pos) {
    if (pos + 2 > bytes.size()) {
        throw std::runtime_error("ANM truncated uint16");
    }
    const auto value =
        static_cast<std::uint16_t>(bytes[pos]) |
        (static_cast<std::uint16_t>(bytes[pos + 1]) << 8U);
    pos += 2;
    return value;
}

std::int16_t payload_s16(
    const std::vector<std::uint8_t>& bytes, std::size_t offset) {
    if (offset + 2 > bytes.size()) {
        throw std::runtime_error("ANM command payload is truncated");
    }
    const auto value =
        static_cast<std::uint16_t>(bytes[offset]) |
        (static_cast<std::uint16_t>(bytes[offset + 1]) << 8U);
    return static_cast<std::int16_t>(value);
}

std::uint16_t payload_u16(
    const std::vector<std::uint8_t>& bytes, std::size_t& pos) {
    if (pos + 2 > bytes.size()) {
        throw std::runtime_error("ANM image payload is truncated");
    }
    const auto value =
        static_cast<std::uint16_t>(bytes[pos]) |
        (static_cast<std::uint16_t>(bytes[pos + 1]) << 8U);
    pos += 2;
    return value;
}

int text_width(const std::string& text, const Type2Font& font) {
    int width = 0;
    for (const unsigned char ch : text) {
        if (ch >= font.widths.size()) {
            throw std::runtime_error("ANM subtitle glyph exceeds font width table");
        }
        width += font.widths[ch];
    }
    return width;
}

}  // namespace

AnmCinematic::AnmCinematic(
    std::vector<std::uint8_t> bytes,
    int cinematic_number,
    const RavenloftBootAssets& assets)
    : assets_(assets), cinematic_number_(cinematic_number) {
    std::size_t pos = 0;
    const auto declared_frames = read_u16(bytes, pos);
    default_wait_ = read_u16(bytes, pos);

    frames_.reserve(declared_frames);
    for (std::uint16_t frame_index = 0;
         frame_index < declared_frames;
         ++frame_index) {
        Frame frame;
        const auto chunk_count = read_u16(bytes, pos);
        frame.chunks.reserve(chunk_count);

        for (std::uint16_t chunk_index = 0;
             chunk_index < chunk_count;
             ++chunk_index) {
            Chunk chunk;
            chunk.id = read_u16(bytes, pos);
            if (chunk.id > 9) {
                throw std::runtime_error("ANM chunk id exceeds executable dispatcher");
            }
            if (pos + 4 > bytes.size()) {
                throw std::runtime_error("ANM truncated chunk size");
            }
            const std::uint32_t raw_size =
                static_cast<std::uint32_t>(bytes[pos]) |
                (static_cast<std::uint32_t>(bytes[pos + 1]) << 8U) |
                (static_cast<std::uint32_t>(bytes[pos + 2]) << 16U) |
                (static_cast<std::uint32_t>(bytes[pos + 3]) << 24U);
            pos += 4;
            if ((raw_size & 0x80000000U) != 0U) {
                throw std::runtime_error("ANM contains negative chunk size");
            }
            const auto size = static_cast<std::size_t>(raw_size);
            if (pos + size > bytes.size()) {
                throw std::runtime_error("ANM chunk payload exceeds file");
            }
            chunk.payload.assign(
                bytes.begin() + static_cast<std::ptrdiff_t>(pos),
                bytes.begin() + static_cast<std::ptrdiff_t>(pos + size));
            pos += size;
            frame.chunks.push_back(std::move(chunk));
        }
        frames_.push_back(std::move(frame));
    }

    if (pos != bytes.size()) {
        throw std::runtime_error("ANM has trailing bytes after declared stream");
    }
}

void AnmCinematic::reset() {
    next_frame_ = 0;
    framebuffer_.clear(0);
    palette_ = {};
    subtitle_active_ = false;
    subtitle_text_.clear();
    subtitle_font_.reset();
}

bool AnmCinematic::finished() const noexcept {
    return next_frame_ >= frames_.size();
}

int AnmCinematic::frame_count() const noexcept {
    return static_cast<int>(frames_.size());
}

int AnmCinematic::default_wait() const noexcept {
    return default_wait_;
}

void AnmCinematic::apply_type2(
    const std::vector<std::uint8_t>& payload) {
    auto& out = framebuffer_.mutable_pixels();
    std::size_t pos = 0;

    for (int y = 0; y < IndexedFramebuffer::kHeight; ++y) {
        int x = 0;
        const auto row =
            static_cast<std::size_t>(y) * IndexedFramebuffer::kWidth;

        while (x < IndexedFramebuffer::kWidth) {
            if (pos >= payload.size()) {
                throw std::runtime_error("ANM type-2 ended inside scanline");
            }
            const auto op = payload[pos++];
            if (op < 0x80U) {
                const int count = static_cast<int>(op) + 1;
                if (x + count > IndexedFramebuffer::kWidth ||
                    pos + static_cast<std::size_t>(count) > payload.size()) {
                    throw std::runtime_error("ANM type-2 invalid literal run");
                }
                std::copy_n(
                    payload.begin() + static_cast<std::ptrdiff_t>(pos),
                    count,
                    out.begin() + static_cast<std::ptrdiff_t>(row + x));
                pos += static_cast<std::size_t>(count);
                x += count;
            } else {
                const int count = 0x101 - static_cast<int>(op);
                if (x + count > IndexedFramebuffer::kWidth ||
                    pos >= payload.size()) {
                    throw std::runtime_error("ANM type-2 invalid repeat run");
                }
                const auto value = payload[pos++];
                std::fill_n(
                    out.begin() + static_cast<std::ptrdiff_t>(row + x),
                    count,
                    value);
                x += count;
            }
        }
    }

    if (pos != payload.size()) {
        throw std::runtime_error("ANM type-2 leaves unconsumed payload");
    }
}

void AnmCinematic::apply_type3(
    const std::vector<std::uint8_t>& payload) {
    auto& pixels = framebuffer_.mutable_pixels();
    std::size_t pos = 0;
    const auto outer_count = payload_u16(payload, pos);
    int x = 0;

    for (std::uint16_t outer = 0; outer < outer_count; ++outer) {
        const auto delta_x = payload_u16(payload, pos);
        const auto group_count = payload_u16(payload, pos);
        x += delta_x;
        if (x < 0 || x >= IndexedFramebuffer::kWidth) {
            throw std::runtime_error("ANM type-3 x exceeds framebuffer");
        }

        int y = 0;
        for (std::uint16_t group = 0; group < group_count; ++group) {
            const auto delta_y = payload_u16(payload, pos);
            const auto run_count = payload_u16(payload, pos);
            y += delta_y;
            if (y + run_count > IndexedFramebuffer::kHeight ||
                pos + run_count > payload.size()) {
                throw std::runtime_error("ANM type-3 run exceeds framebuffer");
            }
            for (std::uint16_t i = 0; i < run_count; ++i) {
                pixels[
                    static_cast<std::size_t>(y++) *
                        IndexedFramebuffer::kWidth +
                    static_cast<std::size_t>(x)] = payload[pos++];
            }
        }
    }

    if (pos != payload.size()) {
        throw std::runtime_error("ANM type-3 leaves unconsumed payload");
    }
}

void AnmCinematic::apply_type8(
    const std::vector<std::uint8_t>& payload) {
    constexpr std::size_t kPlaneSize = 0x5000;
    constexpr int kBytesPerScanline = 80;
    constexpr int kFirstRow = 51;
    constexpr int kRows = 256;

    if (payload.size() != 0x14000U) {
        throw std::runtime_error("ANM type-8 payload is not 0x14000 bytes");
    }

    auto& pixels = framebuffer_.mutable_pixels();
    for (int plane = 0; plane < 4; ++plane) {
        const auto base = static_cast<std::size_t>(plane) * kPlaneSize;
        for (int row = 0; row < kRows; ++row) {
            const auto src =
                base + static_cast<std::size_t>(row) * kBytesPerScanline;
            const auto dst_row =
                static_cast<std::size_t>(kFirstRow + row) *
                IndexedFramebuffer::kWidth;
            for (int byte_x = 0; byte_x < kBytesPerScanline; ++byte_x) {
                const int x = byte_x * 4 + plane;
                pixels[dst_row + static_cast<std::size_t>(x)] =
                    payload[src + static_cast<std::size_t>(byte_x)];
            }
        }
    }
}

std::vector<std::string> AnmCinematic::wrap_subtitle(
    const std::string& text, const Type2Font& font) const {
    std::vector<std::string> lines;
    std::string current;

    auto flush = [&]() {
        if (!current.empty()) {
            lines.push_back(current);
            current.clear();
        }
    };

    std::size_t pos = 0;
    while (pos <= text.size()) {
        const auto control = text.find('\x01', pos);
        const auto paragraph_end =
            control == std::string::npos ? text.size() : control;
        const auto paragraph = text.substr(pos, paragraph_end - pos);

        std::size_t word_pos = 0;
        while (word_pos < paragraph.size()) {
            while (word_pos < paragraph.size() &&
                   paragraph[word_pos] == ' ') {
                ++word_pos;
            }
            if (word_pos >= paragraph.size()) break;

            const auto end = paragraph.find(' ', word_pos);
            const auto word = paragraph.substr(
                word_pos,
                end == std::string::npos
                    ? std::string::npos
                    : end - word_pos);

            const auto candidate =
                current.empty() ? word : current + " " + word;
            if (!current.empty() && text_width(candidate, font) > 320) {
                flush();
                current = word;
            } else {
                current = candidate;
            }

            if (end == std::string::npos) break;
            word_pos = end + 1;
        }
        flush();

        if (control == std::string::npos) break;
        pos = control + 1;
    }

    return lines;
}

void AnmCinematic::draw_centered_line(
    int y, const std::string& text, const Type2Font& font) {
    const int width = text_width(text, font);
    int x = (IndexedFramebuffer::kWidth - width) / 2;

    for (const unsigned char ch : text) {
        if (ch >= font.raster.count || ch >= font.widths.size()) {
            throw std::runtime_error("ANM subtitle glyph is outside font resource");
        }
        framebuffer_.draw_frame(font.raster, ch, y, x);
        x += font.widths[ch];
    }
}

void AnmCinematic::render_subtitle() {
    if (!subtitle_active_) return;

    framebuffer_.fill_rect(320, 0, 80, 320, 0);
    if (!subtitle_font_) {
        throw std::logic_error("subtitle active without font");
    }

    const auto lines = wrap_subtitle(subtitle_text_, *subtitle_font_);
    int y = 320;
    for (const auto& line : lines) {
        draw_centered_line(y, line, *subtitle_font_);
        y += 16;
        if (y >= IndexedFramebuffer::kHeight) break;
    }
}

AnmFrameResult AnmCinematic::render_next() {
    if (finished()) {
        throw std::out_of_range("ANM render requested after final frame");
    }

    const auto frame_index = next_frame_++;
    const auto& frame = frames_[frame_index];
    bool alternate_image_wait = false;
    std::vector<AnmAudioEvent> audio;

    for (const auto& chunk : frame.chunks) {
        switch (chunk.id) {
            case 0:
                if (chunk.payload.size() != palette_.rgb.size()) {
                    throw std::runtime_error("ANM palette packet is not 768 bytes");
                }
                std::copy(
                    chunk.payload.begin(),
                    chunk.payload.end(),
                    palette_.rgb.begin());
                break;

            case 1:
                framebuffer_.clear(0);
                palette_ = {};
                break;

            case 2:
                apply_type2(chunk.payload);
                break;

            case 3:
                alternate_image_wait = true;
                apply_type3(chunk.payload);
                break;

            case 4:
                throw std::runtime_error(
                    "supplied Ravenloft ANM unexpectedly uses type-4 control packet");

            case 5:
                if (chunk.payload.size() != 4) {
                    throw std::runtime_error("ANM music command size is not 4");
                }
                audio.push_back({
                    AnmAudioCommand::MusicSequence,
                    payload_s16(chunk.payload, 0),
                    payload_s16(chunk.payload, 2),
                    5,
                    payload_s16(chunk.payload, 0)});
                break;

            case 6: {
                if (chunk.payload.size() != 2) {
                    throw std::runtime_error("ANM queued SFX size is not 2");
                }
                const int encoded =
                    static_cast<int>(static_cast<std::uint16_t>(
                        payload_s16(chunk.payload, 0)));
                if (encoded != 3 && encoded != 0x0F && encoded != 0x88) {
                    audio.push_back({
                        AnmAudioCommand::QueuedSfx,
                        encoded,
                        0,
                        6,
                        49 + encoded});
                }
                break;
            }

            case 7: {
                if (chunk.payload.size() != 4) {
                    throw std::runtime_error("ANM subtitle command size is not 4");
                }
                const auto mode = static_cast<std::uint16_t>(
                    payload_s16(chunk.payload, 0));
                const auto font_id = static_cast<std::uint16_t>(
                    payload_s16(chunk.payload, 2));
                framebuffer_.fill_rect(320, 0, 80, 320, 0);
                if (mode == 0) {
                    subtitle_active_ = false;
                    subtitle_text_.clear();
                    subtitle_font_.reset();
                } else {
                    subtitle_active_ = true;
                    subtitle_text_ = assets_.text(
                        static_cast<int>(
                            static_cast<std::int16_t>(mode)) +
                        0x324);
                    subtitle_font_ = assets_.res0_font(font_id);
                }
                break;
            }

            case 8:
                alternate_image_wait = true;
                apply_type8(chunk.payload);
                break;

            case 9:
                if (chunk.payload.size() != 2) {
                    throw std::runtime_error("ANM direct SFX size is not 2");
                }
                audio.push_back({
                    AnmAudioCommand::DirectSfx,
                    payload_s16(chunk.payload, 0),
                    0,
                    10,
                    static_cast<std::uint16_t>(
                        payload_s16(chunk.payload, 0))});
                break;
        }
    }

    render_subtitle();

    const int wait_ticks = alternate_image_wait
        ? (cinematic_number_ == 12 ? 2 : 6)
        : default_wait_;

    return AnmFrameResult{
        static_cast<int>(frame_index),
        wait_ticks,
        std::move(audio)};
}

}  // namespace rloft
