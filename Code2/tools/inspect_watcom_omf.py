#!/usr/bin/env python3
"""Fail-closed OMF/OMF32 record, public-symbol and relocation-site inventory.

NO binary equivalence, linker fixup resolution, compiler identification or
Watcom match is claimed. Use with local, independently authenticated .obj files.
"""
from __future__ import annotations

import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import struct

# OMF OMF32 record tags; see OS/2 OMF rev. 8, sections 5.18-5.25.
KNOWN = {0x80, 0x88, 0x8A, 0x8B, 0x8C, 0x90, 0x91, 0x96,
         0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0xA0, 0xA1,
         0xA2, 0xA3, 0xC2, 0xC3}
# Used to check that FIXUPP does not cross the end of its source LEDATA.
LOCATION_BYTES = {0: 1, 1: 2, 2: 2, 3: 4, 4: 1, 5: 2,
                  6: 6, 9: 4, 11: 6, 13: 4}

class OMFError(ValueError):
    pass


class Cursor:
    def __init__(self, data: bytes):
        self.data = data
        self.p = 0

    def take(self, size: int) -> bytes:
        if size < 0 or self.p + size > len(self.data):
            raise OMFError('truncated OMF record field')
        result = self.data[self.p:self.p + size]
        self.p += size
        return result

    def u8(self) -> int:
        return self.take(1)[0]

    def uint(self, width: int) -> int:
        return int.from_bytes(self.take(width), 'little')

    def index(self) -> int:
        x = self.u8()
        return ((x & 0x7F) << 8) | self.u8() if x & 0x80 else x

    def string(self) -> str:
        return self.take(self.u8()).decode('latin-1')

    def done(self) -> None:
        if self.p != len(self.data):
            raise OMFError('unconsumed OMF record fields')


def records(file_bytes: bytes):
    if len(file_bytes) > 64 * 1024 * 1024:
        raise OMFError('OMF larger than 64 MiB supported input cap')
    off = 0
    while off < len(file_bytes):
        if off + 4 > len(file_bytes):
            raise OMFError('truncated OMF record header')
        kind = file_bytes[off]
        reclen = int.from_bytes(file_bytes[off + 1:off + 3], 'little')
        if reclen < 1 or off + 3 + reclen > len(file_bytes):
            raise OMFError('invalid or truncated OMF record length')
        end = off + 3 + reclen
        entire = file_bytes[off:end]
        checksum = entire[-1]
        # Checksum zero is explicitly permitted by OMF for unchecked records.
        if checksum != 0 and sum(entire) & 0xFF:
            raise OMFError(f'bad OMF checksum at {off:#x}')
        yield kind, entire[3:-1], off, checksum != 0
        off = end


