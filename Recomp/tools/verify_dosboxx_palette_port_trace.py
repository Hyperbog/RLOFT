#!/usr/bin/env python3
"""Fail-closed VGA DAC replay from *independently captured* DOSBox-X I/O.

The trace's provenance is declared, not cryptographically attested.
A matching saved VGA palette verifies output bytes, not game call ancestry.
Do not pass recreated/native events as independent original execution.
"""
import argparse
import hashlib
import json
import zipfile
from pathlib import Path

SCHEMA = 'rloft-dosboxx-original-dac-port-v1'
CODE2_SHA = '853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314'
DOSBOX_VERSION = b'DOSBox-X 2026.10.01 (SDL2)'
VGA_SIZE = 2643024
DAC_POS = 0x82FB1
PALETTE_POS = 0x82FD1
PALETTE_LEN = 768


def digest(raw):
    return hashlib.sha256(raw).hexdigest()


def require(condition, reason):
    if not condition:
        raise ValueError(reason)


def replay(events):
    """Only byte OUT events; REP OUTSB must be expanded by the emulator hook."""
    require(isinstance(events,list) and 1<=len(events)<=200000,
            'Invalid DAC event count')
    rgb = bytearray(PALETTE_LEN)
    covered = bytearray(PALETTE_LEN)
    resets = 0
    dac_index = None
    component = 0
    for seq, event in enumerate(events):
        require(isinstance(event,dict) and
                set(event).issubset({'seq','port','value','object1_eip'}) and
                {'seq','port','value'}.issubset(event),
                'Invalid DAC event fields')
        require(type(event['seq']) is int and event['seq']==seq,
                'Noncontiguous DAC event sequence')
        port, value = event['port'],event['value']
        require(type(port) is int and port in (0x3C8,0x3C9) and
                type(value) is int and 0<=value<=255,
                'Non-byte DAC event or unexpected port')
        if 'object1_eip' in event:
            eip=event['object1_eip']
            require(type(eip) is int and 0<=eip<0x3A852,
                    'Invalid CODE.2 object-1 EIP label')
        if port==0x3C8:
            dac_index = value
            component=0
            resets+=1
            continue
        require(dac_index is not None, 'DAC data before index initialization')
        require(value<=63, 'Non-RGB6 component value')
        location=(dac_index%256)*3+component
        rgb[location]=value
        covered[location]=1
        component+=1
        if component==3:
            component=0
            dac_index=(dac_index+1)%256
    missing = PALETTE_LEN-sum(covered)
    require(not missing, 'Incomplete palette: %d RGB6 channels never written' % missing)
    require(resets>0,'No indexed DAC writer')
    return bytes(rgb), {'events':len(events), 'index_writes':resets,
                        'RGB6_channel_writes':len(events)-resets,
                        'distinct_RGB6_channels_written':sum(covered)}


def snapshot(state):
    """Extract only the recorded VGA DAC from the version-guarded SAV."""
    with zipfile.ZipFile(state) as z:
        require(z.read('DOSBox-X_Version')==DOSBOX_VERSION,
                'Unsupported DOSBox-X save-state version')
        require(z.read('Machine_Type')==b'MCH_VGA',
                'Original reference is not a VGA save state')
        require(z.read('Program_Name')==b'CODE',
                'Save state does not identify the retail CODE application')
        vga=z.read('Vga')
    require(len(vga)==VGA_SIZE,'Unverified VGA serialization layout')
    require(vga[DAC_POS]==6 and vga[DAC_POS+1]==255,
            'Unexpected DAC precision or PEL mask')
    colors=vga[PALETTE_POS:PALETTE_POS+PALETTE_LEN]
    require(len(colors)==PALETTE_LEN and all(x<=63 for x in colors),
            'Invalid RGB6 DAC snapshot')
    return colors


