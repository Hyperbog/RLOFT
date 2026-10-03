from pathlib import Path

cpp_path=Path('Code2/Code2.cpp')
progress_path=Path('Code2/PROGRESS.md')
ledger_path=Path('Code2/RESIDUAL_LEDGER.md')
continuity_path=Path('Code2/CODE2_CONTINUITY_ADDENDUM.md')

cpp=cpp_path.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 8632u;' in cpp
assert 'constexpr u32 kVerifiedNonCodeBytes = 58u;' in cpp
assert 'function_006c0' not in cpp
frag=Path('Code2/batch03_part0.inc').read_text()+Path('Code2/batch03_part1.inc').read_text()
assert 'function_009b0' in frag and 'function_010c4' in frag
assert 'else if(load_le16(dg(m,0x22246u+off))==0xADu)' in frag
marker='\n} // namespace code2\n'
assert cpp.count(marker)==1
cpp=cpp.replace('constexpr u32 kVerifiedTranslatedBytes = 8632u;','constexpr u32 kVerifiedTranslatedBytes = 11405u;',1)
cpp=cpp.replace('constexpr u32 kVerifiedNonCodeBytes = 58u;','constexpr u32 kVerifiedNonCodeBytes = 77u;',1)
tail='              (0x2AA9Bu-0x2A8A0u));'
new_tail='''              (0x2AA9Bu-0x2A8A0u) +
              (0x006FDu-0x006C0u) + (0x00792u-0x00700u) +
              (0x007E2u-0x00794u) + (0x00839u-0x007E4u) +
              (0x00931u-0x0083Cu) + (0x009B0u-0x00934u) +
              (0x00B6Cu-0x009B0u) + (0x00BFAu-0x00B6Cu) +
              (0x00CB0u-0x00BFCu) + (0x010C3u-0x00CB0u) +
              (0x011A5u-0x010C4u));'''
assert cpp.count(tail)==1
cpp=cpp.replace(tail,new_tail,1)
cpp=cpp.replace(marker,'\n'+frag+'\n'+marker,1)
cpp_path.write_text(cpp)

p=progress_path.read_text()
repls={
'object%20accounting-3.63%25-red':'object%20accounting-4.79%25-red',
'**Verified translated instruction bytes:** **8,632**':'**Verified translated instruction bytes:** **11,405**',
'**Proven non-code/data bytes:** **58**':'**Proven non-code/data bytes:** **77**',
'**Reconciled object bytes:** **8,690 / 239,698 = 3.6254% (3.63%)**':'**Reconciled object bytes:** **11,482 / 239,698 = 4.7902% (4.79%)**',
'**Unresolved object bytes:** **231,008**':'**Unresolved object bytes:** **228,216**'}
for old,new in repls.items():
    assert old in p,old
    p=p.replace(old,new,1)
