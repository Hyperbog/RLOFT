from pathlib import Path
p=Path('Code1/Code1.cpp')
s=p.read_text()
assert 'function_e360' not in s
assert 'constexpr u32 kVerifiedTranslatedBytes=8576u;' in s
needle='constexpr u32 kVerifiedTranslatedBytes=8576u;'
code='''// E360..E3B2: signed decimal parser. Character classes are indexed by unsigned byte+1 at DGROUP+03B4; bit 02h skips leading class, bit 20h accepts digits.\n[[nodiscard]]u32 function_e360(const u8*data,u32 text)noexcept{u32 p=text;auto cls=[&](u8 c)noexcept->u8{return data[0x3b4u+static_cast<u32>(static_cast<u8>(c+1u))];};while((cls(data[p])&0x02u)!=0)++p;const u8 sign=data[p];if(sign==0x2bu||sign==0x2du)++p;u32 v=0;while((cls(data[p])&0x20u)!=0){v=v*10u+static_cast<u32>(data[p]);++p;v-=0x30u;}if(sign==0x2du)v=0u-v;return v;}\nstatic_assert(0xE3B3u-0xE360u==83u);\n\n'''
s=s.replace(needle,code+'constexpr u32 kVerifiedTranslatedBytes=8659u;',1)
p.write_text(s)
q=Path('Code1/PROGRESS.md')
t=q.read_text()
t=t.replace('verified%20conversion-12.18%25-red','verified%20conversion-12.29%25-red')
t=t.replace('**Verified conversion:** 12.18%  ','**Verified conversion:** 12.29%  ')
t=t.replace('**Remaining:** 87.82%  ','**Remaining:** 87.71%  ')
t=t.replace('Current verified translated bytes: **8,576**.','Current verified translated bytes: **8,659**.')
t=t.replace('Raw coverage: **12.1815%**, displayed conservatively as **12.18%**.','Raw coverage: **12.2993%**, displayed conservatively as **12.29%**.')
t=t.replace('- Complete formatter driver/parser/materializer `EDE7..F741`: **2,395 bytes**.','- Signed decimal parser `E360..E3B2`: **83 bytes**.\n- Complete formatter driver/parser/materializer `EDE7..F741`: **2,395 bytes**.')
t=t.replace('Total integrated verified translated bytes: **8,576**.','Total integrated verified translated bytes: **8,659**.')
t=t.replace('`RuntimeGapE0D0.staging.cpp` contains **722 functional instruction bytes**','`RuntimeGapE0D0.staging.cpp` originally contains **722 functional instruction bytes**; `E360..E3B2` is now integrated, leaving **639 unique unintegrated bytes**')
t=t.replace('The unique unintegrated instruction-bounded staging pool is **2,064 bytes**: 747 startup + 595 remaining DOS/CRT + 722 E0D0..E3B2.','The unique unintegrated instruction-bounded staging pool is **1,981 bytes**: 747 startup + 595 remaining DOS/CRT + 639 remaining E0D0..E35F.')
q.write_text(t)