def parse_omf(file_bytes: bytes) -> dict:
    counts: Counter[str] = Counter()
    names = ['']
    segments = [None]
    public = []
    externals = ['']
    fixups = []
    ledatas = []
    unknown = set()
    unsupported_code = set()
    checksummed = 0
    unchecked = 0
    module_ended = False
    last_data = None
    frames = [None] * 4
    targets = [None] * 4
    initial = True
    nrec = 0
    for tag, payload, off, checked in records(file_bytes):
        if module_ended:
            raise OMFError('record after MODEND')
        if initial and tag != 0x80:
            raise OMFError('first OMF record must be THEADR')
        initial = False
        if tag not in (0x9C, 0x9D, 0xA0, 0xA1):
            # FIXUPP must follow its LEDATA immediately (or another FIXUPP).
            last_data = None
        nrec += 1
        counts[f'{tag:02X}'] += 1
        checksummed += bool(checked)
        unchecked += not checked
        c = Cursor(payload)
        if tag == 0x80:  # THEADR
            c.string()
            c.done()
        elif tag == 0x96:  # LNAMES
            while c.p < len(payload):
                names.append(c.string())
        elif tag in (0x98, 0x99):  # SEGDEF / SEGDEF32
            attrib = c.u8()
            if attrib >> 5 == 0:
                c.take(3)  # absolute-frame special alignment
            declared_size = c.uint(4 if tag == 0x99 else 2)
            if (attrib & 0x02) and declared_size == 0:
                # Big=1 zero length encodes 4 GiB in SEGDEF32; unsupported.
                raise OMFError('SEGDEF big segment needs explicit handling')
            name_idx, class_idx, overlay_idx = c.index(), c.index(), c.index()
            if not all(i < len(names) for i in (name_idx, class_idx, overlay_idx)):
                raise OMFError('unknown LNAMES segment/class/overlay index')
            # Preserve unsupported SEGDEF extension bytes as a visible blocker.
            tail = c.take(len(payload) - c.p)
            if tail:
                unsupported_code.add('SEGDEF_extra_attributes')
            segments.append({'index': len(segments), 'name': names[name_idx],
                             'class': names[class_idx], 'size': declared_size,
                             'use32': bool(attrib & 1),
                             'record_32bit_fields': tag == 0x99, 'alignment': attrib >> 5})
        elif tag in (0x90, 0x91):  # PUBDEF / PUBDEF32
            group_idx, segment_idx = c.index(), c.index()
            if segment_idx >= len(segments):
                raise OMFError('PUBDEF names undefined segment')
            if segment_idx == 0:
                c.take(2)
            while c.p < len(payload):
                label = c.string()
                addr = c.uint(4 if tag == 0x91 else 2)
                c.index()  # type index
                public.append({'name': label, 'segment': segment_idx,
                               'offset': addr, 'group': group_idx})
        elif tag == 0x8C:  # EXTDEF
            while c.p < len(payload):
                externals.append(c.string())
                c.index()  # type index
        elif tag in (0xA0, 0xA1):  # LEDATA / LEDATA32
            seg = c.index()
            if seg < 1 or seg >= len(segments):
                raise OMFError('LEDATA references undefined segment')
            offset = c.uint(4 if tag == 0xA1 else 2)
            size = len(payload) - c.p
            declared = segments[seg]['size']
            if offset + size > declared:
                raise OMFError('LEDATA exceeds declared SEGDEF segment size')
            region = (seg, offset, size)
            for prior_seg, prior_start, prior_length in ledatas:
                if seg == prior_seg and offset < prior_start + prior_length and prior_start < offset + size:
                    raise OMFError('overlapping LEDATA records')
            ledatas.append(region)
            last_data = region
        elif tag in (0x9C, 0x9D):  # FIXUPP / FIXUPP32
            while c.p < len(payload):
                tag0 = c.u8()
                if not tag0 & 0x80:  # THREAD subrecord
                    is_frame = bool(tag0 & 0x40)
                    method = (tag0 >> 2) & 7
                    thread_slot = tag0 & 3
                    if method > (5 if is_frame else 3):
                        raise OMFError('unsupported FIXUPP thread method')
                    datum = c.index() if method < (3 if is_frame else 3) else None
                    (frames if is_frame else targets)[thread_slot] = (method, datum)
                    continue
                if last_data is None:
                    raise OMFError('FIXUPP relocation without previous LEDATA')
                tag1 = c.u8()
                locat = (tag0 << 8) | tag1
                at = locat & 0x3FF
                loc = (locat >> 10) & 0xF
                self_relative = not bool(locat & 0x4000)
                width = LOCATION_BYTES.get(loc)
                if width is None:
                    raise OMFError(f'unsupported OMF32 relocation location {loc}')
                if at + width > last_data[2]:
                    raise OMFError('FIXUPP crosses source LEDATA boundary')
                fixdat = c.u8()
                if fixdat & 0x80:
                    frame = frames[(fixdat >> 4) & 3]
                    if frame is None:
                        raise OMFError('undefined FIXUPP frame thread')
                else:
                    fm = (fixdat >> 4) & 7
                    if fm > 5:
                        raise OMFError('unsupported FIXUPP frame method')
                    frame = (fm, c.index() if fm <= 2 else None)
                if fixdat & 0x08:
                    target = targets[fixdat & 3]
                    if target is None:
                        raise OMFError('undefined FIXUPP target thread')
                else:
                    target = (fixdat & 3, c.index())
                disp = c.uint(4 if tag == 0x9D else 2) if not fixdat & 0x04 else 0
                method, datum = target
                if method == 2 and datum is not None and datum >= len(externals):
                    raise OMFError('unknown external symbol FIXUPP index')
                fixups.append({'segment': last_data[0], 'offset': last_data[1] + at,
                               'location_code': loc, 'width': width,
                               'self_relative': self_relative,
                               'frame_method': frame[0], 'frame_index': frame[1],
                               'target_method': method, 'target_index': datum,
                               'target_external': externals[datum] if method == 2 and datum else None,
                               'target_displacement': disp})
        elif tag in (0xA2, 0xA3, 0xC2, 0xC3):
            unsupported_code.add(f'code/data record {tag:02X} (LIDATA/COMDAT)')
            last_data = None
        elif tag in (0x8A, 0x8B):
            module_ended = True
            last_data = None
        else:
            if tag not in KNOWN:
                unknown.add(f'{tag:02X}')
            if tag not in (0x88, 0x9A, 0x9B):
                # Some records carry data/exports beyond core parsed tags.
                unsupported_code.add(f'unparsed record {tag:02X}')
            if tag != 0x88:
                last_data = None
    if not module_ended or not nrec:
        raise OMFError('missing OMF MODEND')
    if not segments[1:]:
        raise OMFError('missing OMF SEGDEF')
    # This is an inventory ONLY. No relocations were applied.
    return {'schema': 'rloft-omf-inventory-v1', 'format': 'OMF16/OMF32',
            'compiler_identified': False, 'code_bytes_matched': False,
            'fixups_applied': False, 'le_fixup_equivalence': 'not_tested',
            'object_sha256': hashlib.sha256(file_bytes).hexdigest(),
            'record_count': nrec, 'record_types': dict(sorted(counts.items())),
            'validated_checksums': checksummed, 'unchecked_checksum_records': unchecked,
            'segments': segments[1:], 'public_symbols': public,
            'externals': externals[1:], 'ledata_ranges': ledatas, 'fixups': fixups,
            'unknown_record_types': sorted(unknown),
            'unparsed_code_features': sorted(unsupported_code)}


def main() -> None:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('original_watcom_obj', type=Path)
    p.add_argument('--output', type=Path, help='Metadata only; does not export code bytes')
    p.add_argument('--expected-sha256', help='Optional previously pinned Watcom object SHA-256')
    args = p.parse_args()
    data = args.original_watcom_obj.read_bytes()
    report = parse_omf(data)
    if args.expected_sha256 and report['object_sha256'] != args.expected_sha256:
        p.error('Watcom OMF object identity mismatch')
    encoded = json.dumps(report, sort_keys=True, indent=2) + '\n'
    if args.output:
        args.output.write_text(encoded, encoding='utf-8')
    else:
        print(encoded, end='')


if __name__ == '__main__':
    main()
