#pragma once
// Original CODE.2 171FC/181B4 private writable WORK/LEVnnn.DAT overlay.
// Never writes to the owner's original game installation or saved files.
// The caller must validate original source filename bytes before open/remove.
// Handles are deliberately outside the host DOS read-only handle range.
#include "original_code2_checked_dos_seek.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <map>
#include <exception>
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
