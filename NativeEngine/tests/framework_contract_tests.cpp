#include "rloft_native/frame_contract.hpp"
#include "rloft_native/vfs_contract.hpp"
#include <cassert>
#include <cstdint>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

using namespace rloft_native;

namespace {
template <typename F> void rejects(F operation) {
    bool rejected = false;
    try { operation(); }
    catch (const std::exception&) { rejected = true; }
    assert(rejected);
}

class RecordingPresenter final : public IndexedPresenter {
public:
    FrameIdentity last{};
    void present(const IndexedFrame& frame) override { last = frame_identity(frame); }
};

void frame_contract() {
    IndexedFrame frame;
    frame.pixels.resize(IndexedFrame::width * IndexedFrame::height);
    for (std::size_t i = 0; i < frame.pixels.size(); ++i)
        frame.pixels[i] = static_cast<std::uint8_t>(i % 256);
    for (std::size_t i = 0; i < frame.dac6.size(); ++i)
        frame.dac6[i] = static_cast<std::uint8_t>(i % 64);
    const auto before = frame_identity(frame);
    RecordingPresenter renderer;
    renderer.present(frame);
    assert(renderer.last.pixels_fnv1a64 == before.pixels_fnv1a64);
    assert(renderer.last.palette_fnv1a64 == before.palette_fnv1a64);
    frame.dac6[0] ^= 1;
    const auto palette_changed = frame_identity(frame);
    assert(palette_changed.pixels_fnv1a64 == before.pixels_fnv1a64);
    assert(palette_changed.palette_fnv1a64 != before.palette_fnv1a64);
    assert(fit_original_aspect(640, 480).width == 640);
    const auto wide = fit_original_aspect(1000, 480);
    assert(wide.width == 640 && wide.x == 180 && wide.y == 0);
    const auto tall = fit_original_aspect(640, 800);
    assert(tall.height == 480 && tall.y == 160);
    rejects([&] { IndexedFrame invalid = frame; invalid.pixels.pop_back(); invalid.validate(); });
    rejects([&] { IndexedFrame invalid = frame; invalid.dac6[12] = 64; invalid.validate(); });
    rejects([] { fit_original_aspect(0, 480); });
}

void vfs_contract() {
    const std::map<std::string, std::vector<std::uint8_t>> original{
        {"INIT", {1, 2, 3}}, {"RES4", {4, 5}}, {"SAVE.DAT", {8}}};
    unsigned base_reads = 0;
    ReadOnlyVfs vfs(
        [&](std::string_view p) { return original.find(std::string(p)) != original.end(); },
        [&](std::string_view p) { ++base_reads; return original.at(std::string(p)); });
    assert(vfs.read("init").data == (std::vector<std::uint8_t>{1, 2, 3}));
    assert(base_reads == 1);
    vfs.mount({"mod-a", {{"res4", {30}}, {"NEWAREA/FLOOR.RES", {40}}}});
    vfs.mount({"mod-b", {{"RES4", {50}}}});
    assert(vfs.read("Res4").provider == "mod-b");
    assert(vfs.read("Res4").data == (std::vector<std::uint8_t>{50}));
    assert(base_reads == 1); // shadowed original was never read
    assert(vfs.read("newarea\\floor.res").provider == "mod-a");
    assert(vfs.read("SAVE.DAT").provider == "original");
    assert(vfs.providers("RES4") ==
           (std::vector<std::string>{"mod-b", "mod-a", "original"}));
    rejects([&] { vfs.read("../INIT"); });
    rejects([&] { vfs.read("C:\\INIT"); });
    rejects([&] { vfs.read("RES4//OTHER"); });
    rejects([&] { vfs.mount({"mod-c", {{"SAVE.DAT", {0}}}}); });
    rejects([&] { vfs.mount({"mod-c", {{"X", {1}}, {"x", {2}}}}); });
    rejects([&] { vfs.mount({"mod-a", {{"NEW", {1}}}}); });
    rejects([&] { vfs.read("ABSENT"); });
    assert(vfs.read("RES4").provider == "mod-b");
    assert(original.at("RES4") == (std::vector<std::uint8_t>{4, 5}));
}
}

int main() { frame_contract(); vfs_contract(); }
