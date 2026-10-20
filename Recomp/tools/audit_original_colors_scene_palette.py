#!/usr/bin/env python3
"""Audit original retail COLORS bank 0->3 and CODE.2 scene palette chain.

Only original CODE.2 and COLORS with verified SHA256 are accepted.
Optional owner's DOSBox-X 2026.10.01 DAC trace and .sav support exact
original-bank matching. Never emit or publish original media bytes, full
palettes, original trace events, saved memory, or original game artwork.

STATIC EVIDENCE verifies a possible source path, not executed CPU ancestry.
"""
import argparse
import hashlib
import json
import struct
import zipfile
from pathlib import Path

CODE2_SHA = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
COLORS_SHA = "e366986bf701ba832aa769bf99f0db90a76af658300ce3d02e7039a970a9baf5"
BANK0_SHA = "1f60d7831cd7b034ae67c894dc91662f15d4341fbf28e2a954b1b7e3e780f64f"
BANK3_SHA = "0af8ea393f1f266596af59c1a2013350b51000b7456d377ab284ebbc43db0812"
ORIGINAL_GAME_LEVEL5_PALETTE = 3
CODE_OBJ1_OFFSET = 0x3AF24
CODE_OBJ2_OFFSET = 0x75F24
TRACE_HOOK = "dosboxx-v2026.10.01-dac-v1"
SOURCE_ANCHORS = {
    # index scene descriptor (three bytes per level), push resulting index,
    # near call to 1F8E0 selector setter:
    0x18C78: "8b5424548d04950000000029d00fb680ac81000050e84e6c0000",
    # reads current selected palette, flags screen palette refresh,
    # persists new value at object2:0xC034:
    0x1F8E0: "a134c00000c60558cf0000013b44240474ed8b442404a334c00000",
    # REFRESH consumer; 1FA: load COLORS by palette id from 0xC034
    # into C054 using 325B0, then call 29234 to submit:
    0x1FAC4: "68000100006828110000ff3534c000006a006854c00000e8d02a010083c4146854c00000e847970000",
    # The original 325B0 COLORS range loader does seek=1+768*bank,
    # reads into staging buffer and invokes palette normalization 32150:
    0x325B0: "53ff742414e8ceffffff83c40489c38b542410508d04950000000029d0c1e0086a014050e843edffff",
    0x32605: "ff742408e842fbffff83c404",
    0x29234: "68000300006a00ff74240ce85c0a000083c40cc3",
}
# Owner-private DOSBox-X 2026.10.01 SAV Memory serializer writes struct
# header (0x88 bytes) then MemBase physical memory; source from
# tagged src/hardware/memory.cpp::SerializeMemory::getBytes.
SAV_MEMBASE_OFFSET = 0x88
OBSERVED_DATA_LINEAR_BASE = 0x201000


def require(ok, label):
    if not ok:
        raise ValueError(label)


def sha(data):
    return hashlib.sha256(data).hexdigest()


def palette_diff(p, q):
    require(len(p) == 768 and len(q) == 768, "wrong palette width")
    return [n for n in range(256) if p[n*3:n*3+3] != q[n*3:n*3+3]]