def validate(document, sav_raw, expected_palette):
    require(isinstance(document,dict) and document.get('schema')==SCHEMA,
            'Unrecognized original-DOS port-trace schema')
    provenance=document.get('provenance')
    require(isinstance(provenance,dict) and
            provenance.get('origin')=='independent_retail_dosboxx_io_hook' and
            provenance.get('program_sha256')==CODE2_SHA and
            provenance.get('dosbox_version')==DOSBOX_VERSION.decode() and
            provenance.get('saved_state_sha256')==digest(sav_raw) and
            isinstance(provenance.get('capture_id'),str) and
            0<len(provenance['capture_id'])<=100 and
            provenance.get('flush_before_snapshot') is True,
            'Missing independent-original capture declaration or SAV binding')
    output, stats=replay(document.get('events'))
    require(output==expected_palette,
            'DAC port-write replay differs from independent saved VGA DAC')
    return {'status':'DAC_RGB6_EQUAL_DECLARED_CAPTURE_UNATTESTED',
            'state_sha256':digest(sav_raw),
            'saved_and_replayed_RGB6_sha256':digest(output),
            'statistics':stats,
            'original_CPU_call_chain':'NOT_ATTESTED',
            'same_phase_frame_and_actor_pixel_parity':'NOT_VERIFIED',
            'caution':'Capture declarations and event values still require independent original DOSBox-X hook provenance; SAV files alone do not supply port-write history.'}


def self_test():
    colors=bytes((i*17)%64 for i in range(PALETTE_LEN))
    events=[{'seq':0,'port':0x3C8,'value':0}]
    events.extend({'seq':i+1,'port':0x3C9,'value':x} for i,x in enumerate(colors))
    actual,stats=replay(events)
    require(actual==colors and stats['distinct_RGB6_channels_written']==768,
            'Complete trace reconstruction failed')
    # The previous regression falsely tested a complete palette as incomplete.
    for bad,needle in ((events[:-1],'Incomplete palette'),
                       ([dict(events[0],port=0x3C9)]+events[1:],'before index'),
                       (events[:3]+[dict(events[3],seq=9)]+events[4:],'Noncontiguous'),
                       (events[:10]+[dict(events[10],value=64)]+events[11:],'Non-RGB6')):
        try:
            replay(bad)
        except ValueError as exc:
            require(needle in str(exc),'Wrong negative-case rejection: '+str(exc))
        else:
            raise AssertionError('Invalid/incomplete independent trace accepted')
    fixture = b'synthetic snapshot binding; NOT a DOSBox-X save'
    doc = {'schema':SCHEMA, 'provenance':{
        'origin':'independent_retail_dosboxx_io_hook',
        'program_sha256':CODE2_SHA, 'dosbox_version':DOSBOX_VERSION.decode(),
        'saved_state_sha256':digest(fixture), 'capture_id':'SYNTHETIC_ONLY',
        'flush_before_snapshot':True}, 'events':events}
    require(validate(doc,fixture,colors)['status'].endswith('UNATTESTED'),
            'Declared capture fixture unexpectedly treated as attested')
    for changed, reference, error in ((doc,bytes([0])*PALETTE_LEN,'differs'),
                                      ({**doc,'provenance':{**doc['provenance'],
                                        'saved_state_sha256':'0'*64}},colors,'declaration')):
        try:
            validate(changed,fixture,reference)
        except ValueError as exc:
            require(error in str(exc),'Unexpected capture-binding rejection')
        else:
            raise AssertionError('Mismatched state or DAC reference accepted')
    print('SELF_TEST_PASS complete RGB6 replay, four negative controls, '
          'snapshot binding and mismatch refusal')


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--trace',type=Path)
    parser.add_argument('--state',type=Path)
    parser.add_argument('--report',type=Path)
    parser.add_argument('--self-test',action='store_true')
    args=parser.parse_args()
    if args.self_test:self_test()
    if args.trace is None and args.state is None:
        if not args.self_test:parser.error('Supply both --trace and --state')
        return
    require(args.trace is not None and args.state is not None,
            'Both independent DOS trace and matching VGA save state required')
    raw=args.state.read_bytes()
    report=validate(json.loads(args.trace.read_text()),raw,snapshot(args.state))
    if args.report:
        args.report.parent.mkdir(parents=True,exist_ok=True)
        args.report.write_text(json.dumps(report,indent=2)+'\n')
    print('ORIGINAL_DOSBOXX_DAC_REPLAY_EQUAL_UNATTESTED '
          'palette_sha256='+report['saved_and_replayed_RGB6_sha256']+
          ' CPU_CALLER_PARITY=NOT_ATTESTED FRAME_PARITY=NOT_VERIFIED')

if __name__=='__main__':main()
