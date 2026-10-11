# Native DOS keyboard session and hardware IRQ — 2026-10-11

## Source-backed integration

`OriginalDosKeyboardSession::from_installed_code2` reads the
original 89-DWORD `F590` key table from installation-media
`CODE.2` and holds a single mutable IRQ state. `receive_port60`
models original dispatcher `30950`; `input_ready`, `peek_key`
and `consume_key` call the separate original services
`30AD8`, `30B40`, and `30AEC`. Held keys, modifiers and
Space latch retain distinct original behavior.

The native C++17 test exercises shared state across make/peek/consume,
release, an injected input-producing draw callback, repeated polling,
modifier and Space flags and rejecting unsupported CODE.2 size.
It passed GCC 14 `-std=c++17 -O2 -Wall -Wextra -Wpedantic -Werror`
in a local harness using interface-compatible copies of the
previously developed keyboard and feedback services.
It is **not wired** to ordinary Win32 event processing and is not
an exact current-SHA Windows full-library build.

## Authentic original keyboard hardware prelude

Original CODE.2 is 558,689 bytes, SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
The original `308F8..3094F` 88-byte entry/acknowledgement
wrapper has SHA-256
`bb2b244478a8d5f4e6fc4a8027d1c605b76a3d59cc851de8888886de18064a89`.

The ISR saves registers and segments, calls `3566C`,
reads keyboard data at **port 0x60** and status at
**port 0x61**, stores status in `F570`, then at
`30925` compares the **port-61 value** with `0x80`.
It conditionally resamples port 0x61 and acknowledges through
port-61 writes. At `30950` it enters the scan dispatcher,
where a *different* `0x80` comparison applies to the
**port-60 scan byte**. Neither comparison proves E0/E1
extended-key prefix translation.

The special exits at `30A6D` and `30A94` and the
normal exit at `30AB9` issue end-of-interrupt to
PIC **port 0x20**, restore the CPU state and execute IRET.
The optional original-byte oracle passed locally: full
88-byte SHA, **30 opcode windows**, near E8
`30902→3566C`, and all three IRET paths.

## M-PLAY-01 gates still open

Win32 keyboard mapping, E0/E1 handling, installed DOS ISR,
IRQ scheduling, F0C8 graphics callback, memory selectors and
actual playthrough parity are unverified. The source-only CTest
does not fetch original game media and is not a substitute
for current-SHA Windows Full CI. No Windows CI was dispatched
and M-PLAY-01 remains unaccepted.
