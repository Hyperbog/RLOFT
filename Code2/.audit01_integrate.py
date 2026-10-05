from pathlib import Path
import sys

p=Path('Code2/Code2.cpp')
s=p.read_text()
old='''        return b.call_1c124 ? b.call_1c124(0x3Cu) : record_off;\n    }\n\n    eax=b.call_1c1dc?b.call_1c1dc(0x3Cu):0u;'''
new='''        // Binary 00117 JGE branches directly to the epilogue at 001F6.\n        // No 1C124h(3Ch) call occurs on this branch; EAX is the live\n        // record offset after the fourth 248h stride (0920h).\n        return record_off;\n    }\n\n    eax=b.call_1c1dc?b.call_1c1dc(0x3Cu):0u;'''
if sys.argv[1]=='integrate':
    if old not in s:
        raise SystemExit('audit01 target not found or already modified')
    p.write_text(s.replace(old,new,1))
elif sys.argv[1]=='verify':
    if old in s or new not in s:
        raise SystemExit('audit01 verification failed')
else:
    raise SystemExit('usage: integrate|verify')
