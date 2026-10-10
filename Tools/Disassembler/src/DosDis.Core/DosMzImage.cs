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
        IReadOnlyList<DosMzRelocation> relocations, bool isLauncherStub)
    {
        _original = artifact;
        LoadImageFileOffset = imageStart;
        FileBackedImageLength = imageLength;
        DeclaredExecutableFileLength = declaredFileBytes;
        InitialCs = cs; InitialIp = ip; InitialSs = ss; InitialSp = sp;
        Relocations = relocations;
        IsLauncherStub = isLauncherStub;
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
    public bool IsLauncherStub { get; }
    public bool HasTrailingOverlayData => DeclaredExecutableFileLength < _original.Length;

    public static DosMzImage Load(BinaryArtifact artifact) =>
        LoadCore(artifact, isLauncherStub: false);

    /// <summary>
    /// Explicitly interpret only the outer DOS MZ declared load module,
    /// rejecting any declared byte extent overlapping an embedded program.
    /// It does not identify or decode the protected-mode module.
    /// </summary>
    public static DosMzImage LoadLauncherStub(BinaryArtifact artifact) =>
        LoadCore(artifact, isLauncherStub: true);

    private static DosMzImage LoadCore(BinaryArtifact artifact, bool isLauncherStub)
    {
        ArgumentNullException.ThrowIfNull(artifact);
        var bytes = artifact.Data;
        string type = BinaryFormatDetector.Inspect(artifact).FormatId;
        if (!isLauncherStub && type != "mz")
            throw new NotSupportedException(
                "Standalone MZ mode rejects nested protected-mode images; use the explicit DOS launcher mode.");
        if (isLauncherStub && type is not
            ("le-signature" or "lx" or "ne" or "pe" or "ambiguous-mz-extended"))
            throw new NotSupportedException(
                "DOS launcher mode requires a source-backed nested MZ-bound extended module.");
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
        if (isLauncherStub)
        {
            var locations = MzExtendedModuleLocator.Scan(artifact);
            // A file whose outer MZ itself links to an extended module is
            // not an independent DOS launcher, even if another nested
            // module signature occurs later in the same source.
            var nested = locations.Where(x => x.ModuleOrigin > 0).ToArray();
            if (nested.Length == 0 ||
                locations.Any(x => x.ModuleOrigin == 0) ||
                (long)declared > nested.Min(x => x.ModuleOrigin))
                throw new InvalidDataException(
                    "Outer DOS MZ is not independent of the bounded nested extended module.");
        }
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
        return new DosMzImage(artifact, headerBytes, loadLength, declared,
            cs, ip, ss, sp, fixups, isLauncherStub);
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
            IsLauncherStub
                ? "explicit-outer-DOS-MZ-launcher;module-relative-CS;protected-mode-module-excluded"
                : "MZ-module-relative-CS;file-backed;runtime-base-and-relocations-not-applied");
    }

    /// <summary>Initial CS load window, not a verified code-section inventory.</summary>
    public Dos16CodeRegion CreateEntryCodeRegion() => CreateCodeRegion(InitialCs);

    /// <summary>
    /// Analyst-requested 16-bit linear decode. Not proof of a function,
    /// reachable entry or DOS runtime relocation state.
    /// </summary>
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
