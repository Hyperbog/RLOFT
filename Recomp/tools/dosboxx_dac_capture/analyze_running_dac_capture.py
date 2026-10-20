#!/usr/bin/env python3
"""Inspect real DOSBox-X DAC I/O and optional running-game hotkey .sav.

This NEVER claims a same-instant trace/snapshot boundary. A saved VGA palette
and a later-copied log can match without proving exact frame synchronization.
No original media, raw palette, or original event logs are published.
"""
import argparse
import hashlib
import json
import sys
import tempfile
import zipfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import convert_capture as converter
sys.path.insert(0, str(HERE.parent))
import verify_dosboxx_palette_port_trace as verifier


def summarize_game_palette_submissions(events, hints):
    """Retain per-game full-bank deltas without inventing a partial DAC I/O."""
    entries = []
    active = None
    for event, raw_hint in zip(events, hints):
        if event['port'] == 0x3c8:
            if active is not None:
                entries.append(active)
            active = {'seq': event['seq'], 'start_index': event['value'],
                      'cs': raw_hint['guest_cs_hint'], 'rgb6': [],
                      'data_hints': {}}
        elif active is not None:
            active['rgb6'].append(event['value'])
            key = '0x%X' % raw_hint['guest_eip_hint']
            active['data_hints'][key] = active['data_hints'].get(key, 0) + 1
    if active is not None:
        entries.append(active)
    # 0x0180 is an observed protected-mode CS hint from the original
    # owner's CODE.2 recording. It is NOT an independently proven selector
    # for every DOSBox-X process or every game installation.
    game = [x for x in entries if x['start_index'] == 0 and
            len(x['rgb6']) == 768 and x['cs'] == 0x180]
    deltas = []
    for earlier, later in zip(game, game[1:]):
        changed = [i for i in range(256)
                   if earlier['rgb6'][i*3:i*3+3] !=
                      later['rgb6'][i*3:i*3+3]]
        if 0 < len(changed) <= 16:
            deltas.append({
                'from_pass': game.index(earlier)+1,
                'to_pass': game.index(later)+1,
                'changed_indices': changed,
                'changed_RGB6_channels': sum(a != b for a, b in zip(
                    earlier['rgb6'], later['rgb6'])),
                'before_RGB6': {str(i):earlier['rgb6'][3*i:3*i+3]
                                for i in changed},
                'after_RGB6': {str(i):later['rgb6'][3*i:3*i+3]
                               for i in changed},
                'last_submitted_index_event_seq': later['seq'],
            })
    return {
        'game_full_bank_submissions_from_CS_0180_hint': len(game),
        'game_full_bank_start_seqs': [x['seq'] for x in game],
        'game_full_bank_palette_sha256':
            [hashlib.sha256(bytes(x['rgb6'])).hexdigest() for x in game],
        'last_two_full_bank_submissions_equal':
            len(game) >= 2 and game[-1]['rgb6'] == game[-2]['rgb6'],
        'sparse_RGB6_palette_changes_between_game_submissions': deltas,
        'source_provenance': 'ORIGINAL_CODE2_CALLER_NOT_VERIFIED',
    }


def audit_original_writer_code(code_bytes, raw_hints):
    """Validate original retail file and decode stable runtime EIP hints."""
    if hashlib.sha256(code_bytes).hexdigest() != verifier.CODE2_SHA:
        raise ValueError('Original CODE.2 checksum mismatch')
    obj = code_bytes[0x3af24:]
    signatures = ((0x29ca0,bytes.fromhex('c8000000')),
                  (0x29cb2,bytes.fromhex('66bac803ee66bac903')),
                  (0x29ccf,bytes.fromhex('f36e')),
                  (0x29ce0,bytes.fromhex('f36e')))
    if any(obj[offset:offset+len(expected)] != expected
           for offset, expected in signatures):
        raise ValueError('Original VGA DAC writer machine bytes mismatch')
    observations = {(x['port'],x['guest_cs_hint'],x['guest_eip_hint'])
                    for x in raw_hints}
    expectations = {(968,0x180,0x1eeca0),(969,0x180,0x1eeccf),
                    (969,0x180,0x1eece0)}
    matched = expectations.issubset(observations)
    return {
        'writer_source_original_CODE2_authenticated':True,
        'all_three_retail_DAC_writer_EIP_hints_observed': matched,
        'object1_runtime_base_inferred': '0x1C5000' if matched else None,
        'raw_EIP_hints_and_object1_offsets': [
            ['0x1EECA0','0x29CA0'],['0x1EECCF','0x29CCF'],
            ['0x1EECE0','0x29CE0']] if matched else [],
        'original_index_out_instruction_object1': '0x29CB6',
        'CPU_instruction_ancestry': 'NOT_ATTESTED',
    }


