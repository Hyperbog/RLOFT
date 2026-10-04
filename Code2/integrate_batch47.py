from pathlib import Path

root=Path(__file__).resolve().parent.parent
cpp=root/'Code2'/'Code2.cpp'
ledger=root/'Code2'/'RESIDUAL_LEDGER.md'
progress=root/'Code2'/'PROGRESS.md'
continuity=root/'Code2'/'CODE2_CONTINUITY_ADDENDUM.md'
staging=root/'Code2'/'batch47.staging.cpp'

c=cpp.read_text()
s=staging.read_text()
l=ledger.read_text()
p=progress.read_text()
a=continuity.read_text()

assert 'constexpr u32 kVerifiedTranslatedBytes = 138937u;' in c
assert 'constexpr u32 kVerifiedNonCodeBytes = 2667u;' in c
assert 'function_20e08' not in c
assert c.rstrip().endswith('} // namespace code2')

# Final reconciliation corrections made after direct instruction audit.
s=s.replace('if(entry==static_cast<s32>(kind)) {','if(static_cast<u32>(entry)==kind) {')
s=s.replace('const s32 count=static_cast<u32>(*dg(m,desc));','const s32 count=static_cast<s32>(*dg(m,desc));')
s=s.replace('store_le16(dg(m,0x2193Cu+actor_off+slot*2u),load_le16(dg(m,0x8600u)));',
            'store_le16(dg(m,0x2193Cu+actor_off+slot*2u),static_cast<u16>(current));')
s=s.replace('        if(type!=1u)return;\n        const s8 subtype=static_cast<s8>(*dg(m,env_base+env_off+0x31u));\n',
            '        const u32 env_base_after=load_le32(dg(m,0x255D8u));\n'
            '        const u32 type_after=static_cast<u32>(*dg(m,env_base_after+env_off+0x2Fu));\n'
            '        if(type_after!=1u)return;\n'
            '        const s8 subtype=static_cast<s8>(*dg(m,env_base_after+env_off+0x31u));\n')
assert 'env_base_after' in s

c=c.replace('constexpr u32 kVerifiedTranslatedBytes = 138937u;',
            'constexpr u32 kVerifiedTranslatedBytes = 141234u;')
c=c.replace('constexpr u32 kVerifiedNonCodeBytes = 2667u;',
            'constexpr u32 kVerifiedNonCodeBytes = 2670u;')
old_assert='''static_assert(kVerifiedTranslatedBytes == 136130u +
              (0x2043Eu-0x202F0u) + (0x205B4u-0x20440u) +
              (0x20607u-0x205B4u) + (0x207D1u-0x20608u) +
              (0x2084Eu-0x207E8u) + (0x20993u-0x20850u) +
              (0x20A22u-0x20994u) + (0x20B40u-0x20A24u) +
              (0x20C0Cu-0x20B40u) + (0x20CF3u-0x20C0Cu) +
              (0x20D7Bu-0x20CF4u) + (0x20E08u-0x20D7Cu));'''
new_assert='''static_assert(kVerifiedTranslatedBytes == 138937u +
              (0x210AFu-0x20E08u) + (0x211D8u-0x210B0u) +
              (0x214B8u-0x211D8u) + (0x21702u-0x214B8u));'''
assert old_assert in c
c=c.replace(old_assert,new_assert)

header='''\n// -----------------------------------------------------------------------------
// Batch 47: 20E08..21701 interaction gates, directional record activation,
// environment-list insertion, actor inventory/link updates, and paired UI
// refresh. 210AF and 21702..21703 are ledgered compiler separators. The next
// routine begins at 21704.
// -----------------------------------------------------------------------------\n\n'''
body='\n'.join(s.splitlines()[2:]).rstrip()+'\n'
c=c.rstrip()
c=c[:-len('} // namespace code2')].rstrip()+header+body+'\n} // namespace code2\n'
cpp.write_text(c)

insert='210AF..210AF | alignment | 1\n21702..21703 | alignment | 2\n'
marker='<!-- RESIDUAL-RANGES-END -->'
assert insert not in l
assert marker in l
l=l.replace(marker,insert+marker)
l=l.replace('**Total proven non-code/data:** **2,667 bytes**.',
            '**Total proven non-code/data:** **2,670 bytes**.')
l += ('\n\nBatch 47 evidence: `210AF` decodes as a one-byte `NOP` between the directly '
      'reconstructed `20E08` return and the `210B0` entry; `21702..21703` decodes '
      'as `MOV EAX,EAX` between the `214B8` return and the `21704` entry. A full '
      'object-1 direct relative CALL/JMP/conditional-branch target scan found no '
      'incoming target to either separator. The encoded `210B0` `MOVZX word` / '
      '`CMP -1` pair is intentionally not treated as a table terminator.\n')
ledger.write_text(l)

p=p.replace('object%20accounting-59.08%25-red','object%20accounting-60.04%25-red')
p=p.replace('**Verified translated instruction bytes:** **138,937**','**Verified translated instruction bytes:** **141,234**')
p=p.replace('**Proven non-code/data bytes:** **2,667**','**Proven non-code/data bytes:** **2,670**')
p=p.replace('**Reconciled object bytes:** **141,604 / 239,698 = 59.0760% (59.08%)**',
            '**Reconciled object bytes:** **143,904 / 239,698 = 60.0355% (60.04%)**')
p=p.replace('**Unresolved object bytes:** **98,094**','**Unresolved object bytes:** **95,794**')
p += '''\n\n## CODE.2 Batch 47 checkpoint\n\nBatch 47 added **2,297 translated instruction bytes** plus **3 proven alignment bytes**. Cumulative accounting is **141,234 translated + 2,670 non-code = 143,904 / 239,698 = 60.0355% (60.04%)**; unresolved is **95,794 bytes**.\n\nThe sequential application frontier now reaches `21703`; the next routine begins at `21704`. Preserve `20E08`'s two-stage `1C328` decision and saved `20D7C` outputs, the deliberately non-terminating `210B0` `MOVZX word`/`CMP -1` table scan, `211D8`'s type-0Dh/type-0Fh list insertion fallbacks, and `214B8`'s saved `8600/8602` values across opaque calls while reloading `255D8` before the post-callback type/subtype read. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved. Semantic audit remains not started.\n'''
progress.write_text(p)

a += '''\n\n## CODE.2 Batch 47 checkpoint\nBatch 47 adds **2,297 translated bytes** and **3 proven alignment bytes**, bringing cumulative accounting to **141,234 translated + 2,670 non-code = 143,904 / 239,698 = 60.0355% (60.04%)**. The sequential frontier is now `21704`. Preserve the impossible `210B0` word-sentinel comparison exactly, `211D8`'s environment-list fallback order, and `214B8`'s saved current-environment values versus the explicit post-callback reload of `255D8`.\n'''
continuity.write_text(a)
