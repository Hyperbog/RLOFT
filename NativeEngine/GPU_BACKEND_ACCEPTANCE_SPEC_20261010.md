# GPU renderer comparison — implementation and evidence gates (2026-10-10)

**Owner:** `native-engine`. **Status:** official API research + benchmark specification ONLY; no SDL3, bgfx or Godot backend compiled or measured. **Product:** installed-asset native engine, separate from DemoV1. This supplements [Framework RFC-001](FRAMEWORK_RESEARCH_RFC_001_20261010.md) and [Proof B](FRAMEWORK_PROOF_B_DISK_MOD_MOUNTS_20261010.md).

## Decision correction: do not depend on modern 3D APIs to display original VGA

The existing `IndexedFrame` (320×400 indices plus 768 original 6-bit DAC bytes) is the renderer-neutral contract. For **original** display, first target the **SDL3 Renderer** with an RGBA8 streaming/upload texture and explicitly calculated 4:3 output rectangle (e.g. `fit_original_aspect`); this is an independent *fallback* host, not a replacement for the two advanced graphics candidates. SDL3 rendering exposes hardware-accelerated and fallback drivers, and its logical presentation and integer-scaling modes can be investigated for viewport/input handling. **Do not inadvertently force a 320:400 physical aspect ratio** when the target presentation is 4:3.

An eventual GPU-enhanced world layer is a separate rendering implementation behind the same interface. **Candidates remain SDL3+bgfx and SDL3+SDL_GPU, plus a Godot GDExtension evaluation**. OpenGL support could benefit older hardware: SDL_GPU itself officially supports Vulkan, Direct3D12 and Metal, but not OpenGL; bgfx supports a wider set of render APIs. This hardware/backends trade-off must be tested on real machines. The engine's source gameplay/saves/ticks/RNG must never take a dependency on either GPU runtime.

## Two-stage accuracy experiment

1. **Baseline presentation parity:** feed the source `IndexedFrame` through the existing `CpuReferencePresenter` to generate a byte-defined RGBA8 buffer using `(6-bit channel*255+31)/63`. Upload *the same RGBA bytes* into each GPU backend, sample 1:1 nearest, then independently read back an offscreen **320×400 RGBA8** target before screen scaling or color management. Compare exact 512000 bytes with the CPU reference via `tools/compare_renderer_candidates.py`. This conversion is a **diagnostic host expansion**, not proof of original VGA analog gamma or DAC timing.
2. **Palette-on-GPU pipeline:** only once step 1 passes, upload 320×400 indices and original 256×1 DAC palette as two resources. Implement a shader that chooses a palette entry with **precise integer/nearest addressing** and the same channel expansion. Compare against exactly the same CPU reference, including palette mutation with **unchanged source index bytes**. Floating-point normalized texture sampling must not accidentally interpolate adjacent indices; reject even a single mismatched pixel. Original DOS palette fidelity is still an independent gate.
3. Ensure the renderer does not mutate input source pixels, palette, original virtual world, IRQ/ticks or RNG. Read-back and benchmarking only on a **non-gameplay capture host**. Present normal game-frame output separately.
4. Measure nearest scaling at multiple window sizes, aspect ratio, DPI, minimized/restored window, device-loss recovery and GPU fallback. Do not compare scaled screenshot pixels to a 1:1 offscreen capture.
5. Each candidate records actual GPU driver, API, device, shader format/binary version, texture format, command-buffer timing, per-frame timestamps, peak memory and cold build details. Benchmark with a fixed warmup window and at least **120** representative frames after warmup; report VSync-on and VSync-off separately, distinguish CPU submit time from GPU completion, and use the **same hardware** for relative rankings.

## SDL_GPU implementation witness

Official SDL3 GPU API documents `SDL_CreateGPUDevice`, `SDL_CreateGPUTexture`, transfer buffers, `SDL_UploadToGPUTexture`, and `SDL_DownloadFromGPUTexture`. Device creation requires declaring supported shader formats such as SPIR-V, DXIL, MSL/metallib, and the chosen driver must be supported on the test machine. Texture formats/usages should be checked against capabilities. Render to an offscreen RGBA8 color target, enqueue a GPU-to-transfer-buffer download, **submit and wait for the fence**, then map/read bytes. **An immediately queued download is not completed readback.** Shader distribution must be reproducible (offline `SDL_shadercross` or pinned equivalent).

Source APIs:
- https://wiki.libsdl.org/SDL3/CategoryGPU
- https://wiki.libsdl.org/SDL3/SDL_CreateGPUDevice
- https://wiki.libsdl.org/SDL3/SDL_UploadToGPUTexture
- https://wiki.libsdl.org/SDL3/SDL_DownloadFromGPUTexture
- https://wiki.libsdl.org/SDL3/SDL_GPUShaderFormat
- https://wiki.libsdl.org/SDL3/SDL_SetRenderLogicalPresentation

## bgfx implementation witness

Use a supported texture format, submit the same source RGBA/palette pipeline through `createTexture2D`, `updateTexture2D`, and a fullscreen offscreen target. For comparison, **do not call `readTexture` on the ordinary sampled/rendered texture and assume it works**: the documented API requires a readback-enabled texture (subject to `BGFX_CAPS_TEXTURE_READ_BACK`), which is separate from the normal GPU resource; a supported blit/copy path may be required. `bgfx::readTexture` reports **the frame number when results become available**. Enforce that frame boundary before comparing output. The renderer's `frame`/render thread cadence may also differ from CPU submission.

Source API: https://bkaradzic.github.io/bgfx/bgfx.html (texture creation/upload, blit, readback, caps and frame documentation).

## Benchmark acceptance and framework selection

| Gate | Pass evidence |
| --- | --- |
| F0 | All candidate source input indices and DAC palette have the identical SHA256 and source commit; current C++17 CPU reference passes. |
| F1 | Each candidate has a **real compiled host** and source-identified distributable/proof executable; no fake GPU metadata. |
| F2 | Offscreen true GPU readback matches CPU reference byte-for-byte at 320×400, including palette-only changes. |
| F3 | Two candidates benchmarked under same device and API mode where possible; median/p95 frametimes, first-frame latency, peak memory, shader and build complexity recorded. |
| F4 | SDL3 Renderer fallback evaluated for original faithful 2D path regardless of advanced GPU selection. |
| F5 | Godot native-extension proof compiles same source index buffer + original GameSource and documents editor/tooling/ABI costs before inclusion. |
| F6 | Renderer choice documented with quantified Windows and at least one other host result, license/version pins, known driver limitations and explicit reversal strategy. |

**Current proof status:** F0 supported by local CPU/oracle tests; **F1–F6 not yet accepted**. The declared `capture_kind=GPU_READBACK` field in the comparator is untrusted user metadata, not hardware attestation. Never select a permanent GPU stack from synthetic fixtures. No Windows CI was dispatched, no copyright assets bundled, no gameplay or DOS semantics changed. Full faithful original-game recreation remains first; modding/extended content is an architecture provision only.
