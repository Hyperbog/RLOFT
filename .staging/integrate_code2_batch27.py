from pathlib import Path
import base64,zlib,re

root=Path('.')
cpp_path=root/'Code2/Code2.cpp'
progress_path=root/'Code2/PROGRESS.md'
ledger_path=root/'Code2/RESIDUAL_LEDGER.md'
cont_path=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
staged=root/'.staging/code2_batch27_cpp.z'

cpp=cpp_path.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 89498u;' in cpp
assert 'constexpr u32 kVerifiedNonCodeBytes = 1650u;' in cpp
assert 'function_146dc' in cpp and 'static_assert(0x148EEu-0x146DCu==530u);' in cpp
assert 'function_148f0' not in cpp

fragment=zlib.decompress(base64.b64decode(staged.read_text().strip())).decode()
assert 'function_148f0' in fragment and 'function_153fc' in fragment

cpp=cpp.replace('constexpr u32 kVerifiedTranslatedBytes = 89498u;',
                'constexpr u32 kVerifiedTranslatedBytes = 92402u;',1)
cpp=cpp.replace('constexpr u32 kVerifiedNonCodeBytes = 1650u;',
                'constexpr u32 kVerifiedNonCodeBytes = 1718u;',1)
new_assert='''static_assert(kVerifiedTranslatedBytes == 89498u +\n              (0x14A5Eu-0x148F0u) + (0x14AB2u-0x14A60u) +\n              (0x14BDEu-0x14ADCu) + (0x14C3Du-0x14BE0u) +\n              (0x14CA3u-0x14C40u) + (0x14E99u-0x14CA4u) +\n              (0x14FBEu-0x14E9Cu) + (0x1504Du-0x14FC0u) +\n              (0x15143u-0x15050u) + (0x15168u-0x15144u) +\n              (0x1520Bu-0x15168u) + (0x1527Du-0x1520Cu) +\n              (0x1531Fu-0x15280u) + (0x1538Bu-0x15320u) +\n              (0x153F9u-0x1538Cu) + (0x1548Cu-0x153FCu));\n'''
cpp,n=re.subn(r'static_assert\(kVerifiedTranslatedBytes == .*?\);\n',new_assert,cpp,count=1,flags=re.S)
assert n==1
marker='\n} // namespace code2'
pos=cpp.rfind(marker)
assert pos!=-1
cpp=cpp[:pos]+'\n'+fragment.rstrip()+'\n'+cpp[pos:]
cpp_path.write_text(cpp)

progress=progress_path.read_text()
repls={
'object%20accounting-38.03%25-red':'object%20accounting-39.27%25-red',
'**Verified translated instruction bytes:** **89,498**':'**Verified translated instruction bytes:** **92,402**',
'**Proven non-code/data bytes:** **1,650**':'**Proven non-code/data bytes:** **1,718**',
'**Reconciled object bytes:** **91,148 / 239,698 = 38.0262% (38.03%)**':'**Reconciled object bytes:** **94,120 / 239,698 = 39.2661% (39.27%)**',
'**Unresolved object bytes:** **148,550**':'**Unresolved object bytes:** **145,578**',
}
for old,new in repls.items():
    assert old in progress,old
    progress=progress.replace(old,new,1)
progress_path.write_text(progress)

ledger=ledger_path.read_text()
assert '**Total proven non-code/data:** **1,650 bytes**.' in ledger
insert='''14A5E..14A5F | alignment | 2\n14AB2..14AB3 | alignment | 2\n14AB4..14ADB | table | 40\n14BDE..14BDF | alignment | 2\n14C3D..14C3F | alignment | 3\n14CA3..14CA3 | alignment | 1\n14E99..14E9B | alignment | 3\n14FBE..14FBF | alignment | 2\n1504D..1504F | alignment | 3\n15143..15143 | alignment | 1\n1520B..1520B | alignment | 1\n1527D..1527F | alignment | 3\n1531F..1531F | alignment | 1\n1538B..1538B | alignment | 1\n153F9..153FB | alignment | 3\n'''
end='<!-- RESIDUAL-RANGES-END -->'
assert end in ledger and '14AB4..14ADB' not in ledger
ledger=ledger.replace(end,insert+end,1)
ledger=ledger.replace('**Total proven non-code/data:** **1,650 bytes**.',
                      '**Total proven non-code/data:** **1,718 bytes**.',1)
ledger += '''\n\nBatch 27 evidence: `14AB4..14ADB` is the exact ten-entry CS dword table consumed by the indirect jump at `14B71`, targeting `14B79,14B87,14B95,14BA3,14BDA,14BDA,14BB1,14BDA,14BDA,14BBF`. The remaining Batch 27 ranges decode as bounded `MOV EAX,EAX`, `LEA EAX,[EAX+0]`, or `NOP` compiler separators between directly reconstructed routine boundaries. A full object-1 direct relative CALL/JMP/conditional-branch target scan found no incoming target to any Batch 27 separator byte. The next unresolved ownership unit begins with table/data at `1548C`; it is deliberately deferred to the dispatcher beginning at `15534`. The older zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
ledger_path.write_text(ledger)

cont=cont_path.read_text()
assert '## CODE.2 Batch 27 checkpoint' not in cont
cont += '''\n\n## CODE.2 Batch 27 checkpoint\n\nBatch 27 added **2,904 translated bytes** plus **68 proven non-code/data bytes**. Cumulative accounting is **92,402 translated + 1,718 non-code = 94,120 / 239,698 = 39.2661% (39.27%)**; unresolved is **145,578 bytes**.\n\nThe sequential accounting frontier now reaches `1548B`; the next unresolved ownership unit begins at `1548C`, with the owning dispatcher directly decoded at `15534`. Preserve `148F0`'s class-dependent activation side effects, the low-byte `0080h` test in `14A60` versus the distinct word `8000h` tests later in the batch, `14ADC`'s live-EBX second argument into `1C160`, `14E9C`'s asymmetric A6 handling, `15050`'s full-width item value until the encoded low-word store, `15280`'s second-pass encoded no-op flag clear, and `153FC`'s once-per-cursor recursive presence count versus multiplicity counting for party inventory. The table at `1548C` is not yet counted. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
cont_path.write_text(cont)

# Integrated invariants / accounting.
cpp=cpp_path.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 92402u;' in cpp
assert 'constexpr u32 kVerifiedNonCodeBytes = 1718u;' in cpp
for anchor in [
    'if(cls0==4||cls1==4||cls2==4)',
    'b.call_1c160(i+4u,off);',
    'if(item==-1||item==0xA6)continue;',
    '&0x8000u)!=0u)continue;',
    'if((token==0xFFFFu&&type==0x18u)||token==type)',
    'return load_le32(dg(m,0x14020u))==0x0Fu?1u:0u;',
]: assert anchor in cpp,anchor

# Sum ledger ranges exactly.
ledger=ledger_path.read_text()
body=ledger.split('<!-- RESIDUAL-RANGES-BEGIN -->',1)[1].split('<!-- RESIDUAL-RANGES-END -->',1)[0]
total=0
for line in body.splitlines():
    m=re.match(r'([0-9A-F]{5})\.\.([0-9A-F]{5}) \| [^|]+ \| (\d+)$',line.strip())
    if not m: continue
    a,b,n=int(m.group(1),16),int(m.group(2),16),int(m.group(3))
    assert b-a+1==n,(line,b-a+1,n)
    total+=n
assert total==1718,total
assert 92402+1718==94120
assert 239698-94120==145578
print('Batch 27 integration applied:',92402,1718,94120,145578)
