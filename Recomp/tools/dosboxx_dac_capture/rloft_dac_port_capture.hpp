// SPDX-License-Identifier: GPL-2.0-or-later
// RLOFT opt-in DOSBox-X 2026.10.01 VGA DAC capture.
// Emits *observed* emulator-accepted byte writes; not original CODE.2 bytes.
// Records only to a local owner-specified file; never enabled by default.
#ifndef RLOFT_DOSBOXX_DAC_CAPTURE_HPP
#define RLOFT_DOSBOXX_DAC_CAPTURE_HPP

#include <cstdio>
#include <cstdlib>
#include <cstdint>

namespace rloft_dac_capture {
static const unsigned long kMaxEvents = 200000ul;

class Capture {
public:
    Capture() : file_(NULL), count_(0), overflow_(false) {
        const char* path = std::getenv("RLOFT_DAC_CAPTURE_JSONL");
        if (path == NULL || *path == '\0') return;
        file_ = std::fopen(path, "wb");
        if (file_ == NULL) {
            std::fprintf(stderr, "RLOFT DAC capture: cannot create specified private output\n");
            return;
        }
        if (std::fprintf(file_,
            "{\"rloft_hook\":\"dosboxx-v2026.10.01-dac-v1\","
            "\"upstream_vga_blob\":\"509d42fbead67a56594e49cec55549bc6fcc6d7c\","
            "\"capture_stage\":\"accepted_after_lockout_and_rgb6_mask\","
            "\"max_events\":200000}\n") < 0 || std::fflush(file_) != 0) {
            fail();
        }
    }

    ~Capture() {
        if (file_ != NULL) {
            std::fflush(file_);
            std::fclose(file_);
        }
    }

    void record(unsigned int port, unsigned int value,
                unsigned int cs_hint, unsigned int eip_hint) {
        if (file_ == NULL || overflow_) return;
        if (count_ == kMaxEvents) {
            // Converter treats this marker as a HARD FAILURE, not a truncated
            // stream that may happen to equal the saved DAC.
            std::fprintf(file_,
                "{\"overflow\":true,\"after_seq\":%lu}\n", count_);
            std::fflush(file_);
            overflow_ = true;
            return;
        }
        if (std::fprintf(file_,
            "{\"seq\":%lu,\"port\":%u,\"value\":%u,"
            "\"guest_cs_hint\":%u,\"guest_eip_hint\":%u}\n",
            count_, port, value, cs_hint, eip_hint) < 0 ||
            std::fflush(file_) != 0) {
            fail();
            return;
        }
        ++count_;
    }

private:
    void fail() {
        if (file_ != NULL) {
            std::fprintf(stderr, "RLOFT DAC capture: write failure; trace unusable\n");
            std::fclose(file_);
        }
        file_ = NULL;
        overflow_ = true;
    }

    std::FILE* file_;
    unsigned long count_;
    bool overflow_;
};

static inline void record(unsigned int port, unsigned int value,
                          unsigned int cs_hint, unsigned int eip_hint) {
    static Capture capture;
    capture.record(port, value, cs_hint, eip_hint);
}
} // namespace rloft_dac_capture

#endif
