#!/usr/bin/env python3
"""Compile genuine patched DAC handler bodies with a bounded VGA shim.

This tests patched original DOSBox-X 2026.10.01 function bodies, not
the full emulator, and does not claim real original DOS game execution.
"""
import argparse
import os
import subprocess
import tempfile
from pathlib import Path

import convert_capture as conversion


def span(source, start, next_symbol):
    a = source.find(start)
    b = source.find(next_symbol, a)
    if a < 0 or b <= a:
        raise ValueError("Missing patched original DAC handler boundary")
    return source[a:b]


def test(patched_cpp, compiler):
    original = Path(patched_cpp).read_text(encoding="utf-8")
    h8 = span(original, "void write_p3c8(Bitu port,Bitu val,Bitu iolen)",
              "Bitu read_p3c8(")
    h9 = span(original, "void write_p3c9(Bitu port,Bitu val,Bitu iolen)",
              "Bitu read_p3c9(")
    if h8.count("rloft_dac_capture::record(") != 1 or \
       h9.count("rloft_dac_capture::record(") != 1:
        raise ValueError("Patched original DAC handler calls missing")
    prefix = r'''
#include <cstdint>
#include "rloft_dac_port_capture.hpp"
using Bitu=unsigned long;
enum {DAC_READ, DAC_WRITE, MCH_MCGA=9, SVGA_ParadisePVGA1A=15};
enum {cs=0};
unsigned SegValue(int) {return 0x1234;}
unsigned reg_eip=0x29cb6;
struct DacRgb { uint8_t red=0, green=0, blue=0; };
struct DacState {
  int hidac_counter=0, pel_index=0, state=DAC_READ;
  unsigned write_index=0, read_index=0;
  DacRgb rgb[256];
};
struct { DacState dac; struct {bool vga_dac_lockout=false;} dosboxig;
         struct {int mcga_mode_control=0;} other; } vga;
int svgaCard=0, machine=0;
bool vga_8bit_dac=false, vga_palette_update_on_full_load=true;
bool vga_render_on_demand=false;
unsigned VGA_DAC_DeferredUpdate=0;
static unsigned char tmp_dac[3]={0,0,0};
void VGA_RenderOnDemandUpTo() {}
'''
    suffix = r'''
int main() {
    vga.dosboxig.vga_dac_lockout=true;
    write_p3c8(0x3c8,99,1);
    write_p3c9(0x3c9,99,1);
    vga.dosboxig.vga_dac_lockout=false;
    write_p3c8(0x3c8,246,1);
    write_p3c9(0x3c9,70,1);  // effective RGB6=6
    write_p3c9(0x3c9,2,1);
    write_p3c9(0x3c9,3,1);
    if(vga.dac.rgb[246].red!=6 || vga.dac.rgb[246].green!=2 ||
       vga.dac.rgb[246].blue!=3 || vga.dac.write_index!=247)
      return 3;
    return 0;
}
'''
    with tempfile.TemporaryDirectory(prefix="rloft-authentic-handler-shim-") as td:
        root=Path(td)
        cpp=root/"original_dac_handlers.cpp"
        exe=root/"original_dac_handlers"
        raw=root/"accepted-dac.jsonl"
        cpp.write_text(prefix+h8+"\n"+h9+"\n"+suffix,encoding="utf-8")
        subprocess.run([compiler,"-std=c++11","-Wall","-Wextra",
                        "-Wno-unused-parameter","-I",
                        str(Path(__file__).resolve().parent),str(cpp),
                        "-o",str(exe)],check=True,capture_output=True,text=True,timeout=50)
        env=os.environ.copy()
        env["RLOFT_DAC_CAPTURE_JSONL"]=str(raw)
        subprocess.run([str(exe)],env=env,check=True,timeout=30)
        events,_=conversion.normalize(raw.read_bytes())
        if events!=[{"seq":0,"port":0x3c8,"value":246},
                    {"seq":1,"port":0x3c9,"value":6},
                    {"seq":2,"port":0x3c9,"value":2},
                    {"seq":3,"port":0x3c9,"value":3}]:
            raise AssertionError("Actual patched VGA handler acceptance mismatches hook trace")
    print("ORIGINAL_DOSBOXX_DAC_HANDLERS_SHIM_PASS "
          "lockout_ignored=YES post_mask_value=6 "
          "sequential_accepted_writes=4 FULL_EMULATOR_NOT_TESTED")


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--patched-cpp",type=Path,required=True)
    p.add_argument("--compiler",default="c++")
    args=p.parse_args()
    test(args.patched_cpp,args.compiler)


if __name__=="__main__":
    main()
