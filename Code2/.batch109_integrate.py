from pathlib import Path
import os
R=Path('Code2');C=R/'Code2.cpp';P=R/'PROGRESS.md';A=R/'CODE2_CONTINUITY_ADDENDUM.md';K=R/'RESUME_CHECKPOINT.md'
def norm(s):return s.rstrip()+"\n"
def integrate():
 c=C.read_text();assert 'constexpr u32 kVerifiedTranslatedBytes = 219172u;' in c and '// Batch 109:' in c
 old='const u8 resource_index=*dg(m,record+9u);\n    if(!alternate){'
 new='const u8 resource_index=*dg(m,record+9u);\n    const u32 resource_id=load_le32(dg(m,0x258F8u))+static_cast<u32>(resource_index);\n    if(!alternate){'
 assert c.count(old)==1;c=c.replace(old,new,1)
 bad='b.call_2f29c?b.call_2f29c(0x258F8u+resource_index,4u):0u'
 good='b.call_2f29c?b.call_2f29c(resource_id,4u):0u'
 assert c.count(bad)==2;c=c.replace(bad,good)
 c=c.replace('first argument is 258F8h + record byte 9','first argument is [258F8h] + record byte 9',1)
 C.write_text(norm(c))
 p=P.read_text();assert '`258F8h + record[9]`' in p;p=p.replace('`258F8h + record[9]`','`[258F8h] + record[9]`',1);P.write_text(norm(p))
 a=A.read_text();assert 'exact `FDC0/FDB8/FDB4` writes' in a;a=a.replace('exact `FDC0/FDB8/FDB4` writes','exact `[258F8h] + record[9]` resource-id load, `FDC0/FDB8/FDB4` writes',1);A.write_text(norm(a))
def verify():
 c=C.read_text();p=P.read_text();a=A.read_text();assert 'constexpr u32 kVerifiedTranslatedBytes = 219172u;' in c and 'constexpr u32 kVerifiedNonCodeBytes = 4320u;' in c;assert 'const u32 resource_id=load_le32(dg(m,0x258F8u))+static_cast<u32>(resource_index);' in c and c.count('b.call_2f29c?b.call_2f29c(resource_id,4u):0u')==2 and '0x258F8u+resource_index' not in c;assert '`[258F8h] + record[9]`' in p and '`[258F8h] + record[9]`' in a;assert '223,492 / 239,698 = 93.2390% (93.24%)' in p
def checkpoint():
 sha=os.environ['BATCH_SHA'];K.write_text(norm(f'''# CODE.2 resume checkpoint\n\nDurable handoff anchor; live repository/binary evidence wins.\n\n## Last committed substantive decompilation state\n- Last substantive batch commit: `{sha}` — `Correct CODE.2 batch 109 resource-base load`.\n- Verified translated instruction bytes: **219,172**.\n- Proven non-code/data bytes: **4,320**.\n- Reconciled object bytes: **223,492 / 239,698 = 93.2390% (93.24%)**.\n- Unresolved object bytes: **16,206**.\n- Last integrated ownership fold: `34BB0..34DE7`.\n- Next independently decoded instruction boundary: **`34DE8`**.\n\n## Immutable identity\n- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.\n- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.\n\n## Mandatory overlap anchor for next resume\n- Window `[0x34D90,0x34E70)` — 224 bytes; SHA-256 `2b5c55b74f092f6004d88d2d7306013649306b4c1ed8b27a9c8492b4b0372beb`.\n- Narrow `[0x34DC0,0x34E20)` — 96 bytes; SHA-256 `9d1b8717c5be48625d41181b24587fdb1f8dc1a5d9706c69482cd6fedc25011c`.\n- Narrow bytes: `0002c705b8fd000004000000c705b4fd0000830000008d45d450e88df0ffff83c404c95f5e5bc3905356575589e581ec4c00000068800000006a00b8f0b7020050e82abfffff83c40c68800000006a00b870b8020050e815bfffff83c40c6a20`.\n\nInterpretation: `34DE6` is the Batch-109 RET; `34DE7` is an unreferenced NOP separator; `34DE8` begins the next independently called routine. Batch 109 accounting is unchanged by the resource-base fidelity repair. No Batch 110 bytes are counted.\n'''))
if __name__=='__main__':import sys;globals()[sys.argv[1]]()
