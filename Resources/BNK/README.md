# BNK resources

The supplied `GM1.BNK` and `GM2.BNK` files are now identified as **Sierra Semiconductor ARIA General MIDI banks** used by the Miles AIL ARIA XMIDI driver family.

They are third-party audio-hardware support assets, not DreamForge RES-style game-resource archives and not AdLib Instrument Maker `.BNK` files.

Ravenloft ships the corresponding 32-bit Miles ARIA XMIDI driver `A32ARXM.DLL`. Period ARIA documentation assigns:

- `GM1.BNK` to 512 KiB / 0.5 MiB ROM configurations;
- `GM2.BNK` to 1 MiB ROM configurations.

See `ARIA_BANKS.md` for evidence, preservation boundaries, and the distinction between confirmed component role and still-unknown internal record grammar.
