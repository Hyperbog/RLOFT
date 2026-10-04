from pathlib import Path
import re,zlib,base64
root=Path('.')
cpp=root/'Code2/Code2.cpp'
progress=root/'Code2/PROGRESS.md'
ledger=root/'Code2/RESIDUAL_LEDGER.md'
addendum=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
chunk=zlib.decompress(base64.b64decode((root/'Code2/.batch28_source.b64').read_text())).decode()

s=cpp.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 92402u;' in s
assert 'constexpr u32 kVerifiedNonCodeBytes = 1718u;' in s
assert 'static_assert(0x1548Cu-0x153FCu==144u);' in s
assert 'function_15534' not in s
s=s.replace('constexpr u32 kVerifiedTranslatedBytes = 92402u;','constexpr u32 kVerifiedTranslatedBytes = 94703u;',1)
s=s.replace('constexpr u32 kVerifiedNonCodeBytes = 1718u;','constexpr u32 kVerifiedNonCodeBytes = 1893u;',1)
pat=re.compile(r'static_assert\(kVerifiedTranslatedBytes == 89498u \+\n(?:.*\n)*?\s*\(0x153F9u-0x1538Cu\) \+ \(0x1548Cu-0x153FCu\)\);')
m=pat.search(s)
assert m, 'Batch 27 accounting anchor not found'
new=('static_assert(kVerifiedTranslatedBytes == 92402u +\n'
     '              (0x15DF5u-0x15534u) + (0x15E21u-0x15DF8u) +\n'
     '              (0x15E37u-0x15E24u));')
s=s[:m.start()]+new+s[m.end():]
end='\n} // namespace code2\n'
assert s.endswith(end)
s=s[:-len(end)]+'\n'+chunk+end
cpp.write_text(s)

p=progress.read_text()
repls={
'object%20accounting-39.27%25-red':'object%20accounting-40.30%25-red',
'**Verified translated instruction bytes:** **92,402**':'**Verified translated instruction bytes:** **94,703**',
'**Proven non-code/data bytes:** **1,718**':'**Proven non-code/data bytes:** **1,893**',
'**Reconciled object bytes:** **94,120 / 239,698 = 39.2661% (39.27%)**':'**Reconciled object bytes:** **96,596 / 239,698 = 40.2990% (40.30%)**',
'**Unresolved object bytes:** **145,578**':'**Unresolved object bytes:** **143,102**',
}
for a,b in repls.items():
    assert a in p,a
    p=p.replace(a,b,1)
progress.write_text(p)

l=ledger.read_text()
marker='<!-- RESIDUAL-RANGES-END -->'
assert marker in l
insert=('1548C..15533 | table | 168\n'
        '15DF5..15DF7 | alignment | 3\n'
        '15E21..15E23 | alignment | 3\n'
        '15E37..15E37 | alignment | 1\n')
assert '1548C..15533' not in l
l=l.replace(marker,insert+marker,1)
l=l.replace('**Total proven non-code/data:** **1,718 bytes**.','**Total proven non-code/data:** **1,893 bytes**.',1)
l += ('\n\nBatch 28 evidence: `1548C..15533` is the exact 42-entry CS dword table indexed by the '
      'indirect jump at `1556D`; all opcode targets `00h..29h` were decoded and reconciled in '
      '`function_15534`. `15DF5..15DF7` and `15E21..15E23` decode as `LEA EAX,[EAX+0]`, '
      'and `15E37` is `NOP`; a full object-1 direct relative branch/call target scan found no '
      'incoming target to any separator byte. The known zero islands remain unresolved.\n')
ledger.write_text(l)

a=addendum.read_text()
a += '''\n\n## CODE.2 Batch 28 checkpoint\n\nBatch 28 added **2,301 translated bytes** plus **175 proven non-code/data bytes**. Cumulative accounting is **94,703 translated + 1,893 non-code = 96,596 / 239,698 = 40.2990% (40.30%)**; unresolved is **143,102 bytes**.\n\nThe sequential frontier now reaches `15E37`; `15E37` is proven `NOP` alignment and the next routine begins at `15E38`. Preserve `15534`'s exact 42-way `00h..29h` CS dispatch, opcode `0Bh`'s live-EBX second argument into `1C160`, opcode `08h`'s distinct `FFFFh` fifteen-item condition, zero-extended item words passed to `0F4B0/0F540`, opcode `0Fh`'s five 12-byte timer slots and `FFFFh -> FFFFFFFFh` disable conversion, opcode `12h`'s nested interpreter with `7FB8` gating and stream-position restore, opcode `23h`'s unsigned low-32 multiply plus logical `>>15` scale, and opcode `24h`'s forced return value `3` after `13EF8`. `1548C..15533` is now proven table data. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
addendum.write_text(a)
