#pragma once
// Source-authenticated standalone WORK level witness for 181B4's saved
// 22-byte actor path, independent of LEVELS.DAT's packed 11-byte path.
#include "../../Code2/graphics/original_levels_grid.hpp"
#include <array>
#include <cstdint>
#include <stdexcept>
#include <vector>
namespace rloft { namespace saved_level_actor_proof {
using U=std::uint32_t; using B=std::uint8_t;
struct Proof { U moving=0,primary=0,alternate=0,indexed_columns=0; };
inline Proof verify(const std::vector<B>& saved,
                    const std::vector<B>& guest,U level) {
    const auto grid=original_levels::decode_saved_grid(saved,level);
    auto check=[](bool v,const char* msg){if(!v)throw std::runtime_error(msg);};
    auto le16=[](const B* b){return original_levels::le16(b);};
    auto le32=[](const B* b){return original_levels::le32(b);};
    std::array<U,7> sec{};
    for(U i=0;i<7;++i)sec[i]=le32(saved.data()+1u+i*4u);
    check(sec[0]==0x1Du && sec[1]==
          0x1Du+0x2A4u+U(grid.width)*grid.height*2u,
          "Saved 181B4 source grid section differs");
    std::size_t pos=sec[1];
    U columns=0u;
    for(int pass=0;pass<2;++pass) {
        if(pass==1) {
            check(pos<sec[2],"Saved 181B4 CF50 byte missing");
            ++pos;
        }
        for(U column=0;column<grid.width;++column) {
            check(pos+2u<=sec[2],"Saved 181B4 column header overrun");
            const U count=le16(saved.data()+pos);pos+=2u;
            check(std::uint64_t(pos)+std::uint64_t(count)*10u<=sec[2],
                  "Saved 181B4 column list overrun");
            if(pass==0)columns+=count;
            pos+=std::size_t(count)*10u;
        }
    }
    check(pos==sec[2],"Saved 181B4 column section misaligned");
    Proof result{le16(grid.header.data()+0x28Au),
                 le16(grid.header.data()+0x28Eu),
                 le16(grid.header.data()+0x28Cu),columns};
    check(result.moving<=512u && result.primary<=512u &&
          result.alternate<=512u,"Saved 181B4 actor count unbounded");
    check(std::uint64_t(sec[2])+std::uint64_t(result.moving)*22u==sec[3] &&
          std::uint64_t(sec[3])+2u+
              std::uint64_t(result.primary)*22u==sec[4] &&
          std::uint64_t(sec[4])+std::uint64_t(result.alternate)*22u==sec[5] &&
          std::uint64_t(sec[5])+0x514u==sec[6],
          "Saved 181B4 actor/script section offsets differ");
    check(std::uint64_t(sec[3])+2u<=saved.size() &&
          le16(saved.data()+sec[3])==result.primary,
          "Saved 181B4 redundant primary count differs");
    const std::array<U,3> counts{result.moving,result.primary,result.alternate};
    const std::array<U,3> offsets{sec[2],sec[3]+2u,sec[4]};
    const std::array<U,3> targets{0x14D80u,0x22238u,0x17618u};
    constexpr std::array<U,5> stable_fields{0u,2u,4u,6u,14u};
    for(U group=0;group<3;++group) {
        check(std::uint64_t(offsets[group])+std::uint64_t(counts[group])*22u<=saved.size() &&
              std::uint64_t(targets[group])+std::uint64_t(counts[group])*22u<=guest.size(),
              "Saved 181B4 actor source/guest range invalid");
        for(U i=0;i<counts[group];++i) {
            const B* disk=saved.data()+offsets[group]+i*22u;
            const B* ram=guest.data()+targets[group]+i*22u;
            // Original 17A68 can change moving-actor byte 0B after
            // loading and change type 4E to type 1 on a quest branch.
            // Do not compare mutable 0B, and refuse uncertified 4E.
            if(group==0u && le16(disk+14u)==0x4Eu)
                throw std::runtime_error("Saved 181B4 4E postload requires authentication");
            for(U off:stable_fields)
                check(le16(disk+off)==le16(ram+off),
                      "Saved 181B4 stable actor field differs from WORK");
        }
    }
    return result;
}
}} // namespace rloft::saved_level_actor_proof
