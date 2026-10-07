#ifdef _WIN32
#define NOMINMAX
#include "rloft/win32_midi.hpp"

#include <windows.h>
#include <mmsystem.h>

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

namespace rloft {

namespace {

DWORD short_message(const std::vector<std::uint8_t>& bytes) {
    if (bytes.empty() || bytes.size() > 3) {
        throw std::runtime_error("invalid MIDI short message");
    }
    DWORD value = bytes[0];
    if (bytes.size() >= 2) {
        value |= static_cast<DWORD>(bytes[1]) << 8U;
    }
    if (bytes.size() >= 3) {
        value |= static_cast<DWORD>(bytes[2]) << 16U;
    }
    return value;
}

void all_notes_off(HMIDIOUT out) {
    for (int channel = 0; channel < 16; ++channel) {
        const DWORD message =
            static_cast<DWORD>(0xB0 | channel) |
            (static_cast<DWORD>(123) << 8U);
        midiOutShortMsg(out, message);
    }
}

}  // namespace

struct Win32MidiPlayer::Impl {
    struct NoteOff {
        std::uint64_t tick{};
        DWORD message{};
    };

    struct Loop {
        std::size_t target{};
        int remaining{};
        bool infinite{};
    };

    std::mutex mutex;
    std::condition_variable cv;
    bool stop_requested{};
    std::thread worker;

    ~Impl() {
        stop();
    }