assert '## Batch 03 — map/search/render application block' not in p
p += '''\n\n## Batch 03 — map/search/render application block\n\nSource identity and object-1 SHA-256 were reverified before this batch. Added **2,773 translated instruction bytes** and **19 directly proven alignment bytes**, bringing cumulative accounting to **11,405 translated + 77 non-code = 11,482 / 239,698 = 4.7902% (4.79%)**.\n\n### Newly translated ranges\n\n- `006C0..006FC` — **61 bytes**: signed camera-relative coordinate derivation; stored values clamp at `-40`, while EAX retains the unclamped second coordinate.\n- `00700..00791` — **146 bytes**: dual primary/alternate 22-byte-record search selected by `5FCB`, with signed comparisons and exact `6048` snapshot/writeback.\n- `00794..007E1` — **78 bytes**: gated search over the `1761A/1761E` record set.\n- `007E4..00838` — **85 bytes**: type-`54h` primary-record search.\n- `0083C..00930` — **245 bytes**: filtered search through `2ACDC`, `F03C/F03E`, table-bit `40h`, and the encoded +/-10 coordinate test.\n- `00934..009AF` — **124 bytes**: alternate `2AD18`/`2ADE0` predicate search.\n- `009B0..00B6B` — **444 bytes**: four-neighbor tile-shape classifier. Direct audit corrected a tempting normalization: kinds 2/3/4 fall back to the raw neighbor mask; only kind 1 and the ordinary kind-5 path consult `5FF0`.\n- `00B6C..00BF9` — **142 bytes**: tile draw helper with partial-BX palette index semantics and temporary `764D/7651` palette-byte installation/restoration.\n- `00BFC..00CAF` — **180 bytes**: fixed-point eight-step marker/line helper feeding the already-integrated `29CEC` line rasterizer.\n- `00CB0..010C2` — **1,043 bytes**: multi-family map-record renderer. It preserves live `5FCB` layout selection, address-distinct search families, repeated `258DC` reloads across opaque draws, and the asymmetric alternate-record type tests encoded by the binary.\n- `010C4..011A4` — **225 bytes**: eight-call fixed UI-frame decoration routine with `258DC` reloaded before every draw.\n\n### Newly proven non-code/data\n\nThe explicit no-op separators `006FD..006FF`, `00792..00793`, `007E2..007E3`, `00839..0083B`, `00931..00933`, `00BFA..00BFB`, `010C3`, and `011A5..011A7` total **19 bytes**. Their instruction forms were decoded directly and a full-object direct branch/call target scan found no incoming control-flow references. They are recorded in `Code2/RESIDUAL_LEDGER.md`.\n\n`006B9..006BF` remains unresolved: it is still only a zero-filled island, not proven alignment/data.\n\n### Next high-value targets\n\n1. Continue the application path at `011A8`.\n2. Reconcile startup at `353A0` as a CODE.2-specific path.\n3. Continue independently proving relocated CODE.1 runtime/graphics families where complete CODE.2 boundaries are available.\n'''
progress_path.write_text(p)

led=ledger_path.read_text()
insert='''006FD..006FF | alignment | 3
00792..00793 | alignment | 2
007E2..007E3 | alignment | 2
00839..0083B | alignment | 3
00931..00933 | alignment | 3
00BFA..00BFB | alignment | 2
010C3..010C3 | alignment | 1
011A5..011A7 | alignment | 3
'''
end='<!-- RESIDUAL-RANGES-END -->'
assert led.count(end)==1 and '006FD..006FF' not in led
led=led.replace(end,insert+end,1)
assert '**Total proven non-code/data:** **58 bytes**.' in led
led=led.replace('**Total proven non-code/data:** **58 bytes**.','**Total proven non-code/data:** **77 bytes**.',1)
led += '''\nBatch 03 evidence: each added range decodes as an explicit compiler no-op separator between directly reconstructed functions. A full CODE.2 object-1 scan of direct relative `CALL`/`JMP`/conditional-branch targets found no incoming target to any of these separator instructions. `006B9..006BF` remains unresolved.\n'''
ledger_path.write_text(led)

c=continuity_path.read_text()
assert '## CODE.2 Batch 03 checkpoint' not in c
c += '''\n\n## CODE.2 Batch 03 checkpoint\n\nBatch 03 added **2,773 translated bytes** plus **19 proven alignment bytes**. Cumulative accounting is **11,405 translated + 77 non-code = 11,482 / 239,698 = 4.7902% (4.79%)**; unresolved is **228,216 bytes**.\n\nThe application path is now reconstructed through `011A4`; next code begins at `011A8`. High-risk details already reconciled include the `009B0` raw-mask fallbacks for tile kinds 2/3/4, partial-BX palette extraction in `00B6C`, fixed-point overlapping locals in `00BFC`, repeated `258DC` reloads, and the alternate-layout renderer's asymmetric type source (`6048+off+0E` for the `3C..43` range but `22246+off` for the `AD` test). Keep `006B9..006BF` unresolved until independently proven.\n'''
continuity_path.write_text(c)

assert sum([61,146,78,85,245,124,444,142,180,1043,225])==2773
assert 8632+2773==11405
assert 58+19==77
assert 11405+77+228216==239698
