from pathlib import Path
p=Path('Code2/.batch80.fragment.cpp')
s=p.read_text()
old='const s32 count=app74_wrap_add(app74_wrap_add(end,-start),1);'
new='const s32 count=app74_wrap_add(app74_wrap_add(end,static_cast<s32>(0u-static_cast<u32>(start))),1);'
assert s.count(old)==2
p.write_text(s.replace(old,new))
