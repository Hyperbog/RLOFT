from pathlib import Path
p=Path('Code1/Code1.cpp')
s=p.read_text()
old='[[nodiscard]]s32 function_111a4(Machine&,FileBoundaries&,RuntimeIoBoundaries&,u32)noexcept; [[nodiscard]]s32 function_11241(FileBoundaries&,RuntimeIoBoundaries&,u32)noexcept; [[nodiscard]]s32 function_11285(FileBoundaries&,RuntimeIoBoundaries&,u32,u32,u32)noexcept;'
new='[[nodiscard]]s32 function_10c59(FileBoundaries&,u32)noexcept; [[nodiscard]]s32 function_111a4(Machine&,FileBoundaries&,RuntimeIoBoundaries&,u32)noexcept; [[nodiscard]]s32 function_11241(FileBoundaries&,RuntimeIoBoundaries&,u32)noexcept; [[nodiscard]]s32 function_11285(FileBoundaries&,RuntimeIoBoundaries&,u32,u32,u32)noexcept;'
assert old in s
p.write_text(s.replace(old,new,1))
print('added checkpoint 10n source-order declaration')
