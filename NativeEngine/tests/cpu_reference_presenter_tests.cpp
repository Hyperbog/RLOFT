#include "rloft_native/cpu_reference_presenter.hpp"
#include <cassert>
#include <cstdint>
#include <stdexcept>

using namespace rloft_native;

int main() {
    IndexedFrame frame;
    frame.pixels.resize(IndexedFrame::width * IndexedFrame::height, 0);
    frame.pixels.front() = 1;
    frame.pixels.back() = 1;
    frame.dac6[3] = 63;
    frame.dac6[4] = 32;
    frame.dac6[5] = 0;
    assert(diagnostic_dac6_to_u8(0) == 0);
    assert(diagnostic_dac6_to_u8(32) == 130);
    assert(diagnostic_dac6_to_u8(63) == 255);
    const auto source = frame_identity(frame);
    CpuReferencePresenter cpu;
    cpu.present(frame);
    assert(cpu.rgba().size() == 320U * 400U * 4U);
    assert(cpu.rgba()[0] == 255 && cpu.rgba()[1] == 130);
    assert(cpu.rgba()[2] == 0 && cpu.rgba()[3] == 255);
    assert(cpu.rgba()[4] == 0 && cpu.rgba()[5] == 0 && cpu.rgba()[7] == 255);
    const auto tail = cpu.rgba().size() - 4;
    assert(cpu.rgba()[tail] == 255 && cpu.rgba()[tail + 1] == 130);
    const auto original_rgba_hash = cpu.rgba_fnv1a64();
    cpu.present(frame);
    assert(cpu.rgba_fnv1a64() == original_rgba_hash);
    frame.dac6[4] = 63;
    assert(frame_identity(frame).pixels_fnv1a64 == source.pixels_fnv1a64);
    assert(frame_identity(frame).palette_fnv1a64 != source.palette_fnv1a64);
    cpu.present(frame);
    assert(cpu.rgba()[1] == 255);
    assert(cpu.rgba_fnv1a64() != original_rgba_hash);
    frame.dac6[4] = 32;
    cpu.present(frame);
    assert(cpu.rgba_fnv1a64() == original_rgba_hash);
}
