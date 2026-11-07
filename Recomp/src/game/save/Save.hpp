#pragma once
// Original CODE.2 171FC/181B4 private writable WORK/LEVnnn.DAT overlay.
// Never writes to the owner's original game installation or saved files.
// The caller must validate original source filename bytes before open/remove.
// Handles are deliberately outside the host DOS read-only handle range.
#include "../../original_code2_checked_dos_seek.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <map>
#include <exception>
#include <utility>
#include <vector>

namespace rloft { namespace live_level_files {
struct Overlay {
    static constexpr std::uint32_t read_handle=0xFFFEu;
    static constexpr std::uint32_t write_handle=0xFFFDu;
    static constexpr std::uint32_t invalid=0xFFFFFFFFu;
    static constexpr std::uint32_t capacity=4u*1024u*1024u;
    std::map<std::uint32_t,std::vector<std::uint8_t>> levels;
    std::uint32_t reading=invalid,writing=invalid;
    std::uint32_t read_position=0u,write_position=0u;
    std::uint32_t source_reads=0u,source_writes=0u;
    bool failed=false;

    static bool level_name(const std::uint8_t* name,std::size_t count,
                           std::uint32_t& level) noexcept {
        if(!name || count<16u)return false;
        constexpr char prefix[]="WORK\\LEV";
        for(std::uint32_t i=0;i<8u;++i) {
            const auto c=static_cast<char>(name[i]);
            // Lowercase is valid only for letters. In particular the
            // separator must remain a literal DOS backslash, not the
            // unrelated ASCII code obtained by adding 0x20 to '\\'.
            const bool letter=prefix[i]>='A'&&prefix[i]<='Z';
            if(c!=prefix[i] &&
               !(letter&&c==static_cast<char>(prefix[i]+('a'-'A'))))
                return false;
        }
        for(std::uint32_t i=8u;i<=10u;++i)
            if(name[i]<'0'||name[i]>'9')return false;
        const char suffix[]=".DAT";
        for(std::uint32_t i=0;i<4u;++i) {
            const char c=static_cast<char>(name[11u+i]);
            if(c!=suffix[i] &&
               !(suffix[i]>='A' && suffix[i]<='Z' &&
                 c==static_cast<char>(suffix[i]+('a'-'A'))))
                return false;
        }
        if(name[15u]!=0u)return false;
        level=static_cast<std::uint32_t>(name[8u]-'0')*100u+
              static_cast<std::uint32_t>(name[9u]-'0')*10u+
              static_cast<std::uint32_t>(name[10u]-'0');
        return level<32u;
    }
    // A saved game may install a prevalidated, owner-private LEVnnn blob
    // BEFORE invoking original 07938/181B4. No source callback, actor,
    // camera, map or original game file is changed by this transport.
    // A failed insertion leaves the source bytes available to its caller
    // and marks this overlay unusable instead of throwing through the
    // noexcept retained DOS callback or half-installing a saved world.
    bool install_saved_snapshot(std::uint32_t level,
                                std::vector<std::uint8_t>& bytes) noexcept {
        if(level>=32u || bytes.empty() || bytes.size()>capacity ||
           !idle() || has(level)) {
            failed=true;
            return false;
        }
        if(!try_install_into(levels,level,bytes)) {
            failed=true;
            return false;
        }
        return true;
    }
    // The same actual noexcept insertion boundary can be exercised with
    // a deterministic throwing map in media-free CTests. This is not a
    // fallback guest snapshot or an allocation bypass in production.
    template<class Map>
    static bool try_install_into(Map& target,std::uint32_t level,
                                 std::vector<std::uint8_t>& bytes) noexcept {
        try {
            return target.try_emplace(level,std::move(bytes)).second;
        }catch(const std::exception&) {
            return false;
        }
    }
    bool has(std::uint32_t level) const noexcept {
        return levels.find(level)!=levels.end();
    }
    bool erase(std::uint32_t level) noexcept {
        if(level>=32u||reading==level||writing==level)return false;
        levels.erase(level);
        return true;
    }
    std::uint32_t open(std::uint32_t level) noexcept {
        if(level>=32u||reading!=invalid||writing!=invalid||
           !has(level)){failed=true;return invalid;}
        reading=level;read_position=0u;return read_handle;
    }
    std::uint32_t create(std::uint32_t level) noexcept {
        if(level>=32u||reading!=invalid||writing!=invalid){
            failed=true;return invalid;
        }
        // std::map insertion allocates; the guest file service must
        // fail closed instead of terminating inside this noexcept ABI.
        try {
            const auto it=levels.try_emplace(level).first;
            it->second.clear();
        }catch(const std::exception&) {
            failed=true;return invalid;
        }
        writing=level;write_position=0u;
        return write_handle;
    }
    bool close(std::uint32_t handle) noexcept {
        if(handle==read_handle && reading!=invalid) {
            reading=invalid;read_position=0u;return true;
        }
        if(handle==write_handle && writing!=invalid) {
            writing=invalid;write_position=0u;return true;
        }
        failed=true;return false;
    }
    // CODE.2 3131C carries a signed 32-bit DOS seek displacement.
    std::uint32_t seek(std::uint32_t handle,std::uint32_t off,
                       std::uint32_t origin) noexcept {
        const bool is_read=handle==read_handle&&reading!=invalid;
        const bool is_write=handle==write_handle&&writing!=invalid;
        if((!is_read&&!is_write)||origin>2u){failed=true;return invalid;}
        const auto level=is_read?reading:writing;
        const auto it=levels.find(level);
        if(it==levels.end()){failed=true;return invalid;}
        std::uint32_t pos=invalid;
        if(!rloft::dos_seek::compute(
               off,origin,is_read?read_position:write_position,
               it->second.size(),capacity,pos)){
            failed=true;return invalid;
        }
        if(is_read)read_position=pos;
        else write_position=pos;
        return pos;
    }
    std::uint32_t write(const std::uint8_t* data,std::uint32_t count) noexcept {
        if(writing==invalid || (!data&&count) ||
           std::uint64_t(write_position)+count>capacity){
            failed=true;return 0u;
        }
        // create() must have installed the entry. Avoid operator[],
        // which can allocate during a noexcept original-source write.
        const auto it=levels.find(writing);
        if(it==levels.end()){failed=true;return 0u;}
        auto& bytes=it->second;
        const std::uint32_t end=write_position+count;
        try {
            if(end>bytes.size())bytes.resize(end,0u);
        }catch(const std::exception&) {
            // vector::resize has a strong guarantee for byte vectors;
            // neither the write position nor source byte counter moves.
            failed=true;return 0u;
        }
        if(count)std::copy_n(data,count,bytes.data()+write_position);
        write_position=end;++source_writes;
        return count;
    }
    std::uint32_t read(std::uint8_t* dst,std::uint32_t count) noexcept {
        const auto it=levels.find(reading);
        if(reading==invalid || it==levels.end() || (!dst&&count) ||
           std::uint64_t(read_position)+count>it->second.size()){
            failed=true;return 0u;
        }
        if(count)std::copy_n(it->second.data()+read_position,count,dst);
        read_position+=count;++source_reads;
        return count;
    }
    bool idle() const noexcept {
        return !failed && reading==invalid && writing==invalid;
    }
};
}} // namespace rloft::live_level_files

