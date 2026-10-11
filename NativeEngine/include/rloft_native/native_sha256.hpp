#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace rloft_native {
namespace native_hash_detail {
inline std::uint32_t rotate(std::uint32_t v, unsigned shift) {
    return (v >> shift) | (v << (32U-shift));
}
}
// Standalone SHA-256 for metadata; a computed hash does not independently
// authenticate where a user-supplied original file came from.
inline std::string native_sha256_hex(const std::vector<std::uint8_t>& input) {
    if(input.size() > std::numeric_limits<std::uint64_t>::max()/8U)
        throw std::length_error("SHA-256 message length overflow");
    std::vector<std::uint8_t> message(input);
    const auto bit_length=static_cast<std::uint64_t>(input.size())*8U;
    message.push_back(0x80);
    while(message.size()%64U != 56U)message.push_back(0);
    for(int shift=56;shift>=0;shift-=8)
        message.push_back(static_cast<std::uint8_t>(bit_length>>shift));
    static constexpr std::array<std::uint32_t,64> k{{
        0x428a2f98U,0x71374491U,0xb5c0fbcfU,0xe9b5dba5U,
        0x3956c25bU,0x59f111f1U,0x923f82a4U,0xab1c5ed5U,
        0xd807aa98U,0x12835b01U,0x243185beU,0x550c7dc3U,
        0x72be5d74U,0x80deb1feU,0x9bdc06a7U,0xc19bf174U,
        0xe49b69c1U,0xefbe4786U,0x0fc19dc6U,0x240ca1ccU,
        0x2de92c6fU,0x4a7484aaU,0x5cb0a9dcU,0x76f988daU,
        0x983e5152U,0xa831c66dU,0xb00327c8U,0xbf597fc7U,
        0xc6e00bf3U,0xd5a79147U,0x06ca6351U,0x14292967U,
        0x27b70a85U,0x2e1b2138U,0x4d2c6dfcU,0x53380d13U,
        0x650a7354U,0x766a0abbU,0x81c2c92eU,0x92722c85U,
        0xa2bfe8a1U,0xa81a664bU,0xc24b8b70U,0xc76c51a3U,
        0xd192e819U,0xd6990624U,0xf40e3585U,0x106aa070U,
        0x19a4c116U,0x1e376c08U,0x2748774cU,0x34b0bcb5U,
        0x391c0cb3U,0x4ed8aa4aU,0x5b9cca4fU,0x682e6ff3U,
        0x748f82eeU,0x78a5636fU,0x84c87814U,0x8cc70208U,
        0x90befffaU,0xa4506cebU,0xbef9a3f7U,0xc67178f2U}};
    std::array<std::uint32_t,8> h{{
        0x6a09e667U,0xbb67ae85U,0x3c6ef372U,0xa54ff53aU,
        0x510e527fU,0x9b05688cU,0x1f83d9abU,0x5be0cd19U}};
    for(std::size_t offset=0;offset<message.size();offset+=64U) {
        std::array<std::uint32_t,64> w{};
        for(unsigned i=0;i<16;++i) {
            const auto base=offset+4U*i;
            w[i]=(std::uint32_t(message[base])<<24U)|
                 (std::uint32_t(message[base+1])<<16U)|
                 (std::uint32_t(message[base+2])<<8U)|
                  std::uint32_t(message[base+3]);
        }
        for(unsigned i=16;i<64;++i) {
            const auto p=w[i-15],q=w[i-2];
            const auto s0=native_hash_detail::rotate(p,7)^
                          native_hash_detail::rotate(p,18)^(p>>3U);
            const auto s1=native_hash_detail::rotate(q,17)^
                          native_hash_detail::rotate(q,19)^(q>>10U);
            w[i]=w[i-16]+s0+w[i-7]+s1;
        }
        auto a=h[0],b=h[1],c=h[2],d=h[3],e=h[4],f=h[5],g=h[6],j=h[7];
        for(unsigned i=0;i<64;++i) {
            const auto s1=native_hash_detail::rotate(e,6)^
                          native_hash_detail::rotate(e,11)^
                          native_hash_detail::rotate(e,25);
            const auto choose=(e&f)^(~e&g);
            const auto t1=j+s1+choose+k[i]+w[i];
            const auto s0=native_hash_detail::rotate(a,2)^
                          native_hash_detail::rotate(a,13)^
                          native_hash_detail::rotate(a,22);
            const auto majority=(a&b)^(a&c)^(b&c);
            const auto t2=s0+majority;
            j=g;g=f;f=e;e=d+t1;d=c;c=b;b=a;a=t1+t2;
        }
        h[0]+=a;h[1]+=b;h[2]+=c;h[3]+=d;
        h[4]+=e;h[5]+=f;h[6]+=g;h[7]+=j;
    }
    static constexpr char hex[]="0123456789abcdef";
    std::string output;
    output.reserve(64);
    for(const auto value:h)
        for(int shift=28;shift>=0;shift-=4)
            output.push_back(hex[(value>>shift)&0xFU]);
    return output;
}
} // namespace rloft_native
