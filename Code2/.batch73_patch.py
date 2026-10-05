from pathlib import Path
p=Path('Code2/.batch73.fragment.cpp')
s=p.read_text()
s=s.replace('s32 key=static_cast<s16>(load_le16(dg(m,rec));','s32 key=static_cast<s16>(load_le16(dg(m,rec)));')
s=s.replace('const s32 delta=static_cast<s32>(static_cast<u32>(world_x-camera)+static_cast<u32>(wrap));','const s32 delta=static_cast<s32>(static_cast<u32>(world_x)-static_cast<u32>(camera)+static_cast<u32>(wrap));')
p.write_text(s)
