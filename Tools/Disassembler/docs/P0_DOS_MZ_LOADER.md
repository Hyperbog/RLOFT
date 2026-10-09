# Standalone 16-bit DOS MZ loader (source checkpoint)

`DosMzImage` validates the MZ page-count/last-page-byte model, header
paragraph count, relocation table bounds, original-file length, file-backed
load image and initial CS:IP. Relocation entries retain their exact original
source-file offsets. `ToFileOffset(segment, offset)` never invents image data
outside the declared DOS load module. The original bytes remain immutable.

An explicitly requested `Decode(segment, offset, count)` uses iced-x86
**16-bit mode**, stores original instruction bytes and file offsets, and
labels the entire result as a candidate. There is no recursive MZ CFG,
relocated runtime image, PSP segment assignment, dynamic loading, overlay
mapping, BSS reconstruction, DOS COM assumption or verified callback model.

Extended LE/LX/NE/PE MZ wrappers are deliberately refused by the standalone
MZ loader until their own object/segment metadata is validated.
The loader does not trust `.EXE` extensions as evidence.

No Windows build or synthetic regression execution has been requested.
