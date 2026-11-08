#pragma once
// Original 31218 -> DOS 37E39 'CHARS' file access classification.
// A missing CHARS file is an ordinary DOS access failure, not a host
// I/O fault and not permission to synthesize a party.
#include <filesystem>
#include <system_error>
namespace rloft { namespace original_chars_access {
enum class Outcome { Present, Missing, UnsupportedFileType, HostError };
inline Outcome probe(const std::filesystem::path& installation) {
    std::error_code error;
    const auto status=std::filesystem::status(installation/"CHARS",error);
    if(error==std::errc::no_such_file_or_directory ||
       (!error && status.type()==std::filesystem::file_type::not_found))
        return Outcome::Missing;
    if(error)return Outcome::HostError;
    if(status.type()!=std::filesystem::file_type::regular)
        return Outcome::UnsupportedFileType;
    return Outcome::Present;
}
}} // namespace rloft::original_chars_access
