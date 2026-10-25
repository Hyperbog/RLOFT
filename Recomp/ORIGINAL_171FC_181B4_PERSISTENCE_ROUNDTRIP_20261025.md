# Part 34 — CODE.2 original 171FC save and 181B4 restore

**Source branch:** `Recomp`. **Production cross-level gameplay:** still guarded.

## Original-source implementation

The diagnostic `rloft_original_code2_level_loader --transition-roundtrip /owned/LEVELS.DAT`
now invokes the *compiled original* 181B4 and 171FC code on one retained
CODE.2 guest. It uses the real level 22 and 23 records in the owner's
read-only, SHA-authenticated LEVELS.DAT. No original assets are committed.

1. Source 181B4 loads Level 22 (original archive fallback, old-level sentinel -1).
2. One test-only guest map flag is changed in memory, preserving the original
   owner's file. The exact original 0x514-byte script descriptor table is captured.
3. Source 181B4(23) invokes original 171FC(22), using the original 0x819C
   WORK/LEV filename, original 31380/312C8/312B4 file lifecycle, 3131C seeks
   and 31304 guest/local writes. A bounded, private in-memory file captures
   the serialization; 181B4 loads archived Level 23 in the same guest.
4. Source 181B4(22) invokes 171FC(23) for a second private snapshot,
   then follows its *direct* saved-level loading path for saved LEV022.
5. The original 22->23->22 guest transition is accepted by the diagnostic
   only if the saved-level index and dimensions decode independently,
   the controlled tile change survives, all event script bytes match,
   the active guest level is restored, and open handles are closed.

## CI gate

The Windows milestone workflow invokes the new round-trip immediately
after independently verifying the original LEVELS.DAT SHA256 and the
32-level source loader. The command has a nonzero exit status if any
round-trip invariant fails. It never uploads a snapshot or altered frame.

## Boundaries

- The private writer is an *engineering file adapter*, not a DOS INT 21
  writable production file provider. Retail media remains read-only.
- The original loader's memory and world state are retained across the
  three synthetic diagnostic transitions, but continuous live 1FC14
  gameplay re-entry, cross-level camera updates, host VGA presentation
  and return through the actual Windows Level22 window are **not tested**.
- Original 17A68 actor postload dialogue, 1F8FC display-mode switches,
  native sound callbacks and all mission-specific scripts remain separate
  acceptance gates; unsupported branches must not be replaced with fakes.
- Original DOS synchronized framebuffer and AIL audio parity remain open.

**Next:** port only verified writer/reader callbacks into the live
runtime's private owner-session, preflight destination media before
171FC mutates a live level, and re-enter the original 1FC14 after
181B4 creates the destination world. Preserve graceful source-stop
behavior for unresolved actor/dialogue/render and audio dependencies.
