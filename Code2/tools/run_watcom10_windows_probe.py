#!/usr/bin/env python3
"""Run pinned Watcom C 10.0a on Windows only; inventory genuine OMF .OBJ.

Does not run on Linux, upload code, link programs, or claim exact matches.
Output is a PRIVATE folder of source-only diagnostic .OBJ plus report.json.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys

from inspect_watcom_omf import OMFError, parse_omf

BINARIES = {
    'binnt/wcc386.exe': '8fee7d6ab3f2c06a0a874135e3898386898679e7dd53993fcc508e296364767d',
    'binb/wcc386.exe': 'c3666de94f6fa6800f452dae8acf45505ecdb62f0ade2cc27cc86c2d9e8e2b6b',
}
PROBE = Path(__file__).parent / 'probes' / 'watcom10_probe_candidates.c'
# Historical Watcom uses -3s for 386 stack arguments; -3r for registers.
# The five options here are toolchain hypotheses, not validated game flags.
VARIANTS = {
    '386_stack_noopt': ('-bt=dos', '-3s', '-od', '-s', '-zq'),
    '386_stack_time': ('-bt=dos', '-3s', '-ot', '-s', '-zq'),
    '386_register_time': ('-bt=dos', '-3r', '-ot', '-s', '-zq'),
    '486_stack_time': ('-bt=dos', '-4s', '-ot', '-s', '-zq'),
}


def sha(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def verify_toolchain(watcom: Path) -> dict:
    verified = {}
    for path, expected in BINARIES.items():
        candidate = watcom.joinpath(*path.split('/'))
        if not candidate.is_file():
            raise ValueError(f'missing Watcom archive member {path}')
        actual = sha(candidate)
        if actual != expected:
            raise ValueError(f'Watcom archive compiler binary mismatch {path}')
        verified[path] = actual
    return verified


def run_probe(watcom: Path, output: Path, *, windows: bool | None = None,
              executor=None) -> dict:
    if windows is None:
        windows = sys.platform == 'win32'
    if not windows:
        raise RuntimeError('historical Watcom Windows launcher requires a real Windows host')
    fingerprints = verify_toolchain(watcom)
    if output.exists():
        raise FileExistsError('refusing to overwrite Watcom experiment output')
    if not PROBE.is_file():
        raise ValueError('missing source-only Watcom C hypotheses')
    source = PROBE.read_bytes()
    output.mkdir(parents=True, exist_ok=False)
    source_file = output / 'watcom_candidates.c'
    source_file.write_bytes(source)
    compiler = watcom / 'binnt' / 'wcc386.exe'
    environ = os.environ.copy()
    environ['WATCOM'] = str(watcom)
    environ['INCLUDE'] = str(watcom / 'h')
    environ['WCC386'] = ''  # prevent workstation-specific flag injection
    environ['PATH'] = str(watcom / 'binnt') + os.pathsep + str(watcom / 'binb') + os.pathsep + environ.get('PATH', '')
    run = executor if executor is not None else subprocess.run
    outcomes = []
    for label, flags in VARIANTS.items():
        dest = output / label
        dest.mkdir()
        objname = 'watcom_candidates.obj'
        args = [str(compiler), *flags, f'-fo={objname}', str(source_file)]
        try:
            result = run(args, cwd=dest, env=environ,
                         capture_output=True, text=True, timeout=90, check=False)
            status = result.returncode
            err = (result.stdout + '\n' + result.stderr)[-2500:]
        except (OSError, subprocess.TimeoutExpired) as exc:
            status = None
            err = str(exc)[-1500:]
        obj = dest / objname
        item = {'profile': label, 'options': list(flags),
                'exit_code': status, 'object_present': obj.is_file(),
                'object_sha256': sha(obj) if obj.is_file() else None,
                'omf_inventory_success': False, 'diagnostic_tail': err,
                'compiler_match': 'not_tested', 'original_le_fixups': 'not_tested'}
        if obj.is_file():
            try:
                inventory = parse_omf(obj.read_bytes())
                (dest / 'omf_inventory.json').write_text(
                    json.dumps(inventory, indent=2, sort_keys=True) + '\n', encoding='utf-8')
                item['omf_inventory_success'] = True
                item['omf_segments'] = len(inventory['segments'])
                item['omf_fixups'] = len(inventory['fixups'])
                item['unparsed_features'] = inventory['unparsed_code_features']
            except (OMFError, ValueError) as exc:
                item['omf_inventory_error'] = str(exc)
        outcomes.append(item)
    report = {
        'schema': 'rloft-watcom10-first-real-omf-probe-v1',
        'compiler': 'Watcom C 10.0a',
        'verified_compiler_executable_sha256': fingerprints,
        'source_sha256': hashlib.sha256(source).hexdigest(),
        'profiles': outcomes,
        'original_game_binary_embedded': False,
        'compiler_identified_for_ravenloft': False,
        'dos_runtime_parity': 'not_tested',
    }
    (output / 'report.json').write_text(
        json.dumps(report, indent=2, sort_keys=True) + '\n', encoding='utf-8')
    return report


def main() -> None:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--watcom-root', type=Path, required=True,
                   help='Previously verified and extracted Watcom archive root')
    p.add_argument('--output', type=Path, required=True,
                   help='New private directory for generated OMF objects')
    args = p.parse_args()
    result = run_probe(args.watcom_root, args.output)
    print(f"Watcom pilot: {sum(x['object_present'] for x in result['profiles'])} / {len(VARIANTS)} objects generated")
    print(f"Private report: {args.output / 'report.json'}")


if __name__ == '__main__':
    main()
