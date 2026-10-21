#include "../src/host_dos_disk_space.hpp"
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
namespace dos=rloft::host_dos_disk;
void need(bool x,const char* why){if(!x)throw std::runtime_error(why);}
int main(){
 try {
    const auto small=dos::encode(1u*1024u*1024u*1024u,
                                 256u*1024u*1024u);
    need(small.sectors_per_cluster==64u && small.bytes_per_sector==512u &&
         small.total_clusters==32768u && small.free_clusters==8192u,
         "FAT16 cluster mapping of observed host bytes differs");
    const auto huge=dos::encode(100ull*1024ull*1024ull*1024ull,
                                40ull*1024ull*1024ull*1024ull);
    need(huge.total_clusters==65535u && huge.free_clusters==65535u,
         "Modern volume must saturate without 16-bit register overflow");
    const auto zero=dos::encode(1024ull*1024ull,0ull);
    need(zero.total_clusters==32u && zero.free_clusters==0u,
         "Actual zero free bytes cannot be fabricated as free clusters");
    bool reject=false;
    try{(void)dos::encode(100u,300u);}
    catch(const std::runtime_error&){reject=true;}
    need(reject,"Impossible capacity/free values accepted");
    reject=false;
    try{(void)dos::encode(0u,0u);}
    catch(const std::runtime_error&){reject=true;}
    need(reject,"Empty volume accepted");
    reject=false;
    try{(void)dos::encode(std::numeric_limits<std::uint64_t>::max(),0u);}
    catch(const std::runtime_error&){reject=true;}
    need(reject,"Unknown capacity accepted");
    std::cout<<"HOST_DOS_INT21_AH36_DISK_GEOMETRY_PASS"
             <<" FAT16_16bit_registers=YES"
             <<" observed_available_clusters=YES"
             <<" modern_volume_saturates=YES"
             <<" unsupported_geometry=REJECTED\n";
    return 0;
 }catch(const std::exception& ex){
    std::cerr<<"HOST_DOS_DISK_GEOMETRY_FAIL: "<<ex.what()<<'\n';return 1;
 }
}
