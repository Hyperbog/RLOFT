#pragma once

#ifdef _WIN32

#include <memory>

#include "rloft/xmidi.hpp"

namespace rloft {

class Win32MidiPlayer final {
public:
    Win32MidiPlayer();
    ~Win32MidiPlayer();

    Win32MidiPlayer(const Win32MidiPlayer&) = delete;
    Win32MidiPlayer& operator=(const Win32MidiPlayer&) = delete;

    void play(
        XmiSequence sequence,
        int initial_relative_volume = 100,
        int target_relative_volume = 100,
        int fade_milliseconds = 0);

    // Mirrors AIL_set_relative_volume(..., 0, milliseconds) followed by the
    // CODE.1 BCB4 wait-until-volume-zero and sequence stop.
    void fade_out_and_stop(int milliseconds);
    void stop();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace rloft

#endif
