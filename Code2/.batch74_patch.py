from pathlib import Path
p=Path('Code2/.batch74.fragment.cpp')
s=p.read_text()
old='static s32 app74_sar1(s32 v) noexcept{return v>=0?v/2:-static_cast<s32>((static_cast<u32>(-static_cast<s64>(v))+1u)/2u);}'
new='static s32 app74_sar1(s32 v) noexcept{const u32 u=static_cast<u32>(v);return static_cast<s32>((u>>1)|(u&0x80000000u));}'
assert s.count(old)==1
p.write_text(s.replace(old,new,1))
