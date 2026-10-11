#!/usr/bin/env python3
"""Original Strahd CODE.2 C050 direct-operand page-state census.

This is a binary literal-address audit, NOT proof of indirect aliases,
callee-saved EBP, original DOS runtime page bounds, or segment mapping.
"""
from __future__ import annotations
import argparse
from collections import Counter
import hashlib
from pathlib import Path
import struct

ROOT=Path(__file__).resolve().parents[2]
SIZE=558689
SHA="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
OBJ_START=0x3AF24
OBJ_SIZE=0x3A852
DATA_START=0x75F24
C050=0xC050
# Literal DWORD occurrence -> exact immediately preceding opcode.
# Reconcile every occurrence, not a selected subset.
PATTERNS={
    "push_dword":("ff35",115),
    "movzx_byte":("0fb605",111),
    "xor_byte":("8035",56),
    "mov_eax":("a1",25),
    "mov_al":("a0",13),
    "imul_edx":("6915",6),
    "mov_edx":("8b15",5),
    "mov_ecx":("8b0d",4),
    "imul_eax":("6905",2),
    "mov_esi":("8b35",1),
    "xor_dword_ebp":("312d",1),
}

def audit_original(path:Path)->None:
    b=path.read_bytes()
    assert len(b)==SIZE and hashlib.sha256(b).hexdigest()==SHA,"wrong original CODE.2"
    obj=b[OBJ_START:OBJ_START+OBJ_SIZE]
    assert len(obj)==OBJ_SIZE
    assert b[DATA_START+C050:DATA_START+C050+4]==b"\x01\x00\x00\x00"
    encoded=struct.pack("<I",C050)
    positions=[]
    at=0
    while True:
        at=obj.find(encoded,at)
        if at<0:break
        positions.append(at)
        at+=4
    assert len(positions)==339,len(positions)
    found=Counter()
    byte_sites=[]
    wide_sites=[]
    for at in positions:
        matches=[name for name,(hex_op,_) in PATTERNS.items()
                 if obj[at-len(bytes.fromhex(hex_op)):at]==bytes.fromhex(hex_op)]
        assert len(matches)==1,(hex(at),matches)
        name=matches[0]
        found[name]+=1
        if name=="xor_byte":
            assert obj[at+4]==1,hex(at)
            byte_sites.append(at-2)
        if name=="xor_dword_ebp":
            wide_sites.append(at-2)
    for name,(_,expected) in PATTERNS.items():
        assert found[name]==expected,(name,found[name],expected)
    assert wide_sites==[0x16221] and 0x160C0 in byte_sites
    # The caller locally initializes EBP=1, but intervening calls
    # and original protected-mode callback effects remain unproved.
    source_windows={
        0x160C0:"803550c0000001",
        0x160E1:"bd0100000089f7",
        0x160E8:"e807410000",
        0x160F6:"ebf0",
        0x161FC:"5557e815e0ffff",
        0x16216:"ff3550c00000",
        0x1621C:"e88f430000",
        0x16221:"312d50c00000",
        0x1622A:"0fb60550c00000",
        0x16234:"e8772f0100",
        0x1623C:"e9a7feffff",
        0x1F1C8:"530fb60550c00000",
        0x1F1D6:"a150c00000c1e00f",
    }
    for at,hex_op in source_windows.items():
        expected=bytes.fromhex(hex_op)
        assert obj[at:at+len(expected)]==expected,hex(at)
    # Source-proven EBP preservation edges, NOT transitive ABI or callback
    # safety. 16400 enters a shared older-address epilogue (1602C).
    ebp_bounded_spans={
        (0x1608C,0x1624B):"06a38753b857225922bdcd64fc227240236235b49885277f98cd86c3fd876a59",
        (0x14218,0x14309):"26ac89581eed8a55c40a5a7e1f97c676b759a6132cb89fd7284c6dd70bd020ce",
        (0x16400,0x16524):"89ca8b9e8be1c00d726d68a0339e4ee4906782e9189292f2ea24d1e5694ffc71",
        (0x1A1F4,0x1A21D):"fc11e3acbaeb44a13b97555470d0c4fe9985889f86cd70e62d695f131182bbdd",
        (0x1A5B0,0x1A724):"2c1e3cf2a887bdcda658487698fe113b34d487dda4038050bd69f0cca14427c6",
    }
    for (start,end),sha in ebp_bounded_spans.items():
        assert hashlib.sha256(obj[start:end]).hexdigest()==sha,hex(start)
    ebp_windows={
        0x1608C:"53565755",0x160E1:"bd01000000",
        0x160E8:"e807410000",0x161F7:"e804020000",
        0x161FC:"5557e815e0ffff",0x1621C:"e88f430000",
        0x16221:"312d50c00000",
        0x14218:"53565755",0x1425A:"bd202c0100",
        0x14304:"5d5f5e5bc3",
        0x16400:"53565755",0x16411:"31ed",
        0x16431:"0f8d6cfbffff",0x164BF:"0f85defaffff",
        0x1651F:"e977faffff",
        0x15FA3:"834424085083c504",
        0x1602C:"83c40c5d5f5e5bc3",
        0x1A1F4:"e86f560100",
        0x2F868:"565783ec50",
        0x2F86D:"833dc8f0000000",
        0x2F876:"ff15c8f00000",
        0x1A5B0:"535683ec08",
        0x1A6E8:"83c4085e5bc3",
        0x1A71E:"83c4085e5bc3",
    }
    for at,hex_code in ebp_windows.items():
        expected=bytes.fromhex(hex_code)
        assert obj[at:at+len(expected)]==expected,hex(at)
    ebp_calls={
        0x160E8:0x1A1F4, 0x1A1F4:0x2F868,
        0x161F7:0x16400, 0x161FE:0x14218,
        0x1621C:0x1A5B0, 0x14224:0x291B0,
    }
    for at,dest in ebp_calls.items():
        assert obj[at]==0xE8,hex(at)
        actual=at+5+struct.unpack_from("<i",obj,at+1)[0]
        assert actual==dest,(hex(at),hex(actual),hex(dest))
    ebp_tail_branches={
        0x16431:(6,0x15FA3),
        0x164BF:(6,0x15FA3),
        0x1651F:(5,0x15F9B),
    }
    for at,(length,dest) in ebp_tail_branches.items():
        if length==6:
            assert obj[at:at+2] in (b"\x0f\x8d",b"\x0f\x85")
            displacement=struct.unpack_from("<i",obj,at+2)[0]
        else:
            assert obj[at]==0xE9
            displacement=struct.unpack_from("<i",obj,at+1)[0]
        assert at+length+displacement==dest,(hex(at),hex(dest))
    print("ORIGINAL_PASS: 5 EBP source spans, 24 opcodes, 6 E8 edges, 3 cross-tail branches")
    print("BOUND: 14218 and 16400 locally preserve EBP; 16400 uses 1602C shared epilogue")
    print("OPEN: intervening callback/callee effects, especially 160E8->1A1F4->2F868->F0C8")
    # Event-loop 16034 is invoked after EBP=1 (160E1), and its
    # eligible-timer branch enters a 42-way CS-qualified event opcode
    # dispatcher. Authentication covers the source table and code
    # slices, NOT runtime relocation, EBP-preservation by callees,
    # script-handler effects, or any installed F0C8 callback.
    event_vm_spans={
        (0x1548C,0x15534):"4dae5ed51e8d92d1fb2bb4e8db756bcbfe758d2df07297cfae2bcb62100051f2",
        (0x15534,0x15575):"566ea62a969cf82269f6054e44d67a8588046d9ac0174f322c8b86b71bd4b0e6",
        (0x16034,0x1608A):"48e419ca3bcc773f65d5743742c125809d71011f7190a839c796f41e7f5d5827",
        (0x15534,0x15E38):"b9862833b59f84137c2092e26c7a7044f545d27c8737c8b3b82a198ea11f496a",
    }
    for (start,end),expected_sha in event_vm_spans.items():
        assert hashlib.sha256(obj[start:end]).hexdigest()==expected_sha,hex(start)
    event_vm_windows={
        0x15534:"5356",
        0x15536:"803d0181000000",
        0x15560:"663d2900",
        0x15564:"0f8724000000",
        0x1556A:"0fb7c0",
        0x1556D:"2eff24858c540100",
        0x16034:"535631db",
        0x16038:"83bb24400100ff743e",
        0x16041:"e8baa60100",
        0x16046:"3b832c4001007631",
        0x1604E:"8b83244001008b35ec7f0000a3ec7f0000",
        0x1605F:"e8d0f4ffff",
        0x16064:"85c074f7",
        0x1606E:"e88da60100",
        0x1607F:"83c30c",
        0x16082:"83fb3c75b1",
        0x16087:"5e5bc3",
    }
    for at,hex_code in event_vm_windows.items():
        expected=bytes.fromhex(hex_code)
        assert obj[at:at+len(expected)]==expected,hex(at)
    event_vm_calls={
        0x16041:0x30700, 0x1605F:0x15534, 0x1606E:0x30700,
        0x15D16:0x15534, 0x15E0C:0x15534, 0x15E2D:0x15534,
        0x160F1:0x16034, 0x16105:0x16034, 0x161DC:0x15534,
    }
    for at,target in event_vm_calls.items():
        assert obj[at]==0xE8,hex(at)
        assert at+5+struct.unpack_from("<i",obj,at+1)[0]==target,(hex(at),hex(target))
    # Actual original DWORD entries, before loader relocation or
    # any runtime executable mutation, not inferred function names.
    event_vm_table=struct.unpack_from("<42I",obj,0x1548C)
    assert len(set(event_vm_table))==42
    assert all(0x15534<=target<0x15E38 for target in event_vm_table)
    assert (event_vm_table[0],event_vm_table[27],event_vm_table[41])==(
        0x155D0,0x1553F,0x156F2)
    assert obj[0x16089]==0xC3
    # Part 12: source-bounded full handler-range inventory, authenticated
    # by original object-1 identity + full slice hash above. The 48
    # machine-decoded near-E8 edges below cover the bounded VM region,
    # not transitive callees or dynamically installed callbacks.
    handler_calls={
        0x15689:0x146DC,0x156C7:0x14FC0,0x156E5:0x15050,0x1571F:0x2195C,0x15740:0xF540,0x1577C:0x141B8,
        0x15788:0x15168,0x15797:0x15168,0x157A4:0x1520C,0x157AE:0x15280,0x157DA:0x153FC,0x15819:0xF4B0,
        0x15850:0x1C1DC,0x15887:0x1C1DC,0x15934:0x1C124,0x15955:0x1C160,0x15976:0x1C1A0,0x15A47:0x30700,
        0x15A8A:0x291B0,0x15ACB:0x2E510,0x15AF6:0x296C0,0x15B06:0x291B0,0x15B42:0x297D0,0x15B58:0x296C0,
        0x15B7D:0x291B0,0x15BA2:0x2F29C,0x15BAD:0x2E538,0x15BE9:0x2E510,0x15BF2:0x2E538,0x15C0E:0x291B0,
        0x15C25:0x30CA2,0x15C64:0x2E510,0x15C80:0x291B0,0x15C88:0x15320,0x15C9C:0x1A510,0x15CAB:0x1A9F8,
        0x15CC6:0x1A8C8,0x15CDF:0x2E614,0x15CE7:0x2E608,0x15D16:0x15534,0x15D6D:0x27F24,0x15D87:0x28EF0,
        0x15D8E:0x29008,0x15D98:0x28F58,0x15DBD:0x28E58,0x15DDE:0x13EF8,0x15E0C:0x15534,0x15E2D:0x15534,
    }
    assert len(handler_calls)==48 and len(set(handler_calls.values()))==36
    for at,target in handler_calls.items():
        assert 0x15534<=at<0x15E38 and obj[at]==0xE8,hex(at)
        assert at+5+struct.unpack_from("<i",obj,at+1)[0]==target,(hex(at),hex(target))
    # Opcode 17 in the original 42-way CS jump table enters 15C78.
    # Its normal branch calls 1A510, flips exactly BYTE[C050] and
    # enters 1A9F8. The alternate branch calls 1A8C8 *without*
    # the direct C050 flip at 15CA4.
    assert event_vm_table[17]==0x15C78
    assert hashlib.sha256(obj[0x15C78:0x15CF1]).hexdigest()==(
        "ddc8a55faceaafbe35e95b228f5905ff269ef22b25fbf3f6db976059c313c2a2")
    # Original 1A8C8 uses EBP as an internal geometry register, but
    # locally saves at entry and restores in BOTH source return tails.
    assert hashlib.sha256(obj[0x1A8C8:0x1A9F8]).hexdigest()==(
        "8f3572ad75c478d56df8d82236281a79e701a8ff566d4820a4d3f3b15f91be14")
    handler_windows={
        0x15C78:"0fb60550c00000",
        0x15C8D:"803d0281000000",
        0x15C94:"751c",
        0x15C96:"ff3550c00000",
        0x15C9C:"e86f480000",
        0x15CA4:"803550c0000001",
        0x15CAB:"e8484d0000",
        0x15CB0:"eb1c",
        0x15CC0:"ff3550c00000",
        0x15CC6:"e8fd4b0000",
        0x15CCE:"803db87f000000",
        0x15CD5:"0f84b3f8ffff",
        0x1A8C8:"53565755",
        0x1A8F5:"0fbf2d319e0000",
        0x1A9D0:"5d5f5e5bc3",
        0x1A9F3:"5d5f5e5bc3",
    }
    for at,hex_op in handler_windows.items():
        want=bytes.fromhex(hex_op)
        assert obj[at:at+len(want)]==want,hex(at)
    # An exhaustive literal DWORD-operand scan inside the full decoded
    # script handler region, distinct from all indirect alias paths.
    body=obj[0x15534:0x15E38]
    reference_counts={0xC050:8,0xF0C8:0,0xF0CC:0}
    for address,count in reference_counts.items():
        literal=struct.pack("<I",address)
        occurrences=[i for i in range(len(body)-3)
                     if body[i:i+4]==literal]
        assert len(occurrences)==count,(hex(address),len(occurrences))
    # Opcode 17 two branches: original source-bounded graphics helpers.
    # Whole-body hashes, all E8 targets in the two main functions,
    # VGA port/ES handling and return boundaries. This does not close
    # transitive 1A288/1AB20/1AACC/2F580/2ABF4 call effects beyond
    # the ranges explicitly checked below or installed F0C8 callbacks.
    script17_callee_spans={
        (0x1A510,0x1A5AF):"dd5abbfef05856c37d88ab05dc7f5eea15b32c60f30b29c00b5cd89388c69285",
        (0x1A9F8,0x1AACA):"c9d143ff9177c930a5e445579592dfdbfebeaca20b9014376d136d47152c2fda",
        (0x2A020,0x2A075):"92ec3e0e031e104f07307ca637d5e8af1a71e0fb82f3235dd6f8e5bff51b62cb",
        (0x30CD4,0x30D26):"0b08ba8947ac2925a428bfd02ead86299e66a7a9984a7ec7f82313c25e97cd67",
        (0x291C8,0x291E0):"f9647dbf90774d4c72f0632e4863ebeca61f37d8bd4878fe294d99fd01be1ca0",
        (0x2E590,0x2E5D3):"34c0b454e84e9a9b0b042daefdd2291b6bc0b2397cd11e3198d7e4e672b45bc2",
        (0x2E5D4,0x2E608):"1001144b2e8fbc27822c7c98e07036b27427ec8ac4e4c7ec48b0d55d76493188",
        (0x291B0,0x291C8):"2be56f0929c1cc8a7c662b0c07d7d67bdd75ca3a115135dea8c8ff040e40042b",
        (0x29FFC,0x2A01D):"2efd01db2afcd628ff75c097eb72c6a2a5f1c4df7a52c85903604e9a4351b2f4",
        (0x2ABF4,0x2AC10):"ace61f68b80924da7c40f774c3b95f311b9ed16fd00bdaf7771441099617c7a5",
        (0x2ABC4,0x2ABDB):"3fe94500d0abf6fb8022bc87bd0e669d2fba910f9cab9c38041f1dd86ddc1f2b",
    }
    for (start,end),sha in script17_callee_spans.items():
        assert obj[end-1]==0xC3,hex(end)
        assert hashlib.sha256(obj[start:end]).hexdigest()==sha,hex(start)
        # Literal address census within eleven bounded functions;
        # this does NOT detect any computed indirect writes.
        for address in (0xC050,0xF0C8,0xF0CC):
            assert obj[start:end].count(struct.pack("<I",address))==0,(
                hex(start),hex(address))
    script17_callee_calls={
        0x1A55B:0x1A288,0x1A564:0x291C8,0x1A57C:0x1AB20,
        0x1A59C:0x291C8,
        0x1AA2F:0x2E590,0x1AA42:0x2A020,0x1AA76:0x30CD4,
        0x1AA7E:0x1AACC,0x1AA83:0x2E5D4,0x1AAA8:0x2E590,
        0x1AABB:0x2A020,0x1AAC3:0x2E5D4,
        0x291CE:0x29FFC,0x2E5A1:0x2F580,0x2E5CA:0x291B0,
        0x2E5E2:0x2F580,0x2E5FF:0x291B0,
        0x291B6:0x29FEC,0x2A014:0x2ABF4,0x2AC08:0x2ABC4,
    }
    assert len(script17_callee_calls)==20
    for at,target in script17_callee_calls.items():
        assert obj[at]==0xE8 and (
            at+5+struct.unpack_from("<i",obj,at+1)[0])==target,hex(at)
    for start,end,count in ((0x1A510,0x1A5AF,4),
                            (0x1A9F8,0x1AACA,8),
                            (0x2A020,0x2A075,0),
                            (0x30CD4,0x30D26,0)):
        found=[i for i in range(start,end) if obj[i]==0xE8]
        expected=[i for i in script17_callee_calls if start<=i<end]
        assert found==expected and len(found)==count,hex(start)
    script17_callee_windows={
        0x1A510:"5383ec08",0x1A520:"7476",0x1A52C:"7c6a",
        0x1A55B:"e828fdffff",0x1A57C:"e89f050000",
        0x1A598:"0fb6c3",0x1A5AA:"83c4085bc3",
        0x1A9F8:"53",0x1AA01:"0f8499000000",
        0x1AA11:"0f8c89000000",
        0x1AA2F:"e85c3b0100",0x1AA42:"e8d9f50000",
        0x1AA76:"e859620100",0x1AA7E:"e849000000",
        0x1AA83:"e84c3b0100",0x1AAA8:"e8e33a0100",
        0x1AABB:"e860f50000",0x1AAC3:"e80c3b0100",
        0x1AA9E:"5bc3",0x1AAC8:"5bc3",
        0x2A020:"c8000000",0x2A024:"5657525153",
        0x2A062:"f3a4",0x2A06E:"5b595a5f5ec9c3",
        0x30CF6:"8cda068ec2",0x30D0A:"07fc5f5ec3",
        0x30D0F:"8cda89c7068ec2",0x30D22:"075f5ec3",
        0x30D23:"5f5ec3",
        0x291C8:"0fb6442404",0x291DA:"a24eef0000",
        0x291B0:"0fb6442404",0x291C2:"a24def0000",
        0x29FFC:"c8000000",0x2A014:"e8db0b0000",
        0x2A01B:"c9c3",0x2ABF4:"5253fa",
        0x2ABF7:"66bad403",0x2ABFD:"b00c66ef",
        0x2AC03:"fec066ef",0x2AC07:"fb",
        0x2ABC8:"66bada03",
        0x2ABCD:"a80875fb",0x2ABD2:"a80874fb",
    }
    assert len(script17_callee_windows)==44
    for at,encoded in script17_callee_windows.items():
        want=bytes.fromhex(encoded)
        assert obj[at:at+len(want)]==want,hex(at)
    # Script opcode 17 deeper graphics-state paths. These hashes attest
    # independently decoded original code slices, not transitive effects.
    # 1AACC and 1AB20 have no conventional local frame; they share
    # RET 1AAC9 and JMP to the page-stack unwind helper 2E5D4.
    nested_graphics_spans={
        (0x1AACC,0x1AB20):"07d5112274f40c7e4ef49894bb580a0b02fcb7137bea839b906cd8ce4e5fc0a8",
        (0x1AB20,0x1AC0C):"07ba01f3f70cee6963f2a227ffb3aa7a4d2a0fd85a945c886cf920262b868fc6",
        (0x2F580,0x2F5F1):"f72f1a2dff71df4d206f4dda95e6cedcd2e57170661f72edcdf9a01d776f384f",
        (0x2F5F4,0x2F839):"850f11e768b152840c16337ebe8677ef4b034907ae4fa841e4cbafa6a7201748",
        (0x1A3B8,0x1A50D):"706fe1969bb08173d5330e638e869151a9cc52b36c52a448d6add09c1ccd2774",
    }
    for (start,end),expected in nested_graphics_spans.items():
        assert hashlib.sha256(obj[start:end]).hexdigest()==expected,hex(start)
        for address in (0xC050,0xF0C8,0xF0CC):
            assert struct.pack("<I",address) not in obj[start:end],(
                hex(start),hex(address))
    nested_graphics_calls={
        0x1AAE7:0x2E590,0x1AB13:0x2932C,
        0x1AB3B:0x2E590,0x1AB5F:0x2932C,
        0x1ABAD:0x29804,0x1ABF7:0x29804,0x1ABFF:0x2E5D4,
        0x2F59F:0x2F5F4,0x2F5AE:0x290F4,0x2F5B6:0x2F4E0,
        0x2F5C7:0x290F4,0x2F5CF:0x2F4E0,
        0x2F5D8:0x311E8,0x2F5E0:0x376F0,0x2F5E7:0x352BE,
        0x2F61B:0x3058C,0x2F624:0x30594,
        0x2F652:0x291B0,0x2F667:0x291B0,0x2F686:0x3104C,
        0x2F6A5:0x292BC,0x2F6BE:0x29CEC,0x2F6EC:0x30288,
        0x2F707:0x304F4,0x2F719:0x303C8,0x2F75B:0x2A104,
        0x2F770:0x2E614,0x2F77C:0x30AEC,0x2F79E:0x30AEC,
        0x2F7C1:0x2932C,0x2F7CA:0x310B0,0x2F7D8:0x291B0,
        0x2F7E7:0x304F4,0x2F811:0x2A104,0x2F827:0x311E8,
        0x1A43F:0x33C90,0x1A4AD:0x33C90,0x1A4DC:0x1F820,
    }
    assert len(nested_graphics_calls)==38
    for at,target in nested_graphics_calls.items():
        assert obj[at]==0xE8,hex(at)
        assert at+5+struct.unpack_from("<i",obj,at+1)[0]==target,hex(at)
    # Literal opcode scans produce two false E8 call candidates:
    # each lies INSIDE the displacement of a decoded E8 instruction.
    assert obj[0x1AB15]==obj[0x2F708]==0xE8
    assert 0x1AB15 not in nested_graphics_calls
    assert 0x2F708 not in nested_graphics_calls
    nested_graphics_branches={
        0x1AAD4:(2,0x1AAC9),0x1AADD:(2,0x1AAC9),
        0x1AB28:(2,0x1AAC9),0x1AB31:(2,0x1AAC9),
        0x1AB1B:(5,0x2E5D4),0x1AB67:(5,0x2E5D4),
    }
    for at,(length,target) in nested_graphics_branches.items():
        if length==2:
            assert obj[at] in (0x74,0x75)
            displacement=struct.unpack_from("<b",obj,at+1)[0]
        else:
            assert obj[at]==0xE9
            displacement=struct.unpack_from("<i",obj,at+1)[0]
        assert at+length+displacement==target,hex(at)
    nested_graphics_windows={
        0x1AAC8:"5bc3",0x1AAC9:"c3",
        0x1AACC:"66833dfc85000000",0x1AAD6:"66ff0d379e0000",
        0x1AADD:"75ea",0x1AB1B:"e9b43a0100",
        0x1AB20:"66833dfc85000000",0x1AB2A:"66ff0d379e0000",
        0x1AB31:"7596",0x1AB67:"e9683a0100",
        0x1AC04:"66ff05379e0000",0x1AC0B:"c3",
        0x2F580:"53",0x2F59F:"e850000000",
        0x2F5EF:"5bc3",0x2F5F4:"53565755",
        0x2F6A3:"89c5",0x2F81B:"83c4105d5f5e5bc3",
        0x2F831:"83c4105d5f5e5bc3",
        0x1A3B8:"5356",0x1A50A:"5e5bc3",
    }
    assert len(nested_graphics_windows)==21
    for at,signature in nested_graphics_windows.items():
        wanted=bytes.fromhex(signature)
        assert obj[at:at+len(wanted)]==wanted,hex(at)
    print("ORIGINAL_PASS: 5 nested source spans, 38 instruction-bound E8 edges")
    print("ORIGINAL_PASS: 6 shared-tail branches, 21 opcode windows")
    print("BOUND: 2F5F4 restores EBP at 2F81E/2F834; nested callees remain open")
    print("ORIGINAL_PASS: opcode 17: 11 bounded helper bodies, 20 E8 edges, 44 opcode windows")
    print("BOUND: distinct EF4D drawing/EF4E display page; CRTC 3D4 and retrace 3DA")
    print("BOUND: 30CD4 restores ES on changed paths; deeper handlers and F0C8 open")
    print("ORIGINAL_PASS: 42 opcode handlers, 48 E8 edges, 36 distinct callees")
    print("ORIGINAL_PASS: opcode 17 conditional BYTE C050 toggle, 1A8C8 EBP restores")
    print("BOUND: 8 C050/0 F0C8/0 F0CC literal refs in 15534..15E37")
    print("OPEN: downstream handlers' register, graphics hook and VGA alias effects")
    print("ORIGINAL_PASS: timer event-loop and 42 original CS opcode targets")
    print("ORIGINAL_PASS: four source hashes, 17 op windows and nine near-E8 edges")
    print("OPEN: VM handler callees, EBP/DS/ES effects and runtime callback target")
    print("ORIGINAL_PASS: original CODE.2 SHA and C050=1 initializer")
    print("ORIGINAL_PASS: 339 C050 operands: 282 reads, 56 byte toggles, 1 DWORD EBP toggle")
    print("BOUND: EBP=1 at 160E1; original call preservation and aliases remain open")

