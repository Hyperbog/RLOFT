// Service-provider verification. Runs without copyrighted game assets.
#include "dos_readonly_files.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

namespace fs=std::filesystem;
using rloft_recomp::ReadOnlyDosFiles;
using rloft_recomp::DosFileResult;
void need(bool yes,const char* msg) {
    if(!yes)throw std::runtime_error(msg);
}
void eq(DosFileResult x,bool carry,unsigned ax,const char* msg) {
    need(x.carry==carry && x.ax==ax,msg);
}
struct TemporaryDirectory {
    fs::path p;
    TemporaryDirectory() {
        p=fs::temp_directory_path()/
          ("rloft-recomp-ro-"+std::to_string(
              std::chrono::steady_clock::now().time_since_epoch().count()));
        need(fs::create_directory(p),"temporary root collision");
    }
    ~TemporaryDirectory(){std::error_code ec;fs::remove_all(p,ec);}
};
int main() {
    try {
        TemporaryDirectory tmp;
        std::array<std::uint8_t,59> source{};
        for(std::size_t i=0;i<source.size();++i)
            source[i]=static_cast<std::uint8_t>(i*3u);
        {std::ofstream f(tmp.p/"SOUND.CFG",std::ios::binary);
         need(f.good(),"cannot create fixture");
         f.write(reinterpret_cast<const char*>(source.data()),source.size());
         need(f.good(),"cannot write fixture");}
        ReadOnlyDosFiles files(tmp.p);
        eq(files.open("subdir\\SOUND.CFG",0),true,3,"traversal accepted");
        eq(files.open("../SOUND.CFG",0),true,3,"parent traversal accepted");
        eq(files.open("C:SOUND.CFG",0),true,3,"drive path accepted");
        eq(files.open("CON",0),true,3,"DOS device accepted");
        eq(files.open("SOUND.CFG",1),true,12,"DOS write access accepted");
        eq(files.open("SOUND.CFG",2),true,12,"DOS read/write access accepted");
        eq(files.open("NOFILE.BIN",0),true,2,"missing file status");
        need(files.open_handles()==0u,"rejected DOS opens allocated handles");
        const auto a=files.open("sound.cfg",0);
        const auto b=files.open("SOUND.CFG",0);
        need(!a.carry && a.ax==5u && !b.carry && b.ax==6u,
             "DOS handle assignment");
        need(files.open_handles()==2u,"independent opens not recorded");
        std::array<std::uint8_t,80> buf{};
        eq(files.read(a.ax,buf.data(),17),false,17,"first partial read");
        need(std::equal(buf.begin(),buf.begin()+17,source.begin()),
             "first partial bytes differ");
        eq(files.read(a.ax,buf.data()+17,42),false,42,"second partial read");
        need(std::equal(buf.begin(),buf.begin()+59,source.begin()),
             "full original-style 59-byte read differs");
        eq(files.read(a.ax,buf.data(),20),false,0,"EOF should return zero");
        eq(files.read(b.ax,buf.data(),59),false,59,
           "independent handle cursor differs");
        need(std::equal(buf.begin(),buf.begin()+59,source.begin()),
             "independent handle data differs");
        eq(files.read(b.ax,nullptr,0),false,0,"zero byte read rejected");
        eq(files.read(b.ax,nullptr,1),true,5,"null output accepted");
        eq(files.read(b.ax,buf.data(),65536),true,1,"oversized CX accepted");
        eq(files.read(999,buf.data(),1),true,6,"invalid read handle");
        eq(files.close(a.ax),false,0,"valid close");
        eq(files.close(a.ax),true,6,"double close accepted");
        eq(files.close(b.ax),false,0,"second close failed");
        need(files.open_handles()==0u,"DOS handle leak");
        std::array<std::uint8_t,59> unchanged{};
        {std::ifstream f(tmp.p/"SOUND.CFG",std::ios::binary);
         f.read(reinterpret_cast<char*>(unchanged.data()),unchanged.size());
         need(f.gcount()==59,"host file length changed");}
        need(source==unchanged,"DOS read-only provider modified source file");
        std::cout<<"HOST_DOS_RO_SERVICE=AH3D_AH3F_AH3E\n"
                   "HOST_DOS_READ_ONLY=YES\n"
                   "HOST_DOS_INPUT_UNMODIFIED=YES\n"
                   "SOURCE_GAME_BOOT=NO\n"
                   "ORIGINAL_DOS_PARITY=NO\n";
        return 0;
    }catch(const std::exception& ex) {
        std::cerr<<"DOS_RO_FILE_PROVIDER_FAIL: "<<ex.what()<<'\n';
        return 1;
    }
}
