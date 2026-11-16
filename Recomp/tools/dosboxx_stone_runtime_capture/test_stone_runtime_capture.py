#!/usr/bin/env python3
"""Compile real hook with a bounded fake guest and verify trace semantics.

Synthetic test only; never mark original Stone gameplay executed.
"""
import os
import subprocess
import tempfile
from pathlib import Path
from summarize_stone_runtime_trace import verify

HEADER=Path(__file__).with_name("rloft_stone_runtime_capture.hpp")
SHIM=r'''
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
static std::vector<unsigned char> ram(0x800000,0);
static unsigned char mem_readb_inline(std::uint32_t p) {
    if(p>=ram.size()) std::abort();
    return ram[p];
}
static std::uint16_t mem_readw_inline(std::uint32_t p) {
    return mem_readb_inline(p) | (mem_readb_inline(p+1)<<8);
}
static std::uint32_t mem_readd_inline(std::uint32_t p) {
    return mem_readw_inline(p) | ((std::uint32_t)mem_readw_inline(p+2)<<16);
}
#include "rloft_stone_runtime_capture.hpp"
int main() {
    const std::uint32_t csbase=0x10000, dsbase=0x200000;
    const std::uint32_t ssbase=0x300000, buffer=0x20000;
    const unsigned char signature[]={0x53,0x56,0x57,0x55,0x89,0xe5,
        0x83,0xec,0x14,0x83,0x7d,0x14,0xff};
    for(unsigned i=0;i<sizeof(signature);++i)
        ram[csbase+0x29c+i]=signature[i];
    ram[csbase+0x27ca8]=0xf7;
    ram[csbase+0x27ca9]=0xf9;
    ram[csbase+0x70d]=0xe8;
    ram[dsbase+buffer]=1; // one command in first slot
    ram[dsbase+buffer+1]=13;
    ram[dsbase+buffer+2]=7;
    ram[dsbase+buffer+3]=9;
    ram[dsbase+buffer+4]=24;
    ram[ssbase+0x1004]=24;
    ram[dsbase+0x44ff4]=24;
    auto tick=[&](unsigned pc,unsigned eax=0,unsigned esi=0,
                  unsigned ebx=0,unsigned edi=0,unsigned ecx=1) {
        rloft_stone_capture::tick(0x77,csbase,dsbase,ssbase,pc,eax,
                                   esi,ebx,edi,ecx,0,0x1000);
    };
    tick(0x2ba,buffer);
    tick(0x70d,0,buffer);
    tick(0x10);
    tick(0x23af1,0,0,0,0);
    ram[dsbase+buffer+4]=4;
    tick(0x1e1e4);
    tick(0x27ca8,0,0x44ff0,0,0,0);
    return 0;
}
'''
def main():
    with tempfile.TemporaryDirectory(prefix="rloft-stone-hook-test-") as temp:
        root=Path(temp)
        source=root/"test.cpp"
        binary=root/"test"
        trace=root/"test-private.jsonl"
        source.write_text(SHIM,encoding="utf-8")
        command=["g++","-std=c++11","-Wall","-Wextra","-pedantic","-O2",
                 "-I",str(HEADER.parent),str(source),"-o",str(binary)]
        subprocess.run(command,check=True,capture_output=True,text=True)
        env=os.environ.copy()
        env["RLOFT_STONE_ORIGINAL_TRACE_JSONL"]=str(trace)
        env["RLOFT_STONE_TRACE_EVERY_INSTRUCTION"]="1"
        subprocess.run([str(binary)],env=env,check=True,timeout=10)
        report=verify(trace)
        counts=report["event_kinds"]
        if counts.get("event_buffer_changed")!=1:
            raise AssertionError(f"Buffer delta not captured: {counts}")
        if counts.get("constructor_entry")!=1 or \
           report["affected_event_spawn_dispatch_count"]!={24:1}:
            raise AssertionError(f"Original constructor instrumentation failed: {report}")
        if not report["idiv_zero_pre_execution_observed"]:
            raise AssertionError("IDIV zero pre-execution observation missing")
        if report["original_divide_exception_or_crash_observed"]:
            raise AssertionError("Synthetic observation misclassified as runtime crash")
        print("STONE_TRACE_CXX11_SHIM_PASS; original DOS game NOT RUN")
if __name__=="__main__":main()
