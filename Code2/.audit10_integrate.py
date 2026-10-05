from pathlib import Path

a=Path('Code2/FINAL_SEMANTIC_AUDIT.md')
t=a.read_text()
marker='\n## Current result\n'
if marker not in t: raise SystemExit('missing current result')
head,_=t.split(marker,1)
cp='''

## Checkpoint 10 — `0A791..0D54F`

**11,711 bytes.** The enlarged audit span closes the alignment at `0A791..0A793`, all of reconstruction batches 16 and 17, and the first complete Batch-18 family through the four-entry CS table at `0D540..0D54F`. Fresh immutable-object span SHA-256: `d7be8e8c6d82f446f8c25b0e5a56f0bb20dd08974152b6ed0c8f09180a562262`. Mandatory overlap `[0x0A751,0x0A851)` SHA-256: `a1c238a0b3e5eda7fce02986f0d8f76a61eabf7443fe4076242071c3df620311`.

**PASS with no new semantic correction.** The CODE.1 defect checklist was applied across the complete ownership units, with particular attention to cdecl stack slots, signed index/range tests, rank-indexed stack aliases, two's-complement arithmetic, table ownership, and address-distinct exits. The Batch-16 family retains the out-of-bounds hard return in `0B02C`, inclusive world-record scanning through `258A2`, the moving-type-`0Bh` special path, and `0B410`'s exact two redraw pairs followed by one final page flip after sound `+21h`.

Batch 17 preserves `0B518`'s rank-indexed stack aliases, 32-bit two's-complement absolute-value behavior, and the encoded `E284` loop-counter argument. The 132-entry CS dword table at `0B740..0B94F` remains data owned by `0B950`; all command values `00h..83h` retain their encoded handler/default mapping, including the distinct no-selection returns and command-`58h` forced `FFFFFFFEh`. The formerly ambiguous `0C6D7..0C703` boundary is retained exactly as nine padding bytes followed by the nine-dword source table copied by `REP MOVSD` at `0CBD5`.

The Batch-18 portion through `0D54F` was rechecked for the high-risk arithmetic and class-selection paths. `0CD44` keeps countdown state distinct from its damage buffer, `0CF88` retains the encoded signed divide edge, `0D0B0` preserves class-table indexing, and `0D1E0` preserves random-tie behavior. The four dwords at `0D540..0D54F` are data owned by the next routine. No new normalization, signedness, partial-register, global-reload, or call-slot defect was found. The next executable frontier is `0D550`.
'''
cur='''
## Current result

- Audited object bytes: **54,592 / 239,698 = 22.7753%**
- Closed frontier: **`0D54F`**
- Next frontier: **`0D550`**
- Semantic defect classes corrected so far: **6**
- Object byte accounting remains **239,698 / 239,698 = 100.0000%** with **0 unresolved bytes**.
'''
a.write_text(head+cp+'\n'+cur)

p=Path('Code2/PROGRESS.md'); s=p.read_text()
s=s.replace('semantic%20audit-17.89%25-yellow','semantic%20audit-22.78%25-yellow')
s=s.replace('**Semantic audit:** **42,881 / 239,698 = 17.8896%**, closed through object offset `0x0A790`; **6 semantic defect classes** found so far.','**Semantic audit:** **54,592 / 239,698 = 22.7753%**, closed through object offset `0x0D54F`; **6 semantic defect classes** found so far.')
p.write_text(s)

r=Path('Code2/RESUME_CHECKPOINT.md'); q=r.read_text()
q += '''\n\n## Semantic audit checkpoint 10 handoff\n- Object accounting remains **239,698 / 239,698**.\n- Semantic audit: **54,592 / 239,698 = 22.7753%**, closed through **`0D54F`**.\n- Semantic defect classes remain **6**; checkpoint 10 found no new defect.\n- Span `0A791..0D54F` SHA-256: `d7be8e8c6d82f446f8c25b0e5a56f0bb20dd08974152b6ed0c8f09180a562262`.\n- Mandatory overlap `[0x0A751,0x0A851)` SHA-256: `a1c238a0b3e5eda7fce02986f0d8f76a61eabf7443fe4076242071c3df620311`.\n- Next semantic-audit frontier: **`0D550`**.\n'''
r.write_text(q)
