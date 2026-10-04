from pathlib import Path

p=Path('Code2/CODE2_CONTINUITY_ADDENDUM.md')
s=p.read_text()
anchor='''## Continuation behavior\n\nWhen the user says `continue`, perform actual CODE.2 binary analysis and commit substantive work before replying. Begin by reading the copied playbook, this addendum, `README.md`, `PROGRESS.md`, and the current `Code2.cpp`; then verify the branch head and CODE.2 source identity, regenerate disassembly, work, syntax-check, commit, and report exact ranges/bytes/coverage/commit SHA.\n'''
assert anchor in s
addition='''\n## Mandatory interruption/resume safety protocol\n\nBefore continuing after any interruption, context/thread rollover, timeout, failed workflow, or uncertainty about the prior stopping point, read and follow `Code2/INTERRUPTION_RESUME_PROTOCOL.md` and `Code2/RESUME_CHECKPOINT.md`. Do not trust chat-only frontier state. Re-establish live HEAD and accounting from the repository, reverify immutable CODE.2/object-1 hashes, and re-disassemble a small overlap spanning the last committed frontier before moving forward. The overlap is verification only and must never be counted twice. If the overlap, instruction boundary, ownership, address convention, or accounting disagrees with the committed checkpoint, freeze forward progress and repair the prior batch first.\n\nAfter every substantive batch, refresh `RESUME_CHECKPOINT.md` with the last substantive commit, exact cumulative accounting/frontier, and a SHA-256 for a small binary overlap window around the new frontier. Mark any analysis beyond the committed frontier as `UNCOUNTED / NOT COMMITTED`.\n'''
assert '## Mandatory interruption/resume safety protocol' not in s
s=s.replace(anchor,anchor+addition,1)
p.write_text(s.rstrip('\n')+'\n')