    bool wait_until(
        std::unique_lock<std::mutex>& lock,
        const std::chrono::steady_clock::time_point& epoch,
        std::uint64_t tick) {
        constexpr double kXmidiHz = 120.0;
        const auto deadline =
            epoch +
            std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                std::chrono::duration<double>(
                    static_cast<double>(tick) / kXmidiHz));
        cv.wait_until(lock, deadline, [this] { return stop_requested; });
        return !stop_requested;
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(mutex);
            stop_requested = true;
        }
        cv.notify_all();
        if (worker.joinable()) worker.join();
    }

    void play(XmiSequence sequence) {
        stop();

        {
            std::lock_guard<std::mutex> lock(mutex);
            stop_requested = false;
        }

        worker = std::thread(
            [this, sequence = std::move(sequence)]() mutable {
                HMIDIOUT out = nullptr;
                if (midiOutOpen(
                        &out,
                        MIDI_MAPPER,
                        0,
                        0,
                        CALLBACK_NULL) != MMSYSERR_NOERROR) {
                    return;
                }

                const auto epoch = std::chrono::steady_clock::now();
                std::uint64_t tick = 0;
                std::size_t index = 0;
                std::vector<NoteOff> note_offs;
                std::vector<Loop> loops;

                auto send_due = [&](std::uint64_t through) {
                    for (;;) {
                        auto it = std::min_element(
                            note_offs.begin(),
                            note_offs.end(),
                            [](const NoteOff& a, const NoteOff& b) {
                                return a.tick < b.tick;
                            });
                        if (it == note_offs.end() || it->tick > through) break;

                        std::unique_lock<std::mutex> lock(mutex);
                        if (!wait_until(lock, epoch, it->tick)) return false;
                        lock.unlock();

                        midiOutShortMsg(out, it->message);
                        tick = std::max(tick, it->tick);
                        note_offs.erase(it);
                    }
                    return true;
                };

                auto send_sysex = [&](const std::vector<std::uint8_t>& bytes) {
                    if (bytes.empty()) return;
                    std::vector<char> buffer(bytes.begin(), bytes.end());
                    if (static_cast<std::uint8_t>(buffer.back()) != 0xF7U) {
                        buffer.push_back(static_cast<char>(0xF7));
                    }

                    MIDIHDR header{};
                    header.lpData = buffer.data();
                    header.dwBufferLength =
                        static_cast<DWORD>(buffer.size());
                    if (midiOutPrepareHeader(
                            out,
                            &header,
                            sizeof(MIDIHDR)) != MMSYSERR_NOERROR) {
                        return;
                    }
                    midiOutLongMsg(
                        out,
                        &header,
                        sizeof(MIDIHDR));
                    while ((header.dwFlags & MHDR_DONE) == 0) {
                        Sleep(1);
                    }
                    midiOutUnprepareHeader(
                        out,
                        &header,
                        sizeof(MIDIHDR));
                };

                while (index < sequence.events.size()) {
                    {
                        std::lock_guard<std::mutex> lock(mutex);
                        if (stop_requested) break;
                    }

                    const auto& event = sequence.events[index];
                    const auto event_tick =
                        tick + static_cast<std::uint64_t>(event.delay_ticks);

                    if (!send_due(event_tick)) break;

                    {
                        std::unique_lock<std::mutex> lock(mutex);
                        if (!wait_until(lock, epoch, event_tick)) break;
                    }
                    tick = event_tick;

                    if (event.kind == XmiEventKind::EndOfTrack) {
                        break;
                    }

                    if (event.kind == XmiEventKind::SystemExclusive) {
                        send_sysex(event.bytes);
                        ++index;
                        continue;
                    }

                    if (event.bytes.empty()) {
                        ++index;
                        continue;
                    }

                    const auto status = event.bytes[0];
                    const auto family = status & 0xF0U;

                    if (family == 0xB0U && event.bytes.size() >= 3) {
                        const auto controller = event.bytes[1];
                        const auto value = event.bytes[2];

                        if (controller == 116) {
                            if (loops.size() >= 4) {
                                break;
                            }
                            loops.push_back(
                                Loop{
                                    index + 1,
                                    static_cast<int>(value),
                                    value == 0});
                            ++index;
                            continue;
                        }

                        if (controller == 117) {
                            if (loops.empty()) {
                                ++index;
                                continue;
                            }

                            auto& loop = loops.back();
                            if (value < 64) {
                                loops.pop_back();
                                ++index;
                                continue;
                            }

                            if (loop.infinite) {
                                index = loop.target;
                                continue;
                            }

                            --loop.remaining;
                            if (loop.remaining > 0) {
                                index = loop.target;
                            } else {
                                loops.pop_back();
                                ++index;
                            }
                            continue;
                        }

                        if (controller >= 110 && controller <= 120) {
                            // Miles-specific controller. Bank selection has a
                            // direct General-MIDI analogue; the remaining
                            // control semantics are internal to AIL.
                            if (controller == 114) {
                                const DWORD bank =
                                    static_cast<DWORD>(0xB0U | (status & 0x0FU)) |
                                    (static_cast<DWORD>(0) << 8U) |
                                    (static_cast<DWORD>(value) << 16U);
                                midiOutShortMsg(out, bank);
                            }
                            ++index;
                            continue;
                        }
                    }

                    midiOutShortMsg(out, short_message(event.bytes));

                    if (event.kind == XmiEventKind::NoteOn &&
                        event.bytes.size() == 3) {
                        const auto channel = status & 0x0FU;
                        const auto note = event.bytes[1];
                        const auto duration =
                            std::max<std::uint32_t>(
                                1U, event.note_duration);
                        const DWORD off =
                            static_cast<DWORD>(0x80U | channel) |
                            (static_cast<DWORD>(note) << 8U);
                        note_offs.push_back(
                            NoteOff{
                                tick + duration,
                                off});
                    }

                    ++index;
                }

                // XMIDI loop branching does not flush the note queue, but
                // stopping/ending a sequence releases its active channels.
                all_notes_off(out);
                midiOutReset(out);
                midiOutClose(out);
            });
    }
};

Win32MidiPlayer::Win32MidiPlayer()
    : impl_(std::make_unique<Impl>()) {}

Win32MidiPlayer::~Win32MidiPlayer() = default;

void Win32MidiPlayer::play(XmiSequence sequence) {
    impl_->play(std::move(sequence));
}

void Win32MidiPlayer::stop() {
    impl_->stop();
}

}  // namespace rloft
#endif
