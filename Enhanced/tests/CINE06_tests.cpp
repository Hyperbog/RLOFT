#include "rloft/CINE06.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {
using rloft::Cine06Edits;
using rloft::Cine06File;
using rloft::Cine06Frame;
using rloft::Cine06FrameChange;
using rloft::Cine06FrameInsert;
using rloft::Cine06Packet;

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}
template<class F> void rejects(F&& action) {
    bool caught = false;
    try { action(); } catch (const std::exception&) { caught = true; }
    require(caught, "CINE06 accepted invalid or stale frame operation");
}
void put16(std::vector<std::uint8_t>& out, std::uint16_t value) {
    out.push_back(static_cast<std::uint8_t>(value));
    out.push_back(static_cast<std::uint8_t>(value >> 8U));
}
void put32(std::vector<std::uint8_t>& out, std::uint32_t value) {
    for (unsigned shift = 0U; shift < 32U; shift += 8U)
        out.push_back(static_cast<std::uint8_t>(value >> shift));
}
void emit(std::vector<std::uint8_t>& out, const Cine06Frame& frame) {
    put16(out, static_cast<std::uint16_t>(frame.packets.size()));
    for (const auto& p : frame.packets) {
        put16(out, p.id);
        put32(out, static_cast<std::uint32_t>(p.payload.size()));
        out.insert(out.end(), p.payload.begin(), p.payload.end());
    }
}
std::vector<std::uint8_t> rle(std::uint8_t color) {
    std::vector<std::uint8_t> result;
    result.reserve(2400U);
    for (unsigned y = 0U; y < 400U; ++y)
        result.insert(result.end(), {0x81U,color,0x81U,color,0xC1U,color});
    return result;
}
std::vector<std::uint8_t> synthetic() {
    std::vector<std::uint8_t> bytes;
    put16(bytes,6U);put16(bytes,1U);
    emit(bytes, {{{1U,{}},{0U,std::vector<std::uint8_t>(768U,9U)},
                   {2U,rle(33U)}}});
    emit(bytes, {{{5U,{19U,0U,0U,0U}}}});
    emit(bytes, {{{8U,rloft::Cine00File::encode_viewport(
        std::vector<std::uint8_t>(320U*256U,44U))}}});
    emit(bytes, {{{3U,{0U,0U}}}});
    emit(bytes, {{{7U,{31U,0U,7U,0U}}}});
    emit(bytes, {{{9U,{31U,0U}}}});
    return bytes;
}
std::vector<std::uint8_t> bytes(const Cine06Frame& frame) {
    std::vector<std::uint8_t> out;
    emit(out, frame);
    return out;
}
void exercise(const std::vector<std::uint8_t>& original, bool retail) {
    Cine06File file(original);
    const auto original_frames = file.stats().frames;
    require(file.encode() == original && file.bytes() == original &&
            file.stats().default_wait == 1U,
            "CINE06 initial parse/serialize changed source bytes");
    if (retail) {
        require(original.size() == 7339821U && original_frames == 181U &&
                file.stats().chunks == 184U &&
                file.stats().by_id ==
                    std::array<std::size_t,10>{{24U,1U,1U,59U,0U,1U,2U,4U,89U,3U}},
                "CINE06 original file inventory differs");
        require(file.frame(0U).packets.size() == 3U &&
                file.frame(0U).packets[2U].id == 2U &&
                file.frame(0U).packets[2U].payload.size() == 28891U &&
                file.frame(1U).packets[0U].id == 7U &&
                file.frame(2U).packets[0U].id == 5U &&
                file.frame(8U).packets[0U].id == 8U &&
                file.frame(9U).packets[0U].id == 3U &&
                file.frame(9U).packets[0U].payload ==
                    std::vector<std::uint8_t>({0U,0U}),
                "CINE06 original frame controls differ");
        std::size_t no_op_deltas = 0U;
        std::size_t image_timing = 0U;
        for (std::size_t f = 0U; f < original_frames; ++f) {
            image_timing += file.frame_info(f).uses_image_wait ? 1U : 0U;
            for (const auto& p : file.frame(f).packets) {
                if (p.id == 3U) {
                    require(p.payload == std::vector<std::uint8_t>({0U,0U}),
                            "CINE06 unexpected nonempty sparse image delta");
                    ++no_op_deltas;
                }
            }
        }
        require(no_op_deltas == 59U && image_timing == 148U &&
                file.cues().size() == 10U &&
                file.snapshot(8U).pixels.size() == 128000U &&
                file.frame_info(8U).wait_ticks == 6U &&
                file.frame_info(9U).wait_ticks == 6U &&
                file.frame_info(1U).wait_ticks == 1U,
                "CINE06 original cinematic timing or timeline differs");
    } else {
        require(original_frames == 6U && file.stats().chunks == 8U &&
                file.cues().size() == 3U &&
                file.frame_info(2U).wait_ticks == 6U &&
                file.snapshot(2U).pixels[51U * 320U] == 44U,
                "CINE06 synthetic original failed structural validation");
    }

    const auto replace_index = std::size_t{1U};
    const auto duplicate_index = retail ? std::size_t{8U} : std::size_t{2U};
    const auto remove_index = retail ? std::size_t{9U} : std::size_t{3U};
    const auto restore_anchor = retail ? std::size_t{10U} : std::size_t{4U};
    const auto old_command = file.frame(replace_index);
    const auto old_image = file.frame(duplicate_index);
    const auto old_delta = file.frame(remove_index);
    auto changed_command = old_command;
    require(changed_command.packets.size() == 1U &&
            (changed_command.packets[0U].id == 5U ||
             changed_command.packets[0U].id == 7U),
            "CINE06 fixture command not editable");
    changed_command.packets[0U].payload[0U] = static_cast<std::uint8_t>(
        changed_command.packets[0U].payload[0U] + 1U);
    Cine06Edits first{};
    first.expected_frames = original_frames;
    first.expected_wait = 1U;
    first.replacement_wait = 2U;
    first.changes = {{replace_index,old_command,changed_command},
                     {remove_index,old_delta,std::nullopt}};
    first.inserts = {{duplicate_index,old_image,old_image}};
    file.apply(first);
    require(file.stats().frames == original_frames &&
            file.stats().default_wait == 2U &&
            bytes(file.frame(replace_index)) == bytes(changed_command) &&
            bytes(file.frame(duplicate_index)) == bytes(old_image) &&
            bytes(file.frame(duplicate_index + 1U)) == bytes(old_image) &&
            file.frame_info(duplicate_index).wait_ticks == 6U &&
            file.cues().size() == (retail ? 10U : 3U),
            "CINE06 atomic insert/replace/delete/wait edit failed");
    if (retail)
        require(file.cues().front().frame == 1U &&
                file.cues().front().first == 32U,
                "CINE06 original subtitle was not reindexed correctly");
    const auto checkpoint = file.encode();

    rejects([&]{file.apply(first);}); // Stale wait or original command.
    Cine06Edits invalid{};
    invalid.expected_frames = original_frames - 1U;
    invalid.inserts = {{duplicate_index,old_image,old_image}};
    rejects([&]{file.apply(invalid);});
    invalid.expected_frames = original_frames;
    invalid.inserts = {{duplicate_index,old_delta,old_image}};
    rejects([&]{file.apply(invalid);}); // Stale anchor.
    invalid.inserts = {{duplicate_index,file.frame(duplicate_index),old_image},
                       {duplicate_index,file.frame(duplicate_index),old_image}};
    rejects([&]{file.apply(invalid);}); // Duplicate insert.
    invalid = {};
    invalid.expected_frames = original_frames;
    invalid.changes = {{replace_index,old_command,changed_command}};
    rejects([&]{file.apply(invalid);}); // Original expectation stale.
    invalid.changes = {{replace_index,file.frame(replace_index),changed_command},
                       {replace_index,file.frame(replace_index),changed_command}};
    rejects([&]{file.apply(invalid);}); // Duplicate replacement.
    invalid.changes = {{replace_index,file.frame(replace_index),
                        Cine06Frame{{Cine06Packet{8U,{1U,2U}}}}}};
    rejects([&]{file.apply(invalid);}); // Invalid planar image.
    invalid.changes = {{replace_index,file.frame(replace_index),
                        Cine06Frame{{Cine06Packet{4U,{}}}}}};
    rejects([&]{file.apply(invalid);}); // Uncertified control packet.
    invalid.changes = {{original_frames,old_image,std::nullopt}};
    rejects([&]{file.apply(invalid);}); // Out of bounds.
    invalid = {};
    invalid.expected_wait = 3U;
    invalid.replacement_wait = 4U;
    rejects([&]{file.apply(invalid);});
    invalid = {};
    invalid.expected_wait = 2U;
    rejects([&]{file.apply(invalid);});
    require(file.encode() == checkpoint,
            "CINE06 failed edit changed live source bytes");

    Cine06Edits undo{};
    undo.expected_frames = original_frames;
    undo.expected_wait = 2U;
    undo.replacement_wait = 1U;
    undo.changes = {{replace_index,file.frame(replace_index),old_command},
                    {duplicate_index,file.frame(duplicate_index),std::nullopt}};
    undo.inserts = {{restore_anchor,file.frame(restore_anchor),old_delta}};
    file.apply(undo);
    require(file.encode() == original,
            "CINE06 inverse transaction did not restore exact source bytes");
    file.apply({});
    require(file.encode() == original, "CINE06 no-op changed source bytes");

    // Append and then delete, testing the past-end anchor independent of
    // ordered original-frame indices.
    Cine06Edits append{};
    append.expected_frames = original_frames;
    append.inserts = {{original_frames,std::nullopt,Cine06Frame{}}};
    file.apply(append);
    require(file.stats().frames == original_frames + 1U &&
            file.frame(original_frames).packets.empty(),
            "CINE06 append did not produce an empty frame");
    Cine06Edits remove{};
    remove.expected_frames = original_frames + 1U;
    remove.changes = {{original_frames,Cine06Frame{},std::nullopt}};
    file.apply(remove);
    require(file.encode() == original, "CINE06 append/remove was not reversible");
    rejects([&]{(void)file.frame(original_frames);});
    rejects([&]{(void)file.snapshot(original_frames);});
    auto broken = original;
    broken.pop_back();
    rejects([&]{(void)Cine06File(std::move(broken));});
}

void capacity() {
    // ANM frame count is a 16-bit field, even when every frame is empty.
    std::vector<std::uint8_t> data;
    put16(data,65535U);put16(data,1U);
    data.resize(4U + 2U * 65535U);
    Cine06File file(data);
    Cine06Edits edits{};
    edits.expected_frames = 65535U;
    edits.inserts = {{65535U,std::nullopt,Cine06Frame{}}};
    rejects([&]{file.apply(edits);});
    require(file.encode() == data, "CINE06 overflow changed source bytes");
}
} // namespace

int main(int argc, char** argv) {
    exercise(synthetic(),false);
    capacity();
    if (argc > 2) throw std::invalid_argument("usage: CINE06_tests [CINE06.ANM]");
    if (argc == 2) {
        std::ifstream f(argv[1],std::ios::binary);
        if (!f) throw std::runtime_error("cannot open original CINE06.ANM");
        const std::vector<std::uint8_t> data{
            std::istreambuf_iterator<char>(f),std::istreambuf_iterator<char>()};
        exercise(data,true);
    }
}
