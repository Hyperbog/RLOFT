#!/usr/bin/env python3
"""CODE.2 07B94 native title-Load readiness guard + optional original-byte oracle.

Public default reads source files only; --original verifies user-owned CODE.2.
Do not mistake a valid save-slot pathname for implemented game restoration.
"""
from __future__ import annotations
import argparse
import hashlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ORIGINAL_SHA = '853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314'
SELECTOR_SHA = 'd5fdc0c86cfdf304db4927fc0cee394d2354b1284608ba2c044ad228e698ff9f'
# Original logical offsets within CODE.2 code object 1 (file base 0x3AF24).
CALLS = {
    0x7B9F: 0x071B8, # list/render backing region
    0x7BA7: 0x1A1F4, # poll until mouse released
    0x7BB0: 0x1A1F4, # poll for fresh press
    0x7C99: 0x33A5A, # DOS chdir WORK
    0x7CA9: 0x07378, # delete files in WORK
    0x7CB3: 0x33A5A, # DOS chdir ..
    0x7CD7: 0x073A0, # wildcard copy SAVE## -> WORK
    0x7CEB: 0x07938, # common WORK/SAVE.DAT loader
}
# These windows pin original geometry, decimal-digit stores, copy result,
# and the original failure retry/exit branches, not a guessed native UI.
WINDOWS = {
    0x7B94: '53 56 a1 dc 58 02 00',
    0x7BB9: '66 8b 15 35 9e 00 00 30 f6 80 e2 02',
    0x7BE2: '83 fb 15 7c c0',       # X lower bound 21
    0x7BE7: '81 fb 7b 01 00 00',  # X upper bound 379 (exclusive)
    0x7BEF: '83 f8 14 7c b3',     # Y lower bound 20
    0x7BF4: '83 f8 5c 7d ae',     # Y upper bound 92 (exclusive)
    0x7BF9: '83 eb 15 be 25 00 00 00', # X -=21; 37 px cell
    0x7C0A: '83 fa 19 7f 98',     # remainder >25 rejects
    0x7C70: '83 c0 30 89 da a2 90 66 00 00', # tens character
    0x7C8C: 'c6 05 92 66 00 00 00', # temporary slash NUL
    0x7C93: '88 15 91 66 00 00', # ones character
    0x7CCD: '68 20 c0 00 00 68 8c 66 00 00', # WORK, SAVE?? args
    0x7CDC: 'c6 05 92 66 00 00 5c', # slash restored
    0x7CE6: '83 f8 01 74 24', # copy failed -> branch
}
# Original jumps matter: the failed chdir WORK returns WITHOUT
# 07CDC restoring 6692 back to '\\'. Copy failure restores the slash,
# shows original error and retries. The ten-handle cleanup is distinct.
BRANCHES = {
    0x7BD8: ('short', 0x74, 0x7BE2),
    0x7CA3: ('near-conditional', 0x85, 0x7D1F),
    0x7CBD: ('short', 0x74, 0x7CCD),
    0x7CE9: ('short', 0x74, 0x7D0F),
    0x7D08: ('short', 0x75, 0x7CF4),
    0x7D1A: ('near-jump', 0xE9, 0x7BA4),
    0x7D3C: ('short', 0x75, 0x7D28),
}
DATA_STRINGS = {
    0x668C: b'SAVE??\\SAVE.DAT\x00',
    0xC020: b'WORK\x00',
    0x0398: b'\\\x00',
    0x039C: b'SAVE.DAT\x00',
    0x03B0: b'???????????\x00',
    0x03BC: b'\\*.*\x00',
    0x03C4: b'..\x00',
}

def original_oracle(data: bytes) -> None:
    if len(data) != 558689 or hashlib.sha256(data).hexdigest() != ORIGINAL_SHA:
        raise ValueError('Original CODE.2 version/hash mismatch')
    base = 0x3AF24
    span = data[base + 0x7B94:base + 0x7D43]
    if hashlib.sha256(span).hexdigest() != SELECTOR_SHA:
        raise ValueError('CODE.2 07B94 original load selector changed')
    for address, hex_bytes in WINDOWS.items():
        want = bytes.fromhex(hex_bytes)
        if data[base + address:base + address + len(want)] != want:
            raise ValueError(f'CODE.2 load selector window {address:05X} differs')
    for at, target in CALLS.items():
        at += base
        if data[at] != 0xE8:
            raise ValueError(f'Original expected CALL not present at {at-base:05X}')
        displacement = int.from_bytes(data[at+1:at+5], 'little', signed=True)
        if at - base + 5 + displacement != target:
            raise ValueError(f'Original load CALL target differs at {at-base:05X}')
    for logical, (kind, opcode, target) in BRANCHES.items():
        at = base + logical
        if kind == 'short':
            if data[at] != opcode:
                raise ValueError(f'CODE.2 short branch opcode {logical:05X}')
            delta = int.from_bytes(data[at+1:at+2], 'little', signed=True)
            actual = logical + 2 + delta
        elif kind == 'near-conditional':
            if data[at:at+2] != bytes((0x0F,opcode)):
                raise ValueError(f'CODE.2 near conditional {logical:05X}')
            delta = int.from_bytes(data[at+2:at+6], 'little', signed=True)
            actual = logical + 6 + delta
        elif kind == 'near-jump':
            if data[at] != opcode:
                raise ValueError(f'CODE.2 near jump {logical:05X}')
            delta = int.from_bytes(data[at+1:at+5], 'little', signed=True)
            actual = logical + 5 + delta
        else:
            raise ValueError(f'Unknown oracle branch kind {kind}')
        if actual != target:
            raise ValueError(
                f'CODE.2 selector branch {logical:05X} -> {actual:05X}, '
                f'expected {target:05X}')
    # Both exit behaviors rely on the original source ORDER:
    # 07C8C sets the mutable SAVE?? template slash to NUL;
    # failed chdir branches to 07D1F, skipping 07CDC backslash reset.
    if not (0x7C8C < 0x7CA3 < 0x7CDC < 0x7D1F):
        raise ValueError('Original failure/slash mutation order differs')
    for offset, want in DATA_STRINGS.items():
        at = 0x75F24 + offset
        if data[at:at+len(want)] != want:
            raise ValueError(f'Original load DGROUP string changed at {offset:04X}')