def static_audit(code2, colors):
    require(sha(code2) == CODE2_SHA, "Authentic CODE.2 SHA mismatch")
    require(sha(colors) == COLORS_SHA, "Authentic COLORS SHA mismatch")
    require(len(colors) == 11521 and colors[0] == 15,
            "Authentic COLORS palette-bank format changed")
    banks = [colors[1 + i*768:1 + (i+1)*768] for i in range(15)]
    require(all(len(p) == 768 and max(p) <= 63 for p in banks),
            "invalid 6-bit palette-bank payload")
    require(sha(banks[0]) == BANK0_SHA and sha(banks[3]) == BANK3_SHA,
            "Original COLORS bank checksums changed")
    require(palette_diff(banks[0], banks[3]) == list(range(246,254)),
            "Original bank 0 and 3 differ outside 246-253")
    require(sum(a != b for a,b in zip(banks[0],banks[3])) == 24,
            "Original COLORS transition RGB channel count changed")

    code = code2[CODE_OBJ1_OFFSET:]
    for offset, hexdump in SOURCE_ANCHORS.items():
        expected = bytes.fromhex(hexdump)
        require(code[offset:offset+len(expected)] == expected,
                "Original CODE.2 machine-code anchor changed: " + hex(offset))
    require(code2[CODE_OBJ2_OFFSET + 0x81AC + 3*5] == 3,
            "Original Level 5 scene palette selector is not 3")
    # 325B0 palette loader read+postprocess call and 29234 DAC wrapper.
    return {
        "status": "AUTHENTIC_RETAIL_SCENE_PALETTE_BANK_0_TO_3_STATIC_PATH_PASS",
        "original_code2_sha256": CODE2_SHA,
        "original_colors_sha256": COLORS_SHA,
        "colors_bank_0_sha256": BANK0_SHA,
        "colors_bank_3_sha256": BANK3_SHA,
        "bank0_vs_bank3_different_indices": list(range(246,254)),
        "changed_RGB6_components": 24,
        "unaffected_color_count": 248,
        "original_level5_three_byte_descriptor_first_byte": ORIGINAL_GAME_LEVEL5_PALETTE,
        "original_function_path": {
            "level_table_index": "object2:0x81AC + 3*level_id",
            "selector_call_at": "object1:0x18C8D",
            "current_palette_setter": "object1:0x1F8E0",
            "selected_palette_dgroup": "object2:0xC034",
            "palette_refresh_flag": "object2:0xCF58",
            "palette_loader": "object1:0x325B0",
            "optional_brightness_normalizer": "object1:0x32150",
            "palette_staging_buffer": "object2:0xC054",
            "palette_DAC_full_bank_wrapper": "object1:0x29234",
            "palette_DAC_instruction_writer": "object1:0x29CA0",
        },
        "runtime_call_order": "NOT_VERIFIED_BY_V1_DAC_CAPTURE",
        "animation_phase_at_emulator_save": "NOT_SYNCHRONIZED",
    }, banks


def game_submissions(raw):
    lines = raw.splitlines()
    require(len(lines) > 2, "truncated DAC trace")
    meta = json.loads(lines[0])
    require(meta == {
        "rloft_hook": TRACE_HOOK,
        "upstream_vga_blob": "509d42fbead67a56594e49cec55549bc6fcc6d7c",
        "capture_stage": "accepted_after_lockout_and_rgb6_mask",
        "max_events": 200000
    }, "Wrong original DOSBox-X instrumented capture header")
    require(len(lines) <= 200001, "Overlong port history")
    banks, active = [], None
    for seq, row in enumerate(lines[1:]):
        x=json.loads(row)
        require(set(x)=={"seq","port","value","guest_cs_hint","guest_eip_hint"} and
                type(x["seq"]) is int and x["seq"] == seq and
                type(x["port"]) is int and x["port"] in (968,969) and
                type(x["value"]) is int and
                0<=x["value"]<=(255 if x["port"]==968 else 63) and
                type(x["guest_cs_hint"]) is int and
                0<=x["guest_cs_hint"]<=65535 and
                type(x["guest_eip_hint"]) is int and
                0<=x["guest_eip_hint"]<=0xffffffff,
                "Untrusted DAC port event data or ordering")
        if x["port"]==968:
            if active is not None:
                banks.append(active)
            active={"seq":seq,"index":x["value"],
                    "cs":x["guest_cs_hint"],"rgb6":bytearray()}
        elif active is not None:
            active["rgb6"].append(x["value"])
    if active is not None:
        banks.append(active)
    return [b for b in banks if b["cs"]==0x180 and
            b["index"]==0 and len(b["rgb6"])==768]


def audit_trace(raw, banks):
    passes=game_submissions(raw)
    require(len(passes)>=2,"Missing complete original game DAC palette submissions")
    swaps=[]
    for i in range(1,len(passes)):
        prev=bytes(passes[i-1]["rgb6"])
        nxt=bytes(passes[i]["rgb6"])
        if prev==banks[0] and nxt==banks[3]:
            swaps.append({"before_pass":i,"after_pass":i+1,
                          "before_event_seq":passes[i-1]["seq"],
                          "after_event_seq":passes[i]["seq"],
                          "changed_indices":palette_diff(prev,nxt)})
    require(swaps, "No exact original COLORS bank0->bank3 transition recorded")
    return {
        "status":"ORIGINAL_DOS_DAC_0_TO_3_BANK_TRANSITION_BYTE_EQUAL",
        "trace_sha256":sha(raw),
        "complete_game_palette_submissions":len(passes),
        "bank0_to_bank3_transitions":swaps,
        "final_submitted_palette_sha256":sha(bytes(passes[-1]["rgb6"])),
        "source_loader_call_at_transition":"NOT_INSTRUMENTED",
    }


