#!/usr/bin/env python3
"""No-original-media regression for the optional DOSBox-X 2026.10.01 hook.

Compiles the GPL header with a real C++11 compiler; every "original"
port event and SAV here is SYNTHETIC_TEST_ONLY, not DOS provenance.
"""
import argparse
import json
import os
import subprocess
import sys
import tempfile
import zipfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(HERE.parent))
import convert_capture as conversion
import verify_dosboxx_palette_port_trace as verifier


def failed(call, pattern):
    try:
        call()
    except ValueError as e:
        assert pattern in str(e), (str(e), pattern)
    else:
        raise AssertionError("Invalid trace accepted: " + pattern)


def test(compiler, code2=None):
    with tempfile.TemporaryDirectory(prefix="rloft-dac-capture-synthetic-") as temp:
        root = Path(temp)
        cpp = root/"emit.cpp"
        binary = root/"emit"
        raw = root/"synthetic-hook.jsonl"
        # No DOSBox-X runtime code or game bytes in synthetic harness.
        cpp.write_text(
            '#include "rloft_dac_port_capture.hpp"\n'
            'int main() {\n'
            '  rloft_dac_capture::record(0x3c8,0,0x88,0x3000);\n'
            '  for (unsigned i=0;i<768;++i)\n'
            '    rloft_dac_capture::record(0x3c9,(i*17)%64,0x88,0x3000);\n'
            '  return 0;\n'
            '}\n', encoding='utf-8')
        subprocess.run([compiler,'-std=c++11','-Wall','-Wextra','-Werror',
                        '-I',str(HERE),str(cpp),'-o',str(binary)],
                       check=True,capture_output=True,text=True,timeout=40)
        env = os.environ.copy()
        env['RLOFT_DAC_CAPTURE_JSONL'] = str(raw)
        subprocess.run([str(binary)], env=env, check=True,timeout=30)
        events, provenance = conversion.normalize(raw.read_bytes())
        assert len(events)==769
        assert events[0]=={'seq':0,'port':0x3C8,'value':0}
        assert events[1]=={'seq':1,'port':0x3C9,'value':0}
        assert provenance['raw_event_count']==769
        palette,stats = verifier.replay(events)
        assert stats['distinct_RGB6_channels_written']==768
        assert palette==bytes((i*17)%64 for i in range(768))
        header=raw.read_bytes().splitlines()[0]
        failed(lambda: conversion.normalize(header+b'\n'+b'{"overflow":true}\n'),
               'overflow')
        failed(lambda: conversion.normalize(raw.read_bytes().replace(
            b'dosboxx-v2026.10.01-dac-v1',b'unknown-hook-version')),
            'Wrong upstream')
        failed(lambda: verifier.replay(events[:-1]),'Incomplete palette')
        failed(lambda: conversion.normalize(raw.read_bytes().replace(
            b'"seq":1,',b'"seq":7,',1)),'sequence')
        # Guard that an unset environment never writes output.
        raw.unlink()
        env.pop('RLOFT_DAC_CAPTURE_JSONL',None)
        subprocess.run([str(binary)], env=env, check=True,timeout=30)
        assert not raw.exists()
        env['RLOFT_DAC_CAPTURE_JSONL']=str(raw)
        subprocess.run([str(binary)], env=env, check=True,timeout=30)

        sav = root/"SYNTHETIC_NOT_RETAIL.sav"
        vga = bytearray(verifier.VGA_SIZE)
        vga[verifier.DAC_POS]=6
        vga[verifier.DAC_POS+1]=255
        vga[verifier.PALETTE_POS:verifier.PALETTE_POS+768] = palette
        with zipfile.ZipFile(sav,'w') as zipped:
            zipped.writestr('DOSBox-X_Version',verifier.DOSBOX_VERSION)
            zipped.writestr('Machine_Type',b'MCH_VGA')
            zipped.writestr('Program_Name',b'CODE')
            zipped.writestr('Vga',vga)
        # Test the converter end-to-end only with authenticated user-owned
        # CODE.2 when provided; never pretend the mock SAV came from DOSBox-X.
        if code2:
            trace,receipt = conversion.convert(
                raw.read_bytes(),sav,Path(code2).read_bytes(),'SYNTHETIC_TEST_ONLY')
            assert len(trace['events'])==769
            assert receipt['replay_result']['status'].endswith('UNATTESTED')
            failed(lambda: conversion.convert(raw.read_bytes(),sav,b'fake','test'),
                   'CODE.2 SHA256 mismatch')
        print('SYNTHETIC_DOSBOXX_DAC_CAPTURE_TEST_PASS '
              'CXX11=PASS raw_JSONL=PASS RGB6=768 '
              'invalid_trace_rejections=PASS DAC_RUNTIME_PROVENANCE=NOT_VERIFIED')


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--compiler',default='c++')
    p.add_argument('--code2',type=Path)
    a=p.parse_args()
    test(a.compiler,a.code2)


if __name__=='__main__':
    main()
