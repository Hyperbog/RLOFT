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

    void play(XmiSequence sequence);
    void stop();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace rloft

#endif
