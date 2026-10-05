from pathlib import Path
p=Path('Code2/.batch72.fragment.cpp')
s=p.read_text()
old='return (app72_r8(out,0x18u)&1u)==0u?(app72_r32(out,4u)<<4):0u;'
new='return (app72_r8(out,0x18u)&1u)!=0u?(app72_r32(out,4u)<<4):0u;'
assert s.count(old)==1
p.write_text(s.replace(old,new,1))