def inspect(raw, save_file=None, original_code2=None):
    events, meta = converter.normalize(raw)
    # The normalized replay strips raw CPU hints after validating their structure.
    hints = [json.loads(row) for row in raw.splitlines()[1:]]
    palette = bytearray(768)
    known = bytearray(768)
    index, component = None, 0
    writes = [0] * 256
    samples = {str(i): [] for i in range(246, 254)}
    last_writer = {}
    for e, hint in zip(events, hints):
        if e['port'] == 0x3c8:
            index = e['value']
            component = 0
            continue
        verifier.require(index is not None, '3C9 data before 3C8 index')
        verifier.require(e['value'] <= 63, 'Not an RGB6 hardware component')
        pos = (index & 255)*3 + component
        palette[pos] = e['value']
        known[pos] = 1
        writes[index & 255] += 1
        if 246 <= index <= 253:
            last_writer[pos] = e
            sampled = samples[str(index)]
            if len(sampled) < 12:
                sampled.append({
                    'seq': e['seq'], 'channel': component, 'value': e['value'],
                    'guest_cs_hint': hint['guest_cs_hint'],
                    'guest_eip_hint': hint['guest_eip_hint'],
                })
        component += 1
        if component == 3:
            component = 0
            index = (index+1) & 255
    covered = sum(known)
    result = {
        'status': 'RUNTIME_DAC_EVENTS_WITHOUT_SAV',
        'evidence_class': 'DECLARED_INSTRUMENTED_EMULATOR_NOT_EXTERNALLY_ATTESTED',
        'raw_log_sha256': hashlib.sha256(raw).hexdigest(),
        'event_count': len(events),
        'distinct_RGB6_channels_written': covered,
        'complete_palette_observed': covered == 768,
        'last_observed_palette_sha256': hashlib.sha256(palette).hexdigest() if covered == 768 else None,
        'indices_246_to_253': {
            str(i): {
                'channels_observed': sum(known[i*3:i*3+3]),
                'write_count': writes[i],
                'first_12_writes': samples[str(i)],
                'last_channel_write_seq': [last_writer[k]['seq'] if k in last_writer else None
                                           for k in range(i*3,i*3+3)],
            } for i in range(246,254)
        },
        'raw_CS_EIP': 'GUEST_REGISTER_HINTS_ONLY_NOT_VERIFIED_CODE2_INSTRUCTION_ADDRESSES',
        'same_instant_sav_event_alignment': 'NOT_CAPTURED',
        'original_palette_update_caller': 'NOT_VERIFIED',
        'original_native_frame_parity': 'NOT_VERIFIED',
        'game_submissions': summarize_game_palette_submissions(events, hints),
        'original_writer': audit_original_writer_code(original_code2, hints)
            if original_code2 is not None else None,
        'input': meta,
    }
    if save_file is not None:
        reference = verifier.snapshot(save_file)
        differences = sorted(set(j//3 for j in range(768) if known[j] and
                                 palette[j] != reference[j]))
        exact = covered == 768 and not differences
        result['status'] = ('RUNNING_SAVE_DAC_MATCH_UNSYNCHRONIZED' if exact
                            else 'RUNNING_SAVE_DAC_MISMATCH_OR_INCOMPLETE')
        result['saved_state_sha256'] = hashlib.sha256(save_file.read_bytes()).hexdigest()
        result['saved_RGB6_sha256'] = hashlib.sha256(reference).hexdigest()
        result['last_observed_palette_matches_saved_DAC'] = exact
        result['different_color_indices'] = differences
        result['snapshot_warning'] = (
            'No atomic port-event index was recorded at save time. Even if '
            'the final observed palette equals the saved VGA DAC, this does '
            'not certify synchronous capture or original CPU caller ancestry.')
    return result


