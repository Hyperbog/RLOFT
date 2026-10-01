from pathlib import Path
p=Path('Code1/Code1.cpp')
s=p.read_text()
old="struct FormatState{u8 opaque0[4];u32 width;u32 precision;u8 opaque0c[8];u8 flags;u8 opaque15;u8 pad;u8 opaque17;}; static_assert(sizeof(FormatState)==0x18);"
new="struct FormatState{u8 opaque0[4];u32 width;u32 precision;u32 zero_count;u8 opaque10[4];u8 flags;u8 conversion;u8 pad;u8 prefix[3];}; static_assert(sizeof(FormatState)==0x1c);"
assert old in s
s=s.replace(old,new,1).replace('s.opaque17=0;s.pad=', 's.prefix[0]=0;s.pad=',1)
st=Path('Code1/FormatterContinuation.staging.cpp').read_text()
a=st.index('// F206..F2D1')
b=st.index('static_assert(0xF2D2u-0xF206u')
block=st[a:b].rstrip()+"\n\n"
needle='void function_f722(char*s)noexcept'
assert needle in s and 'void function_f206(char*out' not in s
s=s.replace(needle,block+needle,1)
s=s.replace('static_assert(0xF206u-0xF1B3u==83u); static_assert(0xF742u-0xF722u==32u);', 'static_assert(0xF206u-0xF1B3u==83u); static_assert(0xF2D2u-0xF206u==204u); static_assert(0xF2E9u-0xF2D2u==23u); static_assert(0xF722u-0xF2E9u==1081u); static_assert(0xF742u-0xF722u==32u);',1)
s=s.replace('constexpr u32 kVerifiedTranslatedBytes=5473u;','constexpr u32 kVerifiedTranslatedBytes=6781u;',1)
p.write_text(s)

pr=Path('Code1/PROGRESS.md')
t=pr.read_text()
t=t.replace('verified%20conversion-7.77%25-red','verified%20conversion-9.63%25-red')
t=t.replace('**Verified conversion:** 7.77%','**Verified conversion:** 9.63%').replace('**Remaining:** 92.23%','**Remaining:** 90.37%')
t=t.replace('Current verified translated bytes: **5,473**.','Current verified translated bytes: **6,781**.')
t=t.replace('Raw coverage: **7.7739%**, displayed conservatively as **7.77%**.','Raw coverage: **9.6318%**, displayed conservatively as **9.63%**.')
t=t.replace('- Formatter helper subset `F007..F205` and `F722..F741`: **543 bytes**.','- Formatter helpers and materializer `F007..F721` plus `F722..F741`: **1,851 bytes**.')
t=t.replace('Total integrated verified translated bytes: **5,473**.','Total integrated verified translated bytes: **6,781**.')
start=t.find('## Formatter continuation cleared for integration')
end=t.find('## Active continuation')
if start!=-1 and end!=-1:
    t=t[:start]+'''## Formatter materializer integrated\n\n`F206..F721` (**1,308 verified bytes**) has been folded into `Code1.cpp`. The integrated formatter region now covers `F007..F741` except for the separately translated driver `EDE7..F006`, which remains staged pending its hardening pass.\n\nThe integrated materializer preserves the verified signed 16.16 fixed-point path, EAX:DX far-return contract, selector-qualified string handling, octal ESI/EDI aliasing, zero-precision suppression, pointer formatting, sign/prefix accounting, and the shared `F001` caller relationship.\n\n'''+t[end:]
t=t.replace('Fold the verified 1,308-byte `F206..F721` continuation into `Code1.cpp`, update integrated coverage to 9.63%, then translate `EDE7..F006` against the now-proven materializer and helper contracts.','Harden the staged `EDE7..F006` formatter driver against the immutable bytes, then fold it into `Code1.cpp`.')
t=t.replace('Integrate `F206..F721`; translate `EDE7..F006`;','Verify/integrate `EDE7..F006`;')
pr.write_text(t)