def slot_from_click(x: int, y: int, right_button: bool) -> int | None:
    """Pure, source-grounded selector geometry, not a native UI handler."""
    if right_button or not (21 <= x < 379 and 20 <= y < 92):
        return None
    index, remainder = divmod(x - 21, 37)
    return index + 1 if remainder <= 25 else None


def self_test() -> None:
    for i in range(10):
        x = 21 + 37*i
        assert slot_from_click(x, 20, False) == i+1
        assert slot_from_click(min(x+25,378), 91, False) == i+1
        assert slot_from_click(x+26, 20, False) is None
        assert slot_from_click(x, 20, True) is None
    for x,y in [(20,20),(379,20),(21,19),(21,92),(0,0)]:
        assert slot_from_click(x,y,False) is None
    assert tuple(f'SAVE{i:02d}' for i in range(1,11))[-1] == 'SAVE10'


def source_guard(text: str) -> None:
    load = text.index('if (*click == 1) {')
    creation = text.index('if (*click == rloft::BootFlow::kCharacterWorkflowMenuIndex)', load)
    block = text[load:creation]
    if ('Load Game is not yet available' not in block or 'MessageBoxW' not in block
            or 'return run_exploration(' in block):
        raise ValueError('Unaccepted native LOAD cannot claim restored gameplay')


def native_source_guard(header: str, tests: str, cmake: str) -> None:
    """Keep the byte-proved C++ model separate from unfinished menu IO."""
    required = (
        'std::int16_t mouse_x, std::int16_t mouse_y',
        '(mouse_buttons & 0x0002U) != 0U',
        'OriginalTitleLoadHitKind::RightClickExit, 0',
        'x < 21 || x >= 379 || y < 20 || y >= 92',
        'offset=x-21',
        'offset % 37 > 25',
        'const auto slot=(offset/37)+1',
        'OriginalTitleLoadHitKind::SelectedSlot',
        'static_cast<std::uint8_t>(slot)',
    )
    for token in required:
        if token not in header:
            raise ValueError('Original Load selector native mapping changed: '+token)
    if ('#undef NDEBUG' not in tests or
            'original_title_load_hit(379,91,0)' not in tests or
            'original_title_load_hit(21,20,2)' not in tests or
            'for (int column=0;column<10;++column)' not in tests or
            'for (int dx=0;dx<37;++dx)' not in tests):
        raise ValueError('Exhaustive Release slot and right-click assertions absent')
    if ('add_executable(original_title_load_slot_tests' not in cmake or
            'add_test(NAME original_title_load_slot_tests COMMAND original_title_load_slot_tests)' not in cmake):
        raise ValueError('Native selector test omitted from CI')


def main() -> int:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--root',type=Path,default=ROOT)
    p.add_argument('--original',type=Path)
    p.add_argument('--original-only',action='store_true',
                   help='Run private original oracle without repository source checkout')
    args = p.parse_args()
    try:
        self_test()
        if not args.original_only:
            text=(args.root/'DemoV1/src/win32_main.cpp').read_text(encoding='utf-8')
            source_guard(text)
            header=(args.root/'DemoV1/include/rloft/original_title_load_slot.hpp').read_text(encoding='utf-8')
            tests=(args.root/'DemoV1/tests/original_title_load_slot_tests.cpp').read_text(encoding='utf-8')
            cmake=(args.root/'DemoV1/CMakeLists.txt').read_text(encoding='utf-8')
            native_source_guard(header,tests,cmake)
            # Four regression mutations corrupt independent DOS facts.
            mutations = (
                ('x >= 379', 'x >= 380'),
                ('offset % 37 > 25', 'offset % 37 > 26'),
                ('(mouse_buttons & 0x0002U)', '(mouse_buttons & 0x0001U)'),
                ('const auto slot=(offset/37)+1', 'const auto slot=(offset/37)'),
            )
            for before, after in mutations:
                if header.count(before) != 1:
                    raise ValueError('Unable to mutate native selector: '+before)
                try:
                    native_source_guard(header.replace(before,after,1),tests,cmake)
                except ValueError:
                    pass
                else:
                    raise ValueError('Unfaithful selector mutation escaped: '+before)
            bad=text.replace('Load Game is not yet available','Load Game successfully loaded',1)
            try:
                source_guard(bad)
            except ValueError:
                pass
            else:
                raise ValueError('Invalid false-LOAD success mutation passed')
        if args.original:
            original_oracle(args.original.read_bytes())
        elif args.original_only:
            raise ValueError('--original-only requires --original')
    except (ValueError,OSError,AssertionError) as exc:
        p.exit(1,'TITLE_LOAD_SOURCE_REJECTED: '+str(exc)+'\n')
    print('TITLE_LOAD_SLOT_SOURCE_PASS'+(' ORIGINAL_CODE2_PASS' if args.original else ''))
    return 0

if __name__=='__main__':
    raise SystemExit(main())
