#pragma once
// Authenticates the unmodified original retail CODE.1/CODE.2 bytes before
// using them as a reconstruction oracle. SHA-256 is portable C++17; the
// resulting identity is not evidence of original DOS/4GW runtime parity.
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace rloft_recomp {
namespace original_media_detail {
inline std::uint32_t rotate_right(std::uint32_t x, unsigned n) noexcept {
    return (x >> n) | (x << (32u - n));
}
inline constexpr std::array<std::uint32_t, 64> round_constants = {
    0x428a2f98u,0x71374491u,0xb5c0fbcfu,0xe9b5dba5u,0x3956c25bu,0x59f111f1u,0x923f82a4u,0xab1c5ed5u,
    0xd807aa98u,0x12835b01u,0x243185beu,0x550c7dc3u,0x72be5d74u,0x80deb1feu,0x9bdc06a7u,0xc19bf174u,
    0xe49b69c1u,0xefbe4786u,0x0fc19dc6u,0x240ca1ccu,0x2de92c6fu,0x4a7484aau,0x5cb0a9dcu,0x76f988dau,
    0x983e5152u,0xa831c66du,0xb00327c8u,0xbf597fc7u,0xc6e00bf3u,0xd5a79147u,0x06ca6351u,0x14292967u,
    0x27b70a85u,0x2e1b2138u,0x4d2c6dfcu,0x53380d13u,0x650a7354u,0x766a0abbu,0x81c2c92eu,0x92722c85u,
    0xa2bfe8a1u,0xa81a664bu,0xc24b8b70u,0xc76c51a3u,0xd192e819u,0xd6990624u,0xf40e3585u,0x106aa070u,
    0x19a4c116u,0x1e376c08u,0x2748774cu,0x34b0bcb5u,0x391c0cb3u,0x4ed8aa4au,0x5b9cca4fu,0x682e6ff3u,
    0x748f82eeu,0x78a5636fu,0x84c87814u,0x8cc70208u,0x90befffau,0xa4506cebu,0xbef9a3f7u,0xc67178f2u
};
} // namespace original_media_detail

inline std::string sha256_hex(const std::vector<std::uint8_t>& bytes) {
    using namespace original_media_detail;
    const std::uint64_t length=bytes.size();
    if(length>(std::numeric_limits<std::uint64_t>::max()/8u)-72u)
        throw std::length_error("SHA-256 input length overflow");
    const std::uint64_t bits=length*8u;
    const std::uint64_t blocks=(length+9u+63u)/64u;
    const std::uint64_t padded=blocks*64u;
    std::array<std::uint32_t,8> state={
        0x6a09e667u,0xbb67ae85u,0x3c6ef372u,0xa54ff53au,
        0x510e527fu,0x9b05688cu,0x1f83d9abu,0x5be0cd19u
    };
    for(std::uint64_t block=0;block<blocks;++block) {
        std::array<std::uint32_t,64> w{};
        for(std::size_t i=0;i<16u;++i) {
            std::uint32_t v=0;
            for(unsigned k=0;k<4u;++k) {
                const std::uint64_t position=block*64u+i*4u+k;
                std::uint8_t byte=0;
                if(position<length)byte=bytes[static_cast<std::size_t>(position)];
                else if(position==length)byte=0x80u;
                else if(position>=padded-8u)
                    byte=static_cast<std::uint8_t>(
                        bits>>((padded-1u-position)*8u));
                v=(v<<8u)|byte;
            }
            w[i]=v;
        }
        for(unsigned i=16;i<64u;++i) {
            const auto a=w[i-15u],b=w[i-2u];
            const auto s0=rotate_right(a,7u)^rotate_right(a,18u)^(a>>3u);
            const auto s1=rotate_right(b,17u)^rotate_right(b,19u)^(b>>10u);
            w[i]=w[i-16u]+s0+w[i-7u]+s1;
        }
        auto a=state[0],b=state[1],c=state[2],d=state[3];
        auto e=state[4],f=state[5],g=state[6],h=state[7];
        for(unsigned i=0;i<64u;++i) {
            const auto choose=(e&f)^(~e&g);
            const auto majority=(a&b)^(a&c)^(b&c);
            const auto sum1=rotate_right(e,6u)^rotate_right(e,11u)^rotate_right(e,25u);
            const auto sum0=rotate_right(a,2u)^rotate_right(a,13u)^rotate_right(a,22u);
            const auto t1=h+sum1+choose+round_constants[i]+w[i];
            const auto t2=sum0+majority;
            h=g;g=f;f=e;e=d+t1;d=c;c=b;b=a;a=t1+t2;
        }
        const std::array<std::uint32_t,8> round={a,b,c,d,e,f,g,h};
        for(unsigned i=0;i<8u;++i)state[i]+=round[i];
    }
    constexpr char alphabet[]="0123456789abcdef";
    std::string hex;
    hex.reserve(64u);
    for(const auto word:state)
        for(unsigned n=0;n<8u;++n)
            hex+=alphabet[(word>>((7u-n)*4u))&0xFu];
    return hex;
}
inline bool is_original_code1(const std::vector<std::uint8_t>& bytes) {
    return bytes.size()==277787u &&
           sha256_hex(bytes)=="fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed";
}
inline bool is_original_code2(const std::vector<std::uint8_t>& bytes) {
    return bytes.size()==558689u &&
           sha256_hex(bytes)=="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314";
}
} // namespace rloft_recomp
