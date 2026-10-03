from pathlib import Path
cpp_path=Path('Code2/Code2.cpp'); progress_path=Path('Code2/PROGRESS.md'); add_path=Path('Code2/CODE2_CONTINUITY_ADDENDUM.md'); block_path=Path('Code2/batch02_append.inc')
cpp=cpp_path.read_text(); block=block_path.read_text().rstrip()+"\n"
assert 'constexpr u32 kVerifiedTranslatedBytes = 4017u;' in cpp and 'void function_002a8(' not in cpp
cpp=cpp.replace('constexpr u32 kVerifiedTranslatedBytes = 4017u;','constexpr u32 kVerifiedTranslatedBytes = 7139u;',1)
old_account='''static_assert(kVerifiedTranslatedBytes ==
              (0x00073u-0x00010u) + (0x000B8u-0x00074u) +
              (0x001FAu-0x000B8u) + (0x00271u-0x001FCu) +
              (0x369A7u-0x35F88u) + (0x39526u-0x391F2u));'''
new_account='''static_assert(kVerifiedTranslatedBytes ==
              (0x00073u-0x00010u) + (0x000B8u-0x00074u) +
              (0x001FAu-0x000B8u) + (0x00271u-0x001FCu) +
              (0x369A7u-0x35F88u) + (0x39526u-0x391F2u) +
              (0x006B9u-0x002A8u) + (0x006FDu-0x006C0u) +
              (0x00792u-0x00700u) + (0x007E2u-0x00794u) +
              (0x00839u-0x007E4u) + (0x00931u-0x0083Cu) +
              (0x009B0u-0x00934u) +
              (0x29A84u-0x29930u) + (0x29A9Fu-0x29A84u) +
              (0x29AA2u-0x29AA0u) + (0x29AF0u-0x29AA4u) +
              (0x29B19u-0x29AF0u) + (0x29B33u-0x29B1Cu) +
              (0x29B3Eu-0x29B37u) + (0x29C9Eu-0x29B40u) +
              (0x29CE9u-0x29CA0u) +
              (0x2A20Cu-0x2A1E8u) + (0x2A256u-0x2A20Cu) +
              (0x2A305u-0x2A258u) + (0x2A380u-0x2A308u));'''
assert old_account in cpp
cpp=cpp.replace(old_account,new_account,1)
needle='\n} // namespace code2\n'; assert cpp.count(needle)==1; cpp=cpp.replace(needle,'\n'+block+needle,1); cpp_path.write_text(cpp)
p=progress_path.read_text()
for a,b in {
'object%20accounting-1.68%25-red':'object%20accounting-2.98%25-red',
'**Verified translated instruction bytes:** **4,017**':'**Verified translated instruction bytes:** **7,139**',
'**Reconciled object bytes:** **4,017 / 239,698 = 1.6759% (1.68%)**':'**Reconciled object bytes:** **7,139 / 239,698 = 2.9783% (2.98%)**',
'**Unresolved object bytes:** **235,681**':'**Unresolved object bytes:** **232,559**'}.items(): assert a in p,a; p=p.replace(a,b,1)
section='''\n\n## Batch 02 — command interpreter, record searches, and graphics anchors\n\nSource identity and object-1 SHA-256 were reverified before this batch. Added **3,122 translated instruction bytes**, bringing the verified total to **7,139 / 239,698 = 2.9783% (2.98%)**.\n\n- `002A8..006B8` — **1,041 bytes**: complete 13-way five-byte command interpreter driven by the CS table at `00274`; preserves temporary object flag mutation/restoration, packed 16-bit stack arguments, signed IDIV remainder behavior, byte/word table updates, three input-poll loops, and the encoded stop-command path. All observed callers discard EAX.\n- `006C0..006FC` — **61 bytes**: signed camera-relative coordinate derivation and `-40` clamps.\n- `00700..00791` — **146 bytes**: dual primary/alternate record search selected by `5FCB`, preserving signed word comparisons and the `6048` base state.\n- `00794..007E1` — **78 bytes**: conditional search of the `1761A/1761E` 22-byte record table.\n- `007E4..00838` — **85 bytes**: typed `54h` search of the primary 22-byte record table.\n- `0083C..00930` — **245 bytes**: filtered search with type callback, `F03C/F03E` gates, table bit `40h`, and encoded ±10 coordinate proximity test.\n- `00934..009AF` — **124 bytes**: alternate type-predicate record search.\n- `29930..29CE8` selected instruction ranges — **939 bytes**: VGA mode setup, pixel write/read, plane-mode selector and tail alias, planar rectangle fill/shared tail, and DAC palette streaming. Compared against audited CODE.1 `9CA0..A058`: all 49 differing bytes are confined to relocated absolute address fields; code is shifted `+0x1FC90` and DGROUP state `+0xC4B8`, with no opcode/control-flow drift in the modeled functions.\n- `2A1E8..2A37F` selected instruction ranges — **403 bytes**: encoded image size, four-plane transpose, and two reversal helpers. The complete 408-byte CODE.2 span is byte-identical to CODE.1 `A558..A6EF`; `2A256..2A257` and `2A305..2A307` remain uncounted alignment islands.\n\nThe visible separators/jump-table bytes around these ranges remain unresolved rather than being credited as non-code in this batch. C++17 syntax verification with `-Wall -Wextra -pedantic` is required before integration.\n\n### Next targets after batch 02\n\n1. Continue application logic from `009B0`, using the newly proved search helpers as local anchors.\n2. Reconcile the startup/CRT path at `353A0` independently against CODE.2.\n3. Continue harvesting graphics/runtime families only where whole CODE.2 function boundaries can be proved; exact substring matches alone are not sufficient.\n'''
assert '## Batch 02 — command interpreter' not in p; progress_path.write_text(p+section)
a=add_path.read_text(); add='''\n\n## CODE.2 batch 02 checkpoint\n\nVerified translated total: **7,139 / 239,698 = 2.98%**. Batch 02 added **3,122** instruction bytes at `002A8..009AF` selected function ranges, `29930..29CE8` selected VGA ranges, and `2A1E8..2A37F` selected image-helper ranges. The VGA family was reconciled against CODE.1 only after proving its differences are relocated absolute operands; the image-helper 408-byte span is byte-identical, with two known alignment islands left uncounted. Preserve the `002A8` command interpreter's packed stack dwords, temporary flag restoration, signed remainder, and address-distinct table dispatch. Continue at `009B0`, startup `353A0`, or independently proven runtime/graphics anchors.\n'''
assert '## CODE.2 batch 02 checkpoint' not in a; add_path.write_text(a+add)
