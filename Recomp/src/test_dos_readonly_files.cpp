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
void eq_seek(rloft_recomp::DosSeekResult x,bool carry,
             std::uint32_t result,const char* msg) {
    need(x.carry==carry && (carry?x.ax==result:x.offset()==result),msg);
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
        {std::ofstream f(tmp.p/"BIG.BIN",std::ios::binary);
         need(f.good(),"cannot create large fixture");
         for(unsigned i=0;i<0x10040u;++i)
             f.put(static_cast<char>(i&0xFFu));
         need(f.good(),"cannot write large fixture");}
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

        eq_seek(files.seek(a.ax,0,1),false,59u,"position after EOF");
        eq_seek(files.seek(a.ax,-42,1),false,17u,"signed current seek");
        eq(files.read(a.ax,buf.data(),42),false,42,"re-read after EOF");
        need(std::equal(buf.begin(),buf.begin()+42,source.begin()+17),
             "seek after EOF recovered incorrect bytes");
        eq_seek(files.seek(a.ax,-1,2),false,58u,"end-relative seek");
        eq(files.read(a.ax,buf.data(),3),false,1,"read partial at EOF");
        need(buf[0]==source[58],"last byte after end seek differs");
        eq_seek(files.seek(a.ax,70,0),false,70u,"seek beyond EOF");
        eq(files.read(a.ax,buf.data(),1),false,0,"seek past EOF read");
        eq_seek(files.seek(a.ax,0,0),false,0u,"rewind file");
        eq(files.read(a.ax,buf.data(),1),false,1,"rewind recovered read");
        need(buf[0]==source[0],"rewind byte differs");
        eq_seek(files.seek(a.ax,-1,0),true,1u,"negative absolute position");
        eq_seek(files.seek(a.ax,-1,3),true,1u,"invalid origin");
        eq_seek(files.seek(a.ax,-10000,1),true,1u,"negative current position");
        eq_seek(files.seek(999,0,0),true,6u,"invalid seek handle");
        eq_seek(files.seek(a.ax,0,1),false,1u,"rejected seek altered cursor");
        eq_seek(files.seek(b.ax,0,1),false,59u,"second cursor changed");
        const auto large=files.open("big.bin",0);
        need(!large.carry && large.ax==7u,"large file handle assignment");
        eq_seek(files.seek(large.ax,0x10000,0),false,0x10000u,
                "64KiB offset seek high word");
        eq(files.read(large.ax,buf.data(),8),false,8,"large positioned read");
        for(unsigned i=0;i<8;++i)
            need(buf[i]==static_cast<std::uint8_t>(i),"large file content");
        eq_seek(files.seek(large.ax,-8,1),false,0x10000u,
                "large file current seek");
        eq_seek(files.seek(large.ax,-0x40,2),false,0x10000u,
                "large file end seek");
        eq_seek(files.seek(large.ax,0x7fffffff,0),false,0x7fffffffu,
                "valid large seek rejected");
        eq_seek(files.seek(large.ax,0x7fffffff,1),false,0xFFFFFFFEu,
                "32-bit maximum positioning rejected");
        eq_seek(files.seek(large.ax,2,1),true,1u,
                "guest position wrap accepted");
        eq(files.close(large.ax),false,0,"large file close failed");
        eq_seek(files.seek(large.ax,0,0),true,6u,"closed seek handle");
        eq(files.close(a.ax),false,0,"valid close");
        eq(files.close(a.ax),true,6,"double close accepted");
        eq(files.close(b.ax),false,0,"second close failed");
        need(files.open_handles()==0u,"DOS handle leak");
        std::array<std::uint8_t,59> unchanged{};
        {std::ifstream f(tmp.p/"SOUND.CFG",std::ios::binary);
         f.read(reinterpret_cast<char*>(unchanged.data()),unchanged.size());
         need(f.gcount()==59,"host file length changed");}
        need(source==unchanged,"DOS read-only provider modified source file");
        std::cout<<"HOST_DOS_RO_SERVICE=AH3D_AH3F_AH3E_AH42\n"
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
