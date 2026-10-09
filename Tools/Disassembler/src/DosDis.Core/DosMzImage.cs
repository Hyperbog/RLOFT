using System.Buffers.Binary;

namespace DosDis.Core;

public sealed record DosMzRelocation(ushort Segment, ushort Offset, long SourceFileOffset);
/// <summary>
/// Validates the on-disk MZ load module and reports original-byte addresses.
/// No PSP address, runtime segment base, BSS, or relocation values are guessed.
/// </summary>
public sealed class DosMzImage
{
    private readonly BinaryArtifact _original;
    private DosMzImage(BinaryArtifact artifact, int imageStart, int imageLength,
        int declaredFileBytes, ushort cs, ushort ip, ushort ss, ushort sp,
        IReadOnlyList<DosMzRelocation> relocations)
    {
        _original = artifact;
        LoadImageFileOffset = imageStart;
        FileBackedImageLength = imageLength;
        DeclaredExecutableFileLength = declaredFileBytes;
        InitialCs = cs; InitialIp = ip; InitialSs = ss; InitialSp = sp;
        Relocations = relocations;
    }

    public string SourceSha256 => _original.Sha256;
    public int LoadImageFileOffset { get; }
    public int FileBackedImageLength { get; }
    public int DeclaredExecutableFileLength { get; }
    public ushort InitialCs { get; }
    public ushort InitialIp { get; }
    public ushort InitialSs { get; }
    public ushort InitialSp { get; }
    public IReadOnlyList<DosMzRelocation> Relocations { get; }
    public bool HasTrailingOverlayData => DeclaredExecutableFileLength < _original.Length;

    public static DosMzImage Load(BinaryArtifact artifact)
    {
        ArgumentNullException.ThrowIfNull(artifact);
        var bytes = artifact.Data;
        // A bound LE/LX/NE/PE module is not to be relabeled as a DOS
        // real-mode program merely because its wrapper begins with MZ.
        string type = BinaryFormatDetector.Inspect(artifact).FormatId;
        if (type != "mz")
            throw new NotSupportedException("A standalone MZ loader requires a DOS MZ header without an identified extended module.");
        if (bytes.Length < 0x1C)
            throw new InvalidDataException("Truncated MZ header.");
        ushort Read(int offset) => BinaryPrimitives.ReadUInt16LittleEndian(artifact.Data.Slice(offset, 2));
        ushort lastPageBytes = Read(2), pages = Read(4), relocationCount = Read(6);
        ushort headerParagraphs = Read(8), ss = Read(0x0E), sp = Read(0x10);
        ushort ip = Read(0x14), cs = Read(0x16), relocTable = Read(0x18);
        if (pages == 0 || lastPageBytes > 511 || headerParagraphs < 2)
            throw new InvalidDataException("Invalid MZ page or header geometry.");
        int declared = checked((pages - 1) * 512 + (lastPageBytes == 0 ? 512 : lastPageBytes));
        int headerBytes = checked(headerParagraphs * 16);
        if (headerBytes > declared || declared > bytes.Length)
            throw new InvalidDataException("MZ header, declared length, or original file is inconsistent.");
        if (relocationCount != 0 &&
            ((long)relocTable < 0x1C || (long)relocTable + relocationCount * 4 > headerBytes))
            throw new InvalidDataException("MZ relocation table exceeds the declared executable header.");
        int loadLength = declared - headerBytes;
        var fixups = new List<DosMzRelocation>(relocationCount);
        for (int i = 0; i < relocationCount; i++)
        {
            int position = relocTable + i * 4;
            ushort offset = Read(position), segment = Read(position + 2);
            long address = (long)segment * 16 + offset;
            if (address + 2 > loadLength)
                throw new InvalidDataException("MZ relocation points outside the file-backed load module.");
            fixups.Add(new DosMzRelocation(segment, offset, headerBytes + address));
        }
        long entry = (long)cs * 16 + ip;
        if (entry >= loadLength)
            throw new InvalidDataException("MZ initial CS:IP has no file-backed instruction byte; manual loader context required.");
        return new DosMzImage(artifact, headerBytes, loadLength, declared, cs, ip, ss, sp, fixups);
    }

    /// <summary>
    /// Non-mutating fixup simulation for a caller-assumed DOS load segment.
    /// </summary>
    public DosMzRebasedView SimulateRelocations(ushort assumedLoadSegment) =>
        DosMzRebasedView.Simulate(this, assumedLoadSegment);

    /// <summary>
    /// Read source-backed resident load-module bytes for independent metadata
    /// inspection. This never applies relocations or supplies a runtime base.
    /// </summary>
    public byte[] ReadStoredImageBytes(int imageOffset, int length)
    {
        if (imageOffset < 0 || length < 0 ||
            (long)imageOffset + length > FileBackedImageLength)
            throw new ArgumentOutOfRangeException(nameof(imageOffset),
                "MZ load-module read exceeds original-file-backed bytes.");
        return _original.Data.Slice(checked(LoadImageFileOffset + imageOffset), length).ToArray();
    }

    /// <summary>
    /// Returns null for unbacked or non-image offsets. The value is the
    /// original file offset; no inferred process segment base is included.
    /// </summary>
    public long? ToFileOffset(ushort segment, ushort offset)
    {
        long linear = (long)segment * 16 + offset;
        return linear < FileBackedImageLength ? LoadImageFileOffset + linear : null;
    }

    /// <summary>
    /// Analyst-requested 16-bit linear decode. Not proof of a function,
    /// reachable entry or DOS runtime relocation state.
    /// </summary>
    /// <summary>
    /// A candidate 64 KiB instruction window in the stored DOS MZ load image,
    /// keyed by module-relative CS. The caller must supply any non-entry CS.
    /// </summary>
    public Dos16CodeRegion CreateCodeRegion(ushort relativeCodeSegment)
    {
        long start = (long)relativeCodeSegment * 16;
        if (start >= FileBackedImageLength)
            throw new InvalidDataException("Requested MZ segment is not file-backed.");
        int available = checked((int)Math.Min(0x10000L, FileBackedImageLength - start));
        return new Dos16CodeRegion(_original, checked((int)(LoadImageFileOffset + start)),
            available, relativeCodeSegment, 0,
            "MZ-module-relative-CS;file-backed;runtime-base-and-relocations-not-applied");
    }

    /// <summary>Initial CS load window, not a verified code-section inventory.</summary>
    public Dos16CodeRegion CreateEntryCodeRegion() => CreateCodeRegion(InitialCs);

    public IReadOnlyList<Dos16Instruction> Decode(
        ushort segment, ushort offset, int instructionCount = 32)
    {
        long linear = (long)segment * 16 + offset;
        if (linear >= FileBackedImageLength)
            throw new ArgumentOutOfRangeException(nameof(offset),
                "No file-backed bytes at requested MZ segment:offset.");
        return X86RealModeDecoder.Decode(_original,
            checked((int)(LoadImageFileOffset + linear)),
            checked((int)(FileBackedImageLength - linear)),
            segment, offset, instructionCount);
    }
}