def self_test():
    with tempfile.TemporaryDirectory() as tmp:
        tmp = Path(tmp)
        head = converter.EXPECTED_HEADER
        events = [{'seq':0,'port':968,'value':0,
                   'guest_cs_hint':0x17,'guest_eip_hint':0x100}]
        for i in range(768):
            events.append({'seq':i+1,'port':969,'value':(i*7)%64,
                           'guest_cs_hint':0x17,'guest_eip_hint':0x100})
        def encode(event_list):
            return b'\n'.join(json.dumps(x,separators=(',',':')).encode()
                              for x in [head]+event_list)+b'\n'
        raw = encode(events)
        result = inspect(raw)
        assert result['complete_palette_observed']
        assert result['indices_246_to_253']['246']['channels_observed'] == 3
        # Two observed CS=0180 full-bank submissions differing at exactly
        # the targeted eight entries. Synthetic fixtures remain synthetic.
        first = bytes((i*7) % 64 for i in range(768))
        second = bytearray(first)
        for i in range(246,254):
            second[3*i:3*i+3] = bytes((i%64, i%64, i%64))
        special = []
        for bank in (first, second):
            special.append({'seq':len(special),'port':968,'value':0,
                            'guest_cs_hint':0x180,'guest_eip_hint':0x1eeca0})
            for val in bank:
                special.append({'seq':len(special),'port':969,'value':val,
                                'guest_cs_hint':0x180,
                                'guest_eip_hint':0x1eeccf})
        trace = inspect(encode(special))
        audit = trace['game_submissions']
        assert audit['game_full_bank_submissions_from_CS_0180_hint'] == 2
        assert audit['sparse_RGB6_palette_changes_between_game_submissions']
        transition = audit['sparse_RGB6_palette_changes_between_game_submissions'][0]
        assert transition['changed_indices'] == list(range(246,254))
        assert transition['changed_RGB6_channels'] == 24

        sav = tmp/'SYNTHETIC_NOT_RETAIL.sav'
        vga = bytearray(verifier.VGA_SIZE)
        vga[verifier.DAC_POS] = 6
        vga[verifier.DAC_POS+1] = 255
        vga[verifier.PALETTE_POS:verifier.PALETTE_POS+768] = \
            bytes((i*7)%64 for i in range(768))
        def save_fixture():
            with zipfile.ZipFile(sav,'w') as z:
                z.writestr('DOSBox-X_Version',verifier.DOSBOX_VERSION)
                z.writestr('Machine_Type',b'MCH_VGA')
                z.writestr('Program_Name',b'CODE')
                z.writestr('Vga',vga)
        save_fixture()
        same = inspect(raw,sav)
        assert same['status'] == 'RUNNING_SAVE_DAC_MATCH_UNSYNCHRONIZED'
        assert same['same_instant_sav_event_alignment'] == 'NOT_CAPTURED'
        incomplete = inspect(encode(events[:-1]),sav)
        assert incomplete['status'] == 'RUNNING_SAVE_DAC_MISMATCH_OR_INCOMPLETE'
        vga[verifier.PALETTE_POS+246*3] ^= 1
        save_fixture()
        different = inspect(raw,sav)
        assert different['different_color_indices'] == [246]
        print('SYNTHETIC_RUNNING_DAC_AUDIT_PASS exact_match_UNSYNCHRONIZED '
              'incomplete_REJECTED mismatch246_DETECTED')


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--raw-jsonl',type=Path)
    p.add_argument('--state',type=Path,help='Optional .sav created while running')
    p.add_argument('--code2',type=Path,help='Optional authenticated retail CODE.2 binary to test writer EIP normalization')
    p.add_argument('--report',type=Path)
    p.add_argument('--self-test',action='store_true')
    args=p.parse_args()
    if args.self_test:self_test()
    if args.raw_jsonl is None:
        if args.self_test:return
        p.error('Provide --raw-jsonl')
    result=inspect(args.raw_jsonl.read_bytes(),args.state,
                   args.code2.read_bytes() if args.code2 else None)
    if args.report:
        args.report.parent.mkdir(parents=True,exist_ok=True)
        args.report.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
    print(result['status'], 'channels=%d/768' % result['distinct_RGB6_channels_written'],
          'same_instant_alignment=NOT_CAPTURED')


if __name__=='__main__': main()
