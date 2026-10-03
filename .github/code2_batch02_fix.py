from pathlib import Path

p=Path('Code2/Code2.cpp')
s=p.read_text()
old='''static_assert(kVerifiedTranslatedBytes ==
              (0x00073u-0x00010u) + (0x000B8u-0x00074u) +
              (0x001FAu-0x000B8u) + (0x00271u-0x001FCu) +
              (0x369A7u-0x35F88u) + (0x39526u-0x391F2u));'''
new='''static_assert(kVerifiedTranslatedBytes ==
              (0x00073u-0x00010u) + (0x000B8u-0x00074u) +
              (0x001FAu-0x000B8u) + (0x00271u-0x001FCu) +
              (0x369A7u-0x35F88u) + (0x39526u-0x391F2u) +
              (0x006B9u-0x002A8u) +
              (0x29A84u-0x29930u) + (0x29A9Fu-0x29A84u) +
              (0x29AF0u-0x29AA4u) + (0x29B19u-0x29AF0u) +
              (0x29C9Eu-0x29B40u) + (0x29CE9u-0x29CA0u) +
              (0x29EAAu-0x29CECu) +
              (0x2A20Cu-0x2A1E8u) + (0x2A256u-0x2A20Cu) +
              (0x2A305u-0x2A258u) + (0x2A380u-0x2A308u) +
              (0x2A478u-0x2A380u) + (0x2A584u-0x2A478u) +
              (0x2A6B0u-0x2A584u) + (0x2A89Fu-0x2A6B0u) +
              (0x2AA9Bu-0x2A8A0u));'''
assert old in s
s=s.replace(old,new,1)
old_call='} else function_00010(m,static_cast<s32>(a3),static_cast<s32>(a2));'
new_call='} else (void)function_00010(m,static_cast<s32>(a3),static_cast<s32>(a2));'
assert old_call in s
s=s.replace(old_call,new_call,1)
p.write_text(s)
