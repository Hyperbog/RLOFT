// The original 1C77C modal conversation requests text through 1C368.
// Verify its exact source file-index, DOS seek/read ABI and XOR-DC
// string decoding; no synthetic interactive dialogue response is given.
#include "../../Code2/Code2.cpp"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
using code2::u32;
code2::Machine* active=nullptr;
std::vector<code2::u8> indexed_file{};
std::size_t file_pos=0u;
unsigned seek_calls=0u,read_calls=0u;
void demand(bool ok,const char* msg) {
    if(!ok)throw std::runtime_error(msg);
}
void seek(u32 pos,u32 origin,u32 handle) noexcept {
    if(origin!=0u || handle!=7u || pos>indexed_file.size())
        std::terminate();
    file_pos=pos;
    ++seek_calls;
}
void read_local(void* dest,u32 count,u32 handle) noexcept {
    if(handle!=7u || count>indexed_file.size()-file_pos)
        std::terminate();
    std::copy_n(indexed_file.data()+file_pos,count,
                static_cast<code2::u8*>(dest));
    file_pos+=count;
    ++read_calls;
}
void read_guest(u32 ptr,u32 count,u32 handle) noexcept {
    if(handle!=7u || std::uint64_t(ptr)+count>0x300000u ||
       count>indexed_file.size()-file_pos)
        std::terminate();
    std::copy_n(indexed_file.data()+file_pos,count,active->data+ptr);
    file_pos+=count;
    ++read_calls;
}
void write_index(u32 slot,u32 offset,const std::string& msg) {
    code2::store_le32(indexed_file.data()+4u+slot*4u,offset);
    code2::store_le16(indexed_file.data()+offset,
                      static_cast<code2::u16>(msg.size()));
    for(std::size_t i=0;i<msg.size();++i)
        indexed_file[offset+2u+i]=static_cast<code2::u8>(
            static_cast<unsigned char>(msg[i])^0xDCu);
}
}
int main() {
    try {
        std::vector<code2::u8> data(0x300000u,0u);
        code2::Machine machine{};
        machine.data=data.data();
        active=&machine;
        indexed_file.assign(128u,0u);
        write_index(0u,0x20u,"RAVENLOFT");
        write_index(1u,0x40u,"ORIGINAL CODE2");
        code2::store_le32(data.data()+0x22224u,7u);
        code2::Application39Boundary reader{};
        reader.call_3131c=&seek;
        reader.call_312ec_local=&read_local;
        reader.call_312ec_dgroup=&read_guest;

        code2::function_1c368(machine,reader,0x759Cu,0u);
        demand(std::string(reinterpret_cast<const char*>(data.data()+0x759Cu))==
                  "RAVENLOFT",
               "original 1C368 index-zero XOR decoded text changed");
        code2::function_1c368(machine,reader,0x7700u,1u);
        demand(std::string(reinterpret_cast<const char*>(data.data()+0x7700u))==
                  "ORIGINAL CODE2",
               "original 1C368 second text index decoded incorrectly");
        demand(data[0x759Cu+9u]==0u && data[0x7700u+14u]==0u,
               "source 1C368 string NUL terminator lost");
        demand(seek_calls==4u && read_calls==6u,
               "original dialogue file seek/read callback count changed");
        std::cout<<"ORIGINAL_CODE2_1C368_DIALOGUE_TEXT_SOURCE_PASS"
                 <<" distinct_indices=2 source_seek_calls="<<seek_calls
                 <<" source_read_calls="<<read_calls
                 <<" original_XOR_DC=YES"
                 <<" modal_1C77C=NOT_CONNECTED"
                 <<" DOS_PARITY=NOT_CERTIFIED\n";
        return 0;
    }catch(const std::exception& e) {
        std::cerr<<"FAIL "<<e.what()<<"\n";return 1;
    }
}
