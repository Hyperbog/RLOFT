// Opt-in native WinMM audio output for owner-supplied *decoded* original
// Creative Voice PCM8. Real OS buffer completion only; no AIL/32 driver,
// VOC codec guessing, Sound Blaster IRQ/DMA timing or source game callback
// integration is implied. Never ships original game media.
#include "original_voc_pcm8.hpp"
#include "original_code2_pcm_packet.hpp"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#endif

namespace {
std::vector<std::uint8_t> owner_file(const std::filesystem::path& path) {
    if(!std::filesystem::is_regular_file(path) ||
       std::filesystem::file_size(path)>17u*1024u*1024u)
        throw std::runtime_error("Owner-private VOC file missing or oversized");
    std::ifstream in(path,std::ios::binary);
    if(!in)throw std::runtime_error("Cannot open private VOC file");
    return {std::istreambuf_iterator<char>(in),std::istreambuf_iterator<char>()};
}
#ifdef _WIN32
void ensure(MMRESULT status,const char* what) {
    if(status!=MMSYSERR_NOERROR)
        throw std::runtime_error(std::string(what)+
                                 " WinMM status="+std::to_string(status));
}
void output_and_verify_os_completion(const rloft::host_voc::Sample& s) {
    if(s.pcm8.empty() || s.pcm8.size()>static_cast<std::size_t>(0x7FFFFFFFu))
        throw std::runtime_error("Unverified WinMM sample buffer bounds");
    WAVEFORMATEX fmt{};
    fmt.wFormatTag=WAVE_FORMAT_PCM;
    fmt.nChannels=1u; fmt.nSamplesPerSec=s.rate_hz;
    fmt.wBitsPerSample=8u;fmt.nBlockAlign=1u;
    fmt.nAvgBytesPerSec=s.rate_hz;fmt.cbSize=0u;
    HANDLE signal=CreateEventW(nullptr,FALSE,FALSE,nullptr);
    if(!signal)throw std::runtime_error("WinMM completion event allocation failed");
    HWAVEOUT output=nullptr;
    WAVEHDR header{};
    bool prepared=false;
    try {
        ensure(waveOutOpen(&output,WAVE_MAPPER,&fmt,
                           reinterpret_cast<DWORD_PTR>(signal),0,CALLBACK_EVENT),
               "waveOutOpen: no authenticated OS output device");
        header.lpData=reinterpret_cast<LPSTR>(
            const_cast<std::uint8_t*>(s.pcm8.data()));
        header.dwBufferLength=static_cast<DWORD>(s.pcm8.size());
        ensure(waveOutPrepareHeader(output,&header,sizeof(header)),
               "waveOutPrepareHeader");
        prepared=true;
        ensure(waveOutWrite(output,&header,sizeof(header)),"waveOutWrite");
        // An OS signal alone is not completion: WinMM also signals for
        // open/close; require the WHDR_DONE bit set by real buffer return.
        const std::uint64_t budget_ms=
            std::min<std::uint64_t>(120000u,
                10000u+(std::uint64_t(s.pcm8.size())*1000u/s.rate_hz));
        const ULONGLONG begin=GetTickCount64();
        while((header.dwFlags&WHDR_DONE)==0u) {
            const ULONGLONG elapsed=GetTickCount64()-begin;
            if(elapsed>=budget_ms)
                throw std::runtime_error("WinMM did not signal real buffer completion");
            const DWORD pending=WaitForSingleObject(
                signal,static_cast<DWORD>(
                    std::min<std::uint64_t>(budget_ms-elapsed,250u)));
            if(pending!=WAIT_TIMEOUT && pending!=WAIT_OBJECT_0)
                throw std::runtime_error("WinMM completion event wait failed");
        }
        std::cout<<"RLOFT_WINMM_REAL_SAMPLE_COMPLETION_PASS"
                 <<" bytes="<<s.pcm8.size()
                 <<" rate_hz="<<s.rate_hz
                 <<" completion=OS_WAVEHDR_DONE"
                 <<" real_winmm_driver=YES"
                 <<" original_Miles_AIL=NOT_CONNECTED"
                 <<" original_irq_dma=NOT_CERTIFIED\n"<<std::flush;
        ensure(waveOutUnprepareHeader(output,&header,sizeof(header)),
               "waveOutUnprepareHeader");
        prepared=false;
        ensure(waveOutClose(output),"waveOutClose");
        output=nullptr;
        CloseHandle(signal);
    }catch(...) {
        if(output) {
            (void)waveOutReset(output);
            if(prepared)(void)waveOutUnprepareHeader(output,&header,sizeof(header));
            (void)waveOutClose(output);
        }
        CloseHandle(signal);
        throw;
    }
}
#endif
} // namespace

int main(int argc,char** argv) {
    try {
        const bool play=argc==4 && std::string(argv[3])=="--play";
        const bool direct=argc>=2 && std::string(argv[1])=="--voc";
        const bool source_packet=argc>=2 && std::string(argv[1])=="--packet";
        if((argc!=3 && !play) || (!direct && !source_packet))
            throw std::invalid_argument(
                "usage: rloft_host_original_voc_waveout "
                "(--voc PRIVATE_SAMPLE.voc | --packet PRIVATE_27F24_PACKET.bin) [--play]");
        const auto data=owner_file(argv[2]);
        const auto pcm=source_packet?
            rloft::source_audio::decode_original_length_prefixed_voc(data):
            rloft::host_voc::decode_pcm8_mono(data);
        std::cout<<"RLOFT_PRIVATE_VOC_PCM8_VERIFIED"
                 <<" pcm_bytes="<<pcm.pcm8.size()
                 <<" rate_hz="<<pcm.rate_hz
                 <<" decode=CREATIVE_VOC_TYPE1_PCM8"
                 <<" original_27f24_packet="<<(source_packet?"YES":"NO")
                 <<" output_request="<<(play?"USER_EXPLICIT":"NONE")<<"\n";
        if(!play)return 0;
#ifdef _WIN32
        output_and_verify_os_completion(pcm);
        return 0;
#else
        throw std::runtime_error(
            "Real host output requires WinMM on Windows; no fallback driver result invented");
#endif
    }catch(const std::exception& e) {
        std::cerr<<"RLOFT_PRIVATE_VOC_PLAYBACK_FAIL: "<<e.what()<<"\n";
        return 1;
    }
}
