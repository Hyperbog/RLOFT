#ifdef _WIN32
#define NOMINMAX
#include "rloft/win32_audio.hpp"

#include <windows.h>
#include <mmsystem.h>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

namespace rloft {

struct Win32AudioPlayer::Impl {
    struct Voice {
        std::thread thread;
        std::shared_ptr<std::atomic_bool> done;
        std::shared_ptr<std::atomic_bool> stop_requested;
    };

    std::mutex mutex;
    std::vector<std::unique_ptr<Voice>> voices;

    void reap() {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = voices.begin();
        while (it != voices.end()) {
            if ((*it)->done->load(std::memory_order_acquire)) {
                if ((*it)->thread.joinable()) {
                    (*it)->thread.join();
                }
                it = voices.erase(it);
            } else {
                ++it;
            }
        }
    }

    void play(PcmSound sound) {
        reap();

        auto done = std::make_shared<std::atomic_bool>(false);
        auto stop_requested = std::make_shared<std::atomic_bool>(false);
        auto voice = std::make_unique<Voice>();
        voice->done = done;
        voice->stop_requested = stop_requested;

        voice->thread = std::thread(
            [sound = std::move(sound), done, stop_requested]() mutable {
                HANDLE event = CreateEventW(nullptr, FALSE, FALSE, nullptr);
                if (!event) {
                    done->store(true, std::memory_order_release);
                    return;
                }

                WAVEFORMATEX format{};
                format.wFormatTag = WAVE_FORMAT_PCM;
                format.nChannels = sound.channels;
                format.nSamplesPerSec = sound.sample_rate;
                format.wBitsPerSample = 16;
                format.nBlockAlign =
                    static_cast<WORD>(
                        format.nChannels * format.wBitsPerSample / 8);
                format.nAvgBytesPerSec =
                    format.nSamplesPerSec * format.nBlockAlign;

                HWAVEOUT wave = nullptr;
                const auto open = waveOutOpen(
                    &wave,
                    WAVE_MAPPER,
                    &format,
                    reinterpret_cast<DWORD_PTR>(event),
                    0,
                    CALLBACK_EVENT);
                if (open != MMSYSERR_NOERROR) {
                    CloseHandle(event);
                    done->store(true, std::memory_order_release);
                    return;
                }

                WAVEHDR header{};
                header.lpData = reinterpret_cast<LPSTR>(sound.samples.data());
                header.dwBufferLength = static_cast<DWORD>(
                    sound.samples.size() * sizeof(std::int16_t));

                if (waveOutPrepareHeader(
                        wave, &header, sizeof(header)) == MMSYSERR_NOERROR) {
                    if (waveOutWrite(
                            wave, &header, sizeof(header)) == MMSYSERR_NOERROR) {
                        while ((header.dwFlags & WHDR_DONE) == 0 &&
                               !stop_requested->load(std::memory_order_acquire)) {
                            WaitForSingleObject(event, 20);
                        }
                        if (stop_requested->load(std::memory_order_acquire) &&
                            (header.dwFlags & WHDR_DONE) == 0) {
                            waveOutReset(wave);
                        }
                    }
                    waveOutUnprepareHeader(wave, &header, sizeof(header));
                }

                waveOutClose(wave);
                CloseHandle(event);
                done->store(true, std::memory_order_release);
            });

        std::lock_guard<std::mutex> lock(mutex);
        voices.push_back(std::move(voice));
    }

    void stop_all() {
        std::vector<std::unique_ptr<Voice>> owned;
        {
            std::lock_guard<std::mutex> lock(mutex);
            owned.swap(voices);
        }

        for (auto& voice : owned) {
            voice->stop_requested->store(true, std::memory_order_release);
        }
        for (auto& voice : owned) {
            if (voice->thread.joinable()) {
                voice->thread.join();
            }
        }
    }
};

Win32AudioPlayer::Win32AudioPlayer()
    : impl_(std::make_unique<Impl>()) {}

Win32AudioPlayer::~Win32AudioPlayer() {
    impl_->stop_all();
}

void Win32AudioPlayer::play(PcmSound sound) {
    impl_->play(std::move(sound));
}

void Win32AudioPlayer::stop_all() {
    impl_->stop_all();
}

}  // namespace rloft
#endif
