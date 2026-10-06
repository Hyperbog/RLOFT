# GM1.BNK / GM2.BNK — Sierra Semiconductor ARIA General MIDI banks

Status: **CONFIRMED at vendor/component-role level; inner bank grammar remains opaque.** The supplied `GM1.BNK` and `GM2.BNK` are Sierra Semiconductor ARIA sample/control banks used by the Miles AIL ARIA XMIDI driver family. They are not AdLib Instrument Maker `.BNK` files and should not be decoded with the AdLib BNK grammar used by Miles' `GLIB` conversion utility.

## Supplied Ravenloft files

| file | bytes | SHA-256 |
|---|---:|---|
| `GM1.BNK` | 23,907 | `843f012e7980e0a58722aa5c7a6e88b652f2f8d404834638cd994ca0869df5a8` |
| `GM2.BNK` | 24,273 | `b4af656059a6afba5db2da972c3506ca08d899ca04c65389e0bec7aaf6536339` |

Both begin with the byte signature:

```text
02 AF 00 32 07 02 4D 19 71 19 95 19 C5 19 F5 19
```

That signature is independently catalogued as an **ARIA samples Bank** signature. The two Ravenloft files share their first 77 bytes before their contents diverge.

## Hardware/driver role

Contemporary ARIA support documentation identifies:

- `GM1.BNK` — General MIDI bank for ARIA hardware with **0.5 MiB / 512 KiB ROM**;
- `GM2.BNK` — General MIDI bank for ARIA hardware with **1.0 MiB / 1024 KiB ROM**.

The same documentation names `ARIAXMID.ADV` as the 16-bit AIL extended-MIDI driver. Ravenloft's shipped 32-bit Miles driver set contains the corresponding `A32ARXM.DLL` and `A32ARDG.DLL` ARIA XMIDI/digital components.

This supplies a direct runtime explanation for why both `GM1.BNK` and `GM2.BNK` are shipped: the appropriate bank is selected for the installed ARIA hardware/ROM configuration.

Historical reference: `Aria Soundcard Support List v2.04` (1994), archived from `comp.sys.ibm.pc.soundcard.tech`.

Secondary format-signature confirmation: TrID's BNK definitions classify this exact leading signature as `ARIA samples Bank`.

## Vendor provenance

The publicly released Miles AIL 2.14 source tree also contains `GM1.BNK` and `GM2.BNK` alongside the AIL driver/tool source. Those reference files are close relatives but are **not byte-identical Ravenloft assets**: their recorded sizes differ from the supplied Ravenloft pair. Consequently Ravenloft's exact files remain the canonical evidence for this project.

The AIL release README identifies the package as John Miles / Miles Design's original AIL 2.14 and AIL/32 source release. This establishes that the filenames are part of the period Miles middleware ecosystem rather than DreamForge-native naming invented for Ravenloft.

## What is and is not reconstructed

### Confirmed

- both files are ARIA-family `.BNK` files;
- they are General MIDI support banks rather than AdLib/OPL GTLs;
- GM1/GM2 correspond to the two ARIA ROM-size configurations;
- Ravenloft ships the corresponding 32-bit Miles ARIA driver (`A32ARXM.DLL`);
- the common signature and exact Ravenloft file identities are preserved.

### Not yet claimed

- record boundaries inside the BNK bodies;
- sample/timbre field meanings;
- whether the Ravenloft variants were produced by SSI, Miles, Sierra, or another distributor revision;
- byte-level differences versus any other game's or AIL package's GM1/GM2 banks.

Those details require either the ARIA bank specification or direct reconstruction of the shipped `A32ARXM.DLL` consumer. Until then, inventing field names from byte patterns would reduce fidelity.

## Porting boundary

A faithful DOS-compatible preservation path should retain these files unchanged for the original ARIA driver path.

A modern native port does not need to emulate ARIA merely to play Ravenloft music if another supported synthesis backend is selected, but the banks remain part of the authentic hardware-support matrix and should be archived/documented as such.