def audit_repository()->None:
    cm=(ROOT/"DemoV1/CMakeLists.txt").read_text(encoding="utf-8")
    doc=(ROOT/"DemoV1/LEVEL22_ORIGINAL_VGA_C050_PAGE_TOGGLE_AUDIT_20261010.md").read_text(encoding="utf-8")
    runner=(ROOT/"DemoV1/tools/run_original_ground_proof_batch.py").read_text(encoding="utf-8")
    assert "NAME original_ground_c050_page_toggle_tests" in cm
    assert '"c050_page", "verify_original_ground_c050_page_toggle.py"' in runner
    for word in ("339","56","16221","160E1","FDBC","indirect","EBP"):
        assert word in doc,word
    for term in ("14218","16400","1602C","1A1F4","2F868","F0C8"):
        assert term in doc,term
    for term in ("15534","1548C","42-way","16034","7FEC","F0C8"):
        assert term in doc,term
    for term in ("48 direct E8","opcode 17","1A8C8","15CA4","36 distinct","F0CC"):
        assert term in doc,term
    for term in ("1A510","1A9F8","2A020","30CD4","2ABF4","2ABC4","44 opcode","EF4D","EF4E"):
        assert term in doc,term
    for term in ("1AACC","1AB20","2F580","2F5F4","1A3B8","1AAC9","38","2F81E","2F834"):
        assert term in doc,term
    print("SOURCE_ONLY_PASS: registered CTest, original ground batch and audit notes")

if __name__=="__main__":
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--original-code2",type=Path)
    p.add_argument("--binary-only",action="store_true")
    a=p.parse_args()
    if a.binary_only and a.original_code2 is None:
        p.error("--binary-only requires --original-code2")
    if not a.binary_only:audit_repository()
    if a.original_code2 is not None:audit_original(a.original_code2)
