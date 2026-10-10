# Level22 authentic F790 item text and typed caller-provenance checkpoint
**Date:** 2026-10-10. **Branch:** `demo-v1`. **Scope:** original item-description **text** is now used by the native party presenter; the original event-driven F66C ribbon graphics are still not connected.

## Source evidence (original files, not disassembler inference)

The supplied original CODE.2 is 558,689 bytes and has SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. Its object-1 code begins at physical `0x3AF24`, with original F790 at logical `0xF790`. An independent binary E8-rel32 scan reverified exactly **17 direct callsites**, grouped **11 global WORD 8600/BYTE 8602**, **3 explicit (-1,0)**, **2 stack-local** and **1 selected-item/known-zero**. Original F7B4 checks pending 6DAC first; F7DB skips ordinary empty-hand ribbon work; F5F8 returns OBJECTS.DAT item 80's weight for subtype20. These original machine-byte signatures passed locally again.

All **336** supplied OBJECTS.DAT names are NUL-terminated within 40 bytes and contain only 7-bit ASCII; longest name has 39 bytes. CODE.2 original LE data-object punctuation and decoded T-archive text resources were recovered in earlier checkpoints and remain authoritative. The original T resource has 1,073 entries; the original resource file is in the repository's `DnD-Classics` branch.

## Live native presentation change (bounded)

Before this checkpoint, `DemoV1/src/party_modal_presenter.cpp` rendered held text with `InventoryItemDescriptionModel::describe`: a generic name, enchantment and weight. The presenter now loads the original `OBJECTS.DAT`, `CODE.2` and `T` bytes **once in its constructor**, and calls `OriginalF790RuntimeDescription::compose` for ordinary held-item text. This source-backed model validates the original raw record against `ObjectCatalog`, uses original F790 category-specific damage, armor bonus, half-unit weight, charges and capacity formatting, and obtains **current** recursive weight from the already-existing mutable `InventoryRuntimeState`. The display-only text baseline is now consistently original F66C y=`0x65` for pending and ordinary descriptions.

**Pending mage-scribing feedback is unchanged:** its original 759C content takes precedence before ordinary item formatting. The original `T` archive already loads in `RavenloftBootAssets`; the new constructor validates/decodes a separate read-only instance for source backing. The existing provisional flat decorative strip and hardware pointer compositing are deliberately unchanged. Ordinary source text may still be visually positioned differently from DOS because that decorative raster has not yet been wired or pixel-accepted.

The presenter still executes at native VGA refresh frequency. It does **not** call `F790`, `OriginalF790RuntimeEvent::dispatch_source_call`, `OriginalRandom::next15`, source IRQ 30700, or `OriginalF790ModalSequence` during each repaint. This avoids unverified random-state consumption and false original call cadence. Previously retained generic helper unit tests still cover that legacy independent helper, not the live F790 text path.

## Typed source-call ownership for future event routing

`OriginalF790CallerCatalog` is a constexpr C++17 ordered table for all 17 direct original E8 sites and their four verified argument sources. `OriginalF790RuntimeEvent::arguments_for_direct_call` uses that explicit source offset to read **global held**, **immediate empty**, **source stack-local** or **selected-known-zero** values. It rejects unknown offsets. No guessed native mouse event or frame-refresh is allowed to masquerade as a source E8 site.

The existing integration CTest exercises all caller argument categories, source-local values different from the global cursor and pending 759C priority. A new standalone CTest `original_f790_caller_catalog_tests` checks the sorted 17 entries, exact category counts (11/3/2/1), unknown-site rejection and constexpr C++17 operation. The Python `original_f790_runtime_event_contract_tests` compares the catalog to real CODE.2 callsite byte evidence and guards the live text-only/no-RNG boundary.

## Verification and outstanding gates

Local strict C++17 `-Wall -Wextra -Werror -pedantic` compilation and execution passed for a standalone original caller-catalog smoke test. Independently checked the original CODE.2 SHA-256, exact 17 E8 call targets, pending/empty-hand instruction signatures and special bag weight branch, plus all 336 original item name encodings.

**202 CTests are now registered when Python is available**, up from 201, but full native game-library tests and this **new live text change have not been compiled on Windows**. Last completed Windows Full remains GitHub Actions #651, passing 182/182 **at an older revision**. Current original-DOS parity remains unaccepted; engineering readiness and Level22 completion percentages are unchanged.

The next live behavior gate is to map authentic individual original F790 invocations to each native inventory click/selection/scribe-expiry transition, preserving 112EC capture ordering, 2932C background restoration, source-local held arguments, original RES2:3 graphics and **the one shared ExplorationRuntime RNG**. Require a manually authorized Windows build and real original-DOS text/pixel/tick verification before claiming faithful ribbon completion. No independent Level23 gameplay or modernization.
