#pragma once

#ifdef _WIN32

#include <memory>
#include <vector>

#include "rloft/voc_decoder.hpp"

namespace rloft {

class Win32AudioPlayer final {
public:
    Win32AudioPlayer();
    ~Win32AudioPlayer();

    Win32AudioPlayer(const Win32AudioPlayer&) = delete;
    Win32AudioPlayer& operator=(const Win32AudioPlayer&) = delete;

    void play(PcmSound sound);
    void stop_all();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace rloft

#endif