// Bounded original 181B4 saved-world actor payload verification.
// Source-authenticated standalone WORK level witness for 181B4's saved
// 22-byte actor path, independent of LEVELS.DAT's packed 11-byte path.
#include "../../../../Code2/graphics/original_levels_grid.hpp"
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

// Original CODE.2 07938 / 181B4 saved-world and transition policy.
// Authentic CODE.2 07938 writes the unsigned -1 sentinel at C6C8
// before 181B4 reads the level number from SAVE.DAT. CODE.2 uses
// LEVELS.DAT's 32-entry index, not a host allowlist of level pairs.
// These predicates authorize bounded SOURCE-selected requests only;
// the caller must also validate real LEVELS.DAT records, live guest
// dimensions, read-only file ownership and source transition state.
#include <cstdint>
namespace rloft { namespace original_saved_level_policy {
constexpr std::uint32_t no_departing_level=0xFFFFFFFFu;
constexpr std::uint32_t archived_level_count=32u;
inline bool indexed_level(std::uint32_t level) noexcept {
    return level<archived_level_count;
}
inline bool authenticated_no_departing_level(
    std::uint32_t prior,std::uint32_t requested,
    bool restore_active,std::uint32_t source_level_callbacks,
    bool saved_world_available,std::uint32_t source_reads,
    std::uint32_t source_save_opens) noexcept {
    return prior==no_departing_level && indexed_level(requested) &&
        restore_active && source_level_callbacks==1u &&
        saved_world_available && source_reads!=0u && source_save_opens==1u;
}
inline bool source_selected_transition(std::uint32_t prior,
                                       std::uint32_t requested) noexcept {
    // Source 181B4 itself decides the destination. This only excludes
    // invalid archive indices, the save-only sentinel and no-op pairs.
    // Original 2D5D0/2D3D8 scripts, tiles and event logic decide WHICH
    // valid level should be reached; never invent portal relationships.
    return indexed_level(prior) && indexed_level(requested) &&
        prior!=requested;
}
}} // namespace rloft::original_saved_level_policy
