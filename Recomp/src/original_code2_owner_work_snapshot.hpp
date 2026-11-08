#pragma once
// Bounded, read-only snapshot of the owner's original DOS WORK/LEVnnn.DAT.
// The actual CODE.2 181B4 reader consumes these bytes through the PRIVATE
// Overlay; original 171FC writes are never sent to the owner's filesystem.
#include "original_code2_live_level_files.hpp"
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <system_error>
#include <vector>
namespace rloft { namespace original_work_files {
enum class Result { Missing, Loaded, Unsupported, IoError };
inline Result snapshot(const std::filesystem::path& installation,
                       std::uint32_t level,
                       std::vector<std::uint8_t>& bytes) noexcept {
    if(level>=32u || !bytes.empty())return Result::Unsupported;
    try {
        namespace fs=std::filesystem;
        std::error_code ec;
        const auto work=installation/"WORK";
        const auto directory=fs::symlink_status(work,ec);
        if(ec==std::errc::no_such_file_or_directory ||
           (!ec && directory.type()==fs::file_type::not_found))
            return Result::Missing;
        if(ec)return Result::IoError;
        if(directory.type()!=fs::file_type::directory)
            return Result::Unsupported; // Includes symlinked WORK.
        char name[16]{};
        if(std::snprintf(name,sizeof(name),"LEV%03u.DAT",
                         static_cast<unsigned>(level))!=10)
            return Result::Unsupported;
        const auto file=work/name;
        ec.clear();
        const auto status=fs::symlink_status(file,ec);
        if(ec==std::errc::no_such_file_or_directory ||
           (!ec && status.type()==fs::file_type::not_found))
            return Result::Missing;
        if(ec)return Result::IoError;
        if(status.type()!=fs::file_type::regular)
            return Result::Unsupported; // No symlinks, devices, directories.
        const auto count=fs::file_size(file,ec);
        if(ec)return Result::IoError;
        if(count<0x1Du+0x2A4u+4u ||
           count>rloft::live_level_files::Overlay::capacity)
            return Result::Unsupported;
        std::ifstream in(file,std::ios::binary);
        if(!in)return Result::IoError;
        std::vector<std::uint8_t> captured(static_cast<std::size_t>(count));
        in.read(reinterpret_cast<char*>(captured.data()),
                static_cast<std::streamsize>(captured.size()));
        if(!in || in.gcount()!=static_cast<std::streamsize>(captured.size()) ||
           in.peek()!=std::char_traits<char>::eof())
            return Result::IoError; // File changed during read or truncated.
        bytes=std::move(captured);
        return Result::Loaded;
    }catch(const std::exception&) {
        return Result::IoError; // Never unwind the retained noexcept DOS ABI.
    }
}
}} // namespace rloft::original_work_files