def audit_sav(raw, bank3):
    with zipfile.ZipFile(raw) as z:
        require(z.testzip() is None, "Corrupted emulator SAV")
        require(z.read("DOSBox-X_Version")==b"DOSBox-X 2026.10.01 (SDL2)",
                "Wrong DOSBox-X save version")
        require(z.read("Program_Name")==b"CODE", "Unexpected game process")
        vga=z.read("Vga"); require(len(vga)==2643024,"VGA save layout changed")
        require(vga[0x82FB1] == 6, "Saved RGB6 DAC depth changed")
        require(vga[0x82FD1:0x82FD1+768]==bank3,
                "Emulator saved DAC does not equal COLORS bank 3")
        mem=z.read("Memory")
    require(len(mem)>0x210A58+768,"Saved memory component truncated")
    # DOSBox-X stored MemBase after the 0x88-byte structural header.
    base=SAV_MEMBASE_OFFSET+OBSERVED_DATA_LINEAR_BASE
    require(mem[base+0xC054:base+0xC054+768]==bank3,
            "Saved original CODE.2 C054 palette != bank3")
    current=struct.unpack_from("<I",mem,base+0xC034)[0]
    require(current==3,"Saved original code selected palette is not 3")
    require(mem[base+0x81AC+3*5]==3,
            "Saved original scene table no longer selects 3 for Level 5")
    return {
        "status":"SAVED_CODE2_BANK3_STAGING_AND_VGA_MATCH",
        "save_sha256":sha(raw),
        "original_palette_selector_C034":current,
        "palette_staging_C054_full_bank_equals_COLORS3":True,
        "VGA_DAC_equals_COLORS3":True,
        "DOSBoxX_Memory_blob_offset":"0x88",
        "inferred_original_data_linear_base":"0x201000",
        "same_instant_DAC_trace_to_sav":"NOT_ATTESTED",
    }


def self_test():
    a=bytes(range(256))*3;b=bytearray(a)
    for i in range(246,254):b[i*3:i*3+3]=bytes([63,62,61])
    require(palette_diff(a,b)==list(range(246,254)),
            "Synthetic partial palette test failed")
    try:static_audit(b"bad",bytes(11521))
    except ValueError as e:require("CODE.2 SHA" in str(e),"wrong invalid-original rejection")
    else:raise AssertionError("Unverified original accepted")
    print("SELF_TEST_PASS original_SHA_gate and synthetic palette-range detector")


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--code2",type=Path)
    p.add_argument("--colors",type=Path)
    p.add_argument("--trace",type=Path,help="Owner-private runtime port trace; optional")
    p.add_argument("--sav",type=Path,help="Owner-private DOSBox-X .sav; optional")
    p.add_argument("--report",type=Path,help="Owner-private scalar/hash summary only")
    p.add_argument("--self-test",action="store_true")
    args=p.parse_args()
    if args.self_test:self_test()
    if args.code2 is None and args.colors is None:
        if args.self_test:return
        p.error("--code2 and --colors must be supplied together")
    if args.code2 is None or args.colors is None:
        p.error("--code2 and --colors must be supplied together")
    report,banks=static_audit(args.code2.read_bytes(),args.colors.read_bytes())
    if args.trace:report["DOS_capture"]=audit_trace(args.trace.read_bytes(),banks)
    if args.sav:report["DOS_emulator_sav"]=audit_sav(args.sav.read_bytes(),banks[3])
    if args.report:
        args.report.parent.mkdir(parents=True,exist_ok=True)
        args.report.write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
    print("ORIGINAL_COLORS_SCENE_BANK3_SOURCE_PASS "
          "source_LEVEL5_81AC=3 palette_C034=3 "
          "bank0_to_3_changed_indices=246..253 RGB6_components=24 "
          "bank0_sha="+BANK0_SHA+" bank3_sha="+BANK3_SHA+" "
          "runtime_setter_and_loader_CALL_ORDER="+
          "NOT_VERIFIED_BY_V1_DAC_CAPTURE")


if __name__=="__main__":
    main()
