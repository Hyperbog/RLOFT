#include "original_vga_host.hpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#define CHECK(x) do { if (!(x)) throw std::runtime_error("VGA regression: " #x); } while(0)

using namespace rloft::graphics;
int main() {
    static_assert(kWidth==320 && kHeight==400 && kBytesPerScanlinePerPlane==80);
    static_assert(code2_planar_address(0,0).offset==0);
    static_assert(code2_planar_address(319,0).offset==79);
    static_assert(code2_planar_address(319,0).plane==3);
    static_assert(code2_planar_address(0,399).offset==31920);
    static_assert(code2_planar_address(319,399).offset==31999);
    static_assert(code2_planar_address(0,400,32768).offset==64768);
    static_assert(code2_planar_address(0,420,32768).offset==66368);
    static_assert(code2_planar_address(12,5,0xA0000).offset==0xA0193);
    static_assert(dac6_to_byte(0)==0 && dac6_to_byte(63)==255);
    OriginalVgaHost v;
    // Original 0x29AA4: sequencer 0x3C4 reg2 selects write plane.
    CHECK(v.write_indexed_port(0x3C4,0x0102));
    CHECK(v.map_mask()==1);
    v.write_aperture(10,12);
    CHECK(v.write_indexed_port(0x3C4,0x0202));
    v.write_aperture(10,34);
    CHECK(v.write_indexed_port(0x3CE,0x0004));
    CHECK(v.read_aperture(10)==12);
    CHECK(v.write_indexed_port(0x3CE,0x0104));
    CHECK(v.read_aperture(10)==34);
    CHECK(v.write_indexed_port(0x3C4,0x0F02));
    v.write_aperture(11,77);
    for (int i=0;i<4;++i) {
        CHECK(v.write_indexed_port(0x3CE,static_cast<std::uint16_t>((i<<8)|4)));
        CHECK(v.read_aperture(11)==77);
    }
    // Original 0x29CA0: DAC indexed data, each three bytes is RGB6.
    CHECK(v.write_port8(0x3C8,10));
    CHECK(v.write_port8(0x3C9,63));
    CHECK(v.write_port8(0x3C9,32));
    CHECK(v.write_port8(0x3C9,1));
    CHECK(v.palette(10).r==63 && v.palette(10).g==32 && v.palette(10).b==1);
    for(int i=0;i<3;++i) CHECK(v.write_port8(0x3C9,255));
    CHECK(v.palette(11).r==63 && v.palette(11).g==63 && v.palette(11).b==63);
    CHECK(v.write_direct_pixel(0,0,10));
    CHECK(v.write_direct_pixel(1,0,11));
    CHECK(v.write_direct_pixel(319,399,10));
    CHECK(!v.write_direct_pixel(320,0,10));
    CHECK(!v.write_direct_pixel(0,400,10));
    CHECK(v.indexed_scanout(0,0)==10);
    CHECK(v.indexed_scanout(1,0)==11);
    CHECK(v.indexed_scanout(319,399)==10);
    const auto c=v.rgb_at(0,0);
    CHECK(c.r==255 && c.g==130 && c.b==4 && c.a==255);
    // Original 0x2ABF4 programs CRTC 0C/0D display start address.
    CHECK(v.write_indexed_port(0x3D4,0x010C));
    CHECK(v.write_indexed_port(0x3D4,0x000D));
    CHECK(v.crtc_start()==256);
    CHECK(v.indexed_scanout(0,0)==0);
    CHECK(v.write_indexed_port(0x3D4,0x0009)); // CODE.2 400-row scan mode
    CHECK(!v.write_port8(0x3DA,8)); // no synthetic retrace
    CHECK(v.write_port8(0x3CE,7)); // unsupported GC register index
    CHECK(!v.write_port8(0x3CF,0)); // GC miscellaneous register not yet emulated
    // CODE.2 2AC10: CRTC 18 low eight compare bits, 07/09 high bits.
    // The scanout below is 400 physical rows even in repeat-2 mode.
    CHECK(v.write_indexed_port(0x3D4,0x0007));
    CHECK(v.write_indexed_port(0x3D4,0x0109)); // repeat each source row twice
    CHECK(v.repeat_scanlines()==2);
    CHECK(v.write_indexed_port(0x3D4,0x0318)); // split after physical row 3
    CHECK(v.line_compare()==3);
    CHECK(v.indexed_scanout(0,0)==0); // CRTC starts at offset 256
    CHECK(v.indexed_scanout(0,4)==10); // post split resets start to 0
    CHECK(v.indexed_scanout(1,5)==11); // repeated source row
    CHECK(v.read_port8(0x3D5)==3);
    CHECK(v.read_port8(0x3DA)==0x08);
    CHECK(v.read_port8(0x3DA)==0x00);
    CHECK(v.retrace_reads()==2);
    CHECK(!v.unsupported_read());
    const auto pixels=v.scanout_indices();
    CHECK(pixels.size()==320u*400u);
    // Mode 0 read-modify-write: original source draws read to load latches.
    CHECK(v.write_indexed_port(0x3C4,0x0F02));
    CHECK(v.write_indexed_port(0x3CE,0x0003));
    CHECK(v.write_indexed_port(0x3CE,0x0005));
    CHECK(v.write_indexed_port(0x3CE,0xFF08));
    CHECK(v.write_aperture(100,0x0F));
    CHECK(v.read_aperture(100)==0x0F);
    CHECK(v.write_indexed_port(0x3CE,0x1803)); // XOR each latched plane
    CHECK(v.write_aperture(100,0xFF));
    CHECK(v.read_aperture(100)==0xF0);
    CHECK(v.write_indexed_port(0x3CE,0x0003));
    CHECK(v.write_indexed_port(0x3CE,0x0F08)); // preserve upper latch bits
    CHECK(v.write_aperture(100,0x05));
    CHECK(v.read_aperture(100)==0xF5);
    CHECK(v.write_indexed_port(0x3CE,0xFF08));
    CHECK(v.write_indexed_port(0x3CE,0x0105)); // mode 1 copies latch
    CHECK(v.write_aperture(101,0));
    CHECK(v.read_aperture(101)==0xF5);
    CHECK(v.write_indexed_port(0x3CE,0x0205)); // mode 2 color expands bits
    CHECK(v.write_aperture(102,0x05));
    for (int p=0;p<4;++p) {
        CHECK(v.write_indexed_port(0x3CE,static_cast<std::uint16_t>((p<<8)|4)));
        CHECK(v.read_aperture(102)==((p&1)?0u:0xFFu));
    }
    CHECK(v.write_indexed_port(0x3CE,0x0005));
    CHECK(v.write_aperture(103,0x0F));
    CHECK(v.read_aperture(103)==0x0F);
    CHECK(v.write_indexed_port(0x3CE,0x0500)); // set/reset planes 0 and 2
    CHECK(v.write_indexed_port(0x3CE,0xF008)); // upper-nibble bit mask
    CHECK(v.write_indexed_port(0x3CE,0x0305)); // mode 3 masked color
    CHECK(v.write_aperture(103,0xF0));
    for (int p=0;p<4;++p) {
        CHECK(v.write_indexed_port(0x3CE,static_cast<std::uint16_t>((p<<8)|4)));
        CHECK(v.read_aperture(103)==((p&1)?0x0Fu:0xFFu));
    }
    CHECK(!v.write_indexed_port(0x3CE,0x0805)); // read mode1 unsupported
    CHECK(v.write_indexed_port(0x3CE,0x0005));
    CHECK(v.write_indexed_port(0x3CE,0xFF08));
    CHECK(v.write_indexed_port(0x3CE,0x0103)); // CPU data rotate right 1
    CHECK(v.write_aperture(104,0x80));
    CHECK(v.read_aperture(104)==0x40);
    std::cout<<"CODE2_VGA_ADDRESS_PLANES_DAC6_CRTC_LATCH_ROP_PASS\n";
}
