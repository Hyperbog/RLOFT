from pathlib import Path
p=Path('Code1/Code1.cpp')
s=p.read_text()
a=s.index('struct LowGraphics13Boundary{')
z=s.index('\n};',a)
block=s[a:z]
block=block.replace('void(*call_6f74)(u32)=nullptr','u32(*call_6f74)(u32)=nullptr')
block=block.replace('void(*call_e6ea)(u32)=nullptr','u32(*call_e6ea)(u32)=nullptr')
if 'call_9814_local' not in block:
    block=block.replace('u32(*format_ede7)(u32,u32,u32)=nullptr;','u32(*format_ede7)(u32,u32,u32)=nullptr;void(*call_9814_local)(const char*)=nullptr;')
s=s[:a]+block+s[z:]
p.write_text(s)
