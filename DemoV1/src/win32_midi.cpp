#ifdef _WIN32
#define NOMINMAX
#include "rloft/win32_midi.hpp"

#include <windows.h>
#include <mmsystem.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <memory>
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

int scaled_expression(int base, int relative_volume) noexcept {
    return std::clamp(
        (base * relative_volume + 50) / 100,
        0,
        127);
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
    bool sequence_active{};
    HMIDIOUT active_out{};
    int relative_volume{100};
    std::array<std::uint8_t, 16> expression{};

    std::thread worker;
    std::thread fade_thread;

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

    void apply_relative_volume_locked() {
        if (!active_out) return;

        for (int channel = 0; channel < 16; ++channel) {
            const int value =
                scaled_expression(
                    expression[static_cast<std::size_t>(channel)],
                    relative_volume);
            const DWORD message =
                static_cast<DWORD>(0xB0 | channel) |
                (static_cast<DWORD>(11) << 8U) |
                (static_cast<DWORD>(value) << 16U);
            midiOutShortMsg(active_out, message);
        }
    }

    void set_relative_volume_locked(int value) {
        relative_volume = std::clamp(value, 0, 100);
        apply_relative_volume_locked();
    }

    void fade_relative_volume(
        int from,
        int to,
        int milliseconds,
        bool wait_for_sequence_start) {
        std::unique_lock<std::mutex> lock(mutex);

        if (wait_for_sequence_start) {
            cv.wait(lock, [this] {
                return stop_requested || sequence_active;
            });
            if (stop_requested) return;
        }

        if (milliseconds <= 0) {
            set_relative_volume_locked(to);
            return;
        }

        const auto epoch = std::chrono::steady_clock::now();
        const auto duration =
            std::chrono::milliseconds(milliseconds);
        const auto deadline = epoch + duration;

        for (;;) {
            if (stop_requested) return;

            const auto now = std::chrono::steady_clock::now();
            if (now >= deadline) {
                set_relative_volume_locked(to);
                return;
            }

            const auto elapsed =
                std::chrono::duration_cast<std::chrono::microseconds>(
                    now - epoch);
            const auto total =
                std::chrono::duration_cast<std::chrono::microseconds>(
                    duration);

            const auto numerator =
                static_cast<std::int64_t>(to - from) *
                elapsed.count();
            const int value =
                from +
                static_cast<int>(
                    numerator / total.count());
            if (value != relative_volume) {
                set_relative_volume_locked(value);
            }

            // The DOS AIL driver services XMIDI at a timer cadence. Updating
            // the host-side relative-volume multiplier at 120 Hz preserves
            // the same audible gradual fade without altering authored CC7.
            const auto next =
                std::min(
                    deadline,
                    now +
                        std::chrono::duration_cast<
                            std::chrono::steady_clock::duration>(
                            std::chrono::duration<double>(
                                1.0 / 120.0)));

            cv.wait_until(
                lock,
                next,
                [this] { return stop_requested; });
        }
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(mutex);
            stop_requested = true;
        }
        cv.notify_all();

        if (fade_thread.joinable()) fade_thread.join();
        if (worker.joinable()) worker.join();

        std::lock_guard<std::mutex> lock(mutex);
        active_out = nullptr;
        sequence_active = false;
    }

    void play(
        XmiSequence sequence,
        int initial_relative_volume,
        int target_relative_volume,
        int fade_milliseconds) {
        stop();

        initial_relative_volume =
            std::clamp(initial_relative_volume, 0, 100);
        target_relative_volume =
            std::clamp(target_relative_volume, 0, 100);

        {
            std::lock_guard<std::mutex> lock(mutex);
            stop_requested = false;
            sequence_active = false;
            active_out = nullptr;
            expression.fill(127);
            relative_volume =
                fade_milliseconds > 0
                    ? initial_relative_volume
                    : target_relative_volume;
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
                    std::lock_guard<std::mutex> lock(mutex);
                    cv.notify_all();
                    return;
                }

                {
                    std::lock_guard<std::mutex> lock(mutex);
                    if (stop_requested) {
                        midiOutClose(out);
                        cv.notify_all();
                        return;
                    }
                    active_out = out;
                    sequence_active = true;
                    apply_relative_volume_locked();
                }
                cv.notify_all();

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
                        tick + static_cast<std::uint64_t>(
                            event.delay_ticks);

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

                        if (controller == 11) {
                            const auto channel = status & 0x0FU;
                            int scaled = value;
                            {
                                std::lock_guard<std::mutex> lock(mutex);
                                expression[
                                    static_cast<std::size_t>(channel)] =
                                    value;
                                scaled = scaled_expression(
                                    value,
                                    relative_volume);
                            }
                            const DWORD message =
                                static_cast<DWORD>(status) |
                                (static_cast<DWORD>(11) << 8U) |
                                (static_cast<DWORD>(scaled) << 16U);
                            midiOutShortMsg(out, message);
                            ++index;
                            continue;
                        }

                        if (controller >= 110 && controller <= 120) {
                            // Miles-specific controller. Bank selection has a
                            // direct General-MIDI analogue; the remaining
                            // control semantics are internal to AIL.
                            if (controller == 114) {
                                const DWORD bank =
                                    static_cast<DWORD>(
                                        0xB0U | (status & 0x0FU)) |
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

                all_notes_off(out);
                midiOutReset(out);
                midiOutClose(out);

                {
                    std::lock_guard<std::mutex> lock(mutex);
                    active_out = nullptr;
                    sequence_active = false;
                }
                cv.notify_all();
            });

        if (fade_milliseconds > 0 &&
            initial_relative_volume != target_relative_volume) {
            fade_thread = std::thread(
                [this,
                 initial_relative_volume,
                 target_relative_volume,
                 fade_milliseconds] {
                    fade_relative_volume(
                        initial_relative_volume,
                        target_relative_volume,
                        fade_milliseconds,
                        true);
                });
        }
    }

    void fade_out_and_stop(int milliseconds) {
        if (fade_thread.joinable()) {
            fade_thread.join();
        }

        int from = 0;
        bool active = false;
        {
            std::lock_guard<std::mutex> lock(mutex);
            active = sequence_active;
            from = relative_volume;
        }

        // CODE.1 BB80 returns immediately for a sequence that is already
        // stopped/done. Otherwise BCB4 waits until the relative volume reaches
        // zero and only then normalizes/stops the active sequence.
        if (active) {
            fade_relative_volume(
                from,
                0,
                milliseconds,
                false);
        }
        stop();
    }
};

Win32MidiPlayer::Win32MidiPlayer()
    : impl_(std::make_unique<Impl>()) {}

Win32MidiPlayer::~Win32MidiPlayer() = default;

void Win32MidiPlayer::play(
    XmiSequence sequence,
    int initial_relative_volume,
    int target_relative_volume,
    int fade_milliseconds) {
    impl_->play(
        std::move(sequence),
        initial_relative_volume,
        target_relative_volume,
        fade_milliseconds);
}

void Win32MidiPlayer::fade_out_and_stop(int milliseconds) {
    impl_->fade_out_and_stop(milliseconds);
}

void Win32MidiPlayer::stop() {
    impl_->stop();
}

}  // namespace rloft
#endif
