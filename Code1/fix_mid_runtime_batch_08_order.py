from pathlib import Path
p=Path('Code1/Code1.cpp')
s=p.read_text()
marker='// 59AC..5A33: scan 256 RGB triples and return the triple index nearest to (31,31,31).'
assert marker in s and 'struct PaletteTreeBoundaries{' not in s
struct=r'''struct PaletteTreeBoundaries{
 u32(*read_dgroup32)(u32)=nullptr;
 void(*insert_5880)(u8,u8,u8,u16,s32)=nullptr;
 u8(*read_guest8)(u32)=nullptr; void(*write_guest8)(u32,u8)=nullptr;
 void(*dos_print_dollar_guest)(u32)=nullptr;
 s32(*call_d23a)(u32,u16)=nullptr; s32(*call_d280)(u32,u16,u32)=nullptr; s32(*call_d2ad)(u32)=nullptr;
 u32(*call_d313)(u32,u32)=nullptr; u32(*call_d55e)(u32,u32)=nullptr; u32(*call_d573)(u32)=nullptr;
 s32(*call_d5ad)(u32,void*,u32)=nullptr; s32(*call_d686)(u32,const void*,u32)=nullptr; s32(*call_d85f)(u32,s32,u32)=nullptr;
 s32(*call_d8aa)(const char*)=nullptr; u32(*call_d8e8)(u32,u32*)=nullptr; u32(*call_d8d0)(u32*)=nullptr;
 u32(*call_d906_guest)(u32)=nullptr; u32(*call_d991)(u32)=nullptr; s32(*call_d9a6)(const char*)=nullptr; s32(*call_d9ca)(const char*)=nullptr;
 void(*call_a2b0)(s32,s32)=nullptr; void(*call_a21c)(s32)=nullptr; void(*call_a484)(u32,s32,s32,s32,s32)=nullptr;
 void(*call_a4e8)(u32,s32,s32,s32,s32)=nullptr; void(*call_a6f0)(u32,s32,s32,u32)=nullptr;
 void(*call_a7e8)(u32,s32,s32,u32,u8)=nullptr; void(*call_a8f4)(u32,s32,s32,u32,u32)=nullptr; void(*call_8960)()=nullptr;
};

'''
s=s.replace(marker,struct+marker,1)
p.write_text(s)
