#pragma once
// Read-only, fail-closed host transport for DOS INT 21h AH=3Dh/3Fh/3Eh.
// This is a host service component, not an emulation of DOS/4GW or game boot.
// No writes, implicit host-current-directory lookup, or DOS device access.
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <map>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>

namespace rloft_recomp {
struct DosFileResult {
    bool carry;
    std::uint16_t ax;
};
// DOS INT 21h AH=42h: carry-clear returns DX:AX, carry-set AX=error.
struct DosSeekResult {
    bool carry;
    std::uint16_t ax;
    std::uint16_t dx;
    std::uint32_t offset() const noexcept {
        return (static_cast<std::uint32_t>(dx)<<16u)|ax;
    }
};

class ReadOnlyDosFiles {
public:
    explicit ReadOnlyDosFiles(std::filesystem::path installation_root)
        : root_(std::move(installation_root)) {
        std::error_code ec;
        const auto st=std::filesystem::symlink_status(root_,ec);
        if(ec || !std::filesystem::is_directory(st) ||
           std::filesystem::is_symlink(st))
            throw std::invalid_argument("DOS file root must be an existing non-symlink directory");
    }

    // AL=00h (read-only, compatibility sharing) only. Other modes fail
    // without opening the host file; original callers retain DOS errors.
    DosFileResult open(const std::string& dos_name,std::uint8_t al) {
        if(al!=0u) return {true,12u}; // invalid access
        if(!valid_basename(dos_name)) return {true,3u}; // path not found
        const auto wanted=ascii_upper(dos_name);
        std::filesystem::path match;
        unsigned matches=0;
        std::error_code ec;
        for(std::filesystem::directory_iterator it(root_,ec),end; !ec && it!=end;
            it.increment(ec)) {
            const auto name=it->path().filename().string();
            if(ascii_upper(name)!=wanted)continue;
            ++matches;
            const auto status=it->symlink_status(ec);
            if(ec || !std::filesystem::is_regular_file(status) ||
               std::filesystem::is_symlink(status))
                return {true,5u}; // access denied
            match=it->path();
        }
        if(ec) return {true,5u};
        if(matches==0u) return {true,2u}; // file not found
        if(matches!=1u) return {true,5u}; // ambiguous on case-sensitive hosts
        if(files_.size()>=0xFFF9u)return {true,4u}; // too many open files
        std::uint16_t handle=5u;
        while(files_.count(handle)!=0u && handle<0xFFFEu)++handle;
        if(files_.count(handle)!=0u)return {true,4u};
        std::ifstream file(match,std::ios::in|std::ios::binary);
        if(!file.is_open())return {true,5u};
        files_.emplace(handle,OpenFile{std::move(file),0u});
        return {false,handle};
    }

    // Count is the DOS CX word, not an unbounded C++ allocation request.
    DosFileResult read(std::uint16_t handle,std::uint8_t* destination,
                       std::size_t count) {
        auto it=files_.find(handle);
        if(it==files_.end())return {true,6u}; // invalid handle
        if(count>0xFFFFu)return {true,1u}; // invalid function/unsupported count
        if(count && destination==nullptr)return {true,5u};
        if(!count)return {false,0u};
        auto& opened=it->second;
        if(count>std::numeric_limits<std::uint32_t>::max()-opened.cursor)
            return {true,1u}; // constrained 32-bit guest position
        auto& file=opened.stream;
        // Clear EOF/fail status before every positioned read. The explicit
        // guest cursor also makes seek-after-EOF independent of host iostream
        // recovery behavior and preserves independent DOS handles.
        file.clear();
        file.seekg(static_cast<std::streamoff>(opened.cursor),std::ios::beg);
        if(file.fail())return {true,5u};
        file.read(reinterpret_cast<char*>(destination),
                  static_cast<std::streamsize>(count));
        const auto actual=file.gcount();
        if(file.bad() || actual<0)return {true,5u};
        opened.cursor+=static_cast<std::uint32_t>(actual);
        return {false,static_cast<std::uint16_t>(actual)};
    }

    // Signed CX:DX relative displacement. AL=0 start, 1 current, 2 end.
    // No writes occur; seeking beyond EOF is allowed, just as on DOS files.
    DosSeekResult seek(std::uint16_t handle,std::int32_t displacement,
                       std::uint8_t origin) {
        const auto it=files_.find(handle);
        if(it==files_.end())return {true,6u,0u};
        if(origin>2u)return {true,1u,0u};
        std::int64_t base=0;
        if(origin==1u)base=it->second.cursor;
        if(origin==2u) {
            auto& file=it->second.stream;
            file.clear();
            file.seekg(0,std::ios::end);
            if(file.fail())return {true,5u,0u};
            const auto end=file.tellg();
            if(end<0)return {true,5u,0u};
            base=static_cast<std::int64_t>(end);
        }
        const auto target=base+static_cast<std::int64_t>(displacement);
        if(target<0 || target>std::numeric_limits<std::uint32_t>::max())
            return {true,1u,0u};
        it->second.cursor=static_cast<std::uint32_t>(target);
        return {false,static_cast<std::uint16_t>(target&0xFFFFu),
                static_cast<std::uint16_t>((target>>16u)&0xFFFFu)};
    }

    DosFileResult close(std::uint16_t handle) {
        const auto it=files_.find(handle);
        if(it==files_.end())return {true,6u};
        // A successful EOF read leaves failbit/eofbit set; that is not
        // a failed DOS AH=3Eh close. Clear prior read status first.
        it->second.stream.clear();
        it->second.stream.close();
        const bool failed=it->second.stream.fail();
        files_.erase(it);
        return failed?DosFileResult{true,5u}:DosFileResult{false,0u};
    }

    std::size_t open_handles() const noexcept {return files_.size();}

private:
    static std::string ascii_upper(const std::string& s) {
        std::string v=s;
        for(char& ch:v)if(ch>='a' && ch<='z')ch=static_cast<char>(ch-'a'+'A');
        return v;
    }
    static bool valid_basename(const std::string& v) {
        if(v.empty() || v.size()>12u)return false;
        const auto dot=v.find('.');
        if(dot==std::string::npos) {
            if(v.size()>8u)return false;
        } else if(dot==0u || dot>8u || dot==v.size()-1u ||
                  v.size()-dot-1u>3u ||
                  v.find('.',dot+1u)!=std::string::npos)
            return false;
        for(unsigned char ch:v) {
            if(ch=='.')continue;
            if(!((ch>='A'&&ch<='Z')||(ch>='a'&&ch<='z')||
                 (ch>='0'&&ch<='9')||ch=='_'||ch=='-'||ch=='$'))
                return false;
        }
        const auto stem=ascii_upper(v.substr(0,dot));
        if(stem=="CON"||stem=="PRN"||stem=="AUX"||stem=="NUL")
            return false;
        if(stem.size()==4u && ((stem.substr(0,3)=="COM")||
            (stem.substr(0,3)=="LPT")) && stem[3]>='1' && stem[3]<='9')
            return false;
        return true;
    }
    struct OpenFile {std::ifstream stream; std::uint32_t cursor;};
    std::filesystem::path root_;
    std::map<std::uint16_t,OpenFile> files_;
};
} // namespace rloft_recomp
