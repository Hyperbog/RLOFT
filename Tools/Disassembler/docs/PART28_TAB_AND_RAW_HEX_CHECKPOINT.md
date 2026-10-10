# Part 28 — Explorer Tab-State Fix and Raw-File Hex Inspection

Status: **source implemented, NOT YET COMPILED ON WINDOWS**. Date: 2026-10-08.

## User-reported defects in the latest Windows preview

1. When viewing the **Hexadecimal** or **References** tab, selecting a function in the Explorer forcibly switches back to **Disassembly**.
2. Disassembly appears to work only for Ravenloft: Strahd's Possession and Stone Prophet CODE files. Other binaries/resources appear to produce no findings or useful data.

Both observations are grounded in current implementation scope. Function selection invokes `Navigate()`, which explicitly selected tab index 0. Separately, `LeImage.Load` only understands certain nested MZ / 32-bit LE DOS/4GW executable layouts; this is NOT a complete loader for MZ 16-bit, COM, game resource archives or arbitrary binary inputs. Empty disassembly for other file types **is expected**; an empty inspection pane is NOT a good user experience. File Intelligence also has only two known exact RLOFT CODE fingerprint entries, though it can inspect any loaded artifact's hash and length.

## Source-level changes

- Explorer/Go-to-function/navigation now **retains the current active tab**. Selected instruction, hex contents and indexed references continue to update; the code view scrolls only if Disassembly is the active tab.
- Every loaded file (up to the existing 128 MiB prototype bound), even if not recognized as LE, shows a **raw hexadecimal and ASCII preview** of its original bytes using physical file offsets, without applying 32-bit instruction decoding.
- Opening a non-LE artifact automatically switches to **Hexadecimal** once, as a sensible view for new file loading. Selecting Explorer entries no longer switches tabs.
- **Go to** now accepts `file:0xOFFSET` to inspect physical offsets in any file; an LE mapped executable still accepts `object:0xOFFSET`. These are explicitly different address spaces.
- The Explorer is labeled `RAW FILE — NO FUNCTION ANALYSIS` for unsupported sources. File Intelligence adds an evidence-tagged `analysis_support` field explaining which analysis engine features can be used.
- Opening a project for a raw/non-LE file keeps raw hex visible and never resurrects cached interpreted code when there is no executable image to verify.
- A bounded `RawHexPreview.Render` helper is in Core and a synthetic test covers ASCII/hex bytes, immutable source, empty files, start offsets, end-of-file bounds and the capability label.
- No guessed assembly or false function candidates for raw resource bytes. The original game files are never altered.

## What is *not* yet supported

- Full original Ravenloft and Stone Prophet resource formats (DAT, RES, ANM, etc.): inspectable as raw bytes, but structure-specific decoders are separate workstreams.
- 16-bit MZ/.COM executable disassembly, NE/LX/PE loaders, code/data classification of unknown files.
- A general-purpose strings, symbols, binary patterns or reconstructed game asset viewer within this disassembler.

Do not silently treat arbitrary game data as executable code. The general resource extractor and reverse-engineering workstreams remain separate, and 100% original-game behavior parity requires their dedicated validation.

## Acceptance gate — Windows testing

1. Build the new source on Windows 11, run both synthetic suites, and verify `RawHexPreview` tests pass.
2. Load an analyzed original CODE.1 or CODE.2, select the **Hexadecimal** tab, then choose different function candidates: **Hexadecimal must remain selected**, and the text view must follow each address.
3. Repeat using the **References** tab. Relevant incoming/outgoing data should change with the function address while the tab stays selected. Explicit source-file `file:0xOFFSET` Go may switch to Hexadecimal by design.
4. Open an ordinary data/resource file (such as `OBJECTS.DAT`) and confirm SHA-256/size/analysis-support metadata, RAW FILE explorer label and true `file+0x...` hex rows. There must be **no invented disassembly instructions**. Use `file:0x10` or `file:0x100` to inspect other offsets.
5. Save/reopen a raw metadata-only project. It should remain raw even if an older project contains cached analysis that cannot be validated.
6. Test null/empty/truncated and 128 MiB-limited files, non-ASCII bytes and boundary offsets. Review layout at 100%/150% DPI.

**Overall first-release completion estimate remains 39%** from Part 27; this source fix is not yet built or visually confirmed. GitHub Actions remains strictly `workflow_dispatch` **only** to save minutes. No Linux release work and no AI assistant until after the final release.
