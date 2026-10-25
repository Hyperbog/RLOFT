#pragma once
// Experimental host-backed sampled SFX device for the *original* CODE.2
// 28110 -> 28168 -> 27F24 software sequence. This maps an AIL-shaped
// callback ABI onto WinMM; it IS NOT Miles/AIL hardware or IRQ/DMA parity.
// The queue remains untouched if native device/payload preflight fails.
#include "original_code2_pcm_packet.hpp"
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <mmsystem.h>
#endif
namespace rloft { namespace source_audio {
#ifdef _WIN32
class WinMMSourceSfx {
    struct Device {
        host_voc::Sample pcm{};
        WAVEFORMATEX format{};
        HWAVEOUT out=nullptr;
        HANDLE event=nullptr;
        WAVEHDR header{};
        bool prepared=false;
        bool submitted=false;
        ~Device() noexcept {
            if(out) {
                if(submitted)(void)waveOutReset(out);
                if(prepared)(void)waveOutUnprepareHeader(out,&header,sizeof(header));
                (void)waveOutClose(out);
            }
            if(event)CloseHandle(event);
        }
        Device()=default;
        Device(const Device&)=delete;
        Device& operator=(const Device&)=delete;
    };
    std::unique_ptr<Device> staged_, active_;
    unsigned completed_=0;
    static void ensure(MMRESULT v,const char* operation) {
        if(v!=MMSYSERR_NOERROR)
            throw std::runtime_error(std::string(operation)+
                       " returned OS WinMM error "+std::to_string(v));
    }
public:
    WinMMSourceSfx()=default;
    WinMMSourceSfx(const WinMMSourceSfx&)=delete;
    WinMMSourceSfx& operator=(const WinMMSourceSfx&)=delete;
    // Call BEFORE entering original 28168: no source queue is consumed
    // unless a real waveOut device and its exact header are prepared.
    void preflight(const std::vector<std::uint8_t>& packet) {
        auto candidate=std::make_unique<Device>();
        candidate->pcm=decode_original_length_prefixed_voc(packet);
        if(candidate->pcm.pcm8.empty() || candidate->pcm.rate_hz==0u)
            throw std::runtime_error("WinMM original sample payload invalid");
        candidate->format.wFormatTag=WAVE_FORMAT_PCM;
        candidate->format.nChannels=1u;
        candidate->format.nSamplesPerSec=candidate->pcm.rate_hz;
        candidate->format.wBitsPerSample=8u;
        candidate->format.nBlockAlign=1u;
        candidate->format.nAvgBytesPerSec=candidate->pcm.rate_hz;
        candidate->event=CreateEventW(nullptr,FALSE,FALSE,nullptr);
        if(!candidate->event)
            throw std::runtime_error("Host SFX completion event unavailable");
        ensure(waveOutOpen(&candidate->out,WAVE_MAPPER,
               &candidate->format,
               reinterpret_cast<DWORD_PTR>(candidate->event),
               0u,CALLBACK_EVENT),"waveOutOpen");
        candidate->header.lpData=reinterpret_cast<LPSTR>(
            candidate->pcm.pcm8.data());
        candidate->header.dwBufferLength=
            static_cast<DWORD>(candidate->pcm.pcm8.size());
        ensure(waveOutPrepareHeader(candidate->out,
              &candidate->header,sizeof(candidate->header)),
              "waveOutPrepareHeader");
        candidate->prepared=true;
        staged_=std::move(candidate);
    }
    bool status_playing() {
        if(!active_ || !active_->submitted)return false;
        if((active_->header.dwFlags&WHDR_DONE)!=0u) {
            active_.reset();
            ++completed_;
            return false;
        }
        return true;
    }
    void stop_active() noexcept {active_.reset();}
    void discard_staged() noexcept {staged_.reset();}
    void start_staged() {
        if(!staged_ || !staged_->prepared || staged_->submitted)
            throw std::runtime_error("WinMM source device not preflighted");
        // Old device is reset only after 27F24's original 36817 stop
        // callback, if playing; a fresh sample does not inherit state.
        stop_active();
        ensure(waveOutWrite(staged_->out,
              &staged_->header,sizeof(staged_->header)),
              "waveOutWrite");
        staged_->submitted=true;
        active_=std::move(staged_);
    }
    unsigned completions() const noexcept{return completed_;}
};
#else
// No fake silent device for Linux, non-Windows, or headless CI.
class WinMMSourceSfx {
public:
    void preflight(const std::vector<std::uint8_t>&) {
        throw std::runtime_error("Original SFX host device needs WinMM on Windows");
    }
    bool status_playing(){return false;}
    void stop_active() noexcept {}
    void discard_staged() noexcept {}
    void start_staged() {
        throw std::runtime_error("No authenticated host sound device");
    }
    unsigned completions() const noexcept{return 0u;}
};
#endif
}} // namespace
