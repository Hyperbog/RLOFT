using System.Buffers.Binary;

namespace DosDis.Core;

/// <summary>
/// One DOS MZ loader relocation in original table order. The stored word is
/// preserved; each applied write adds the caller-assumed load segment mod 65536.
/// </summary>
public sealed record DosMzRelocationWrite(
    int Ordinal, ushort SourceSegment, ushort SourceOffset,
    int SourceImageOffset, long OriginalFileOffset,
    ushort OriginalStoredWord, ushort ValueBeforeWrite, ushort ValueAfterWrite);

public sealed record DosMzRuntimeMapping(
    ushort AssumedLoadSegment, ushort RuntimeSegment, ushort Offset,
    ushort RelativeSegment, int ImageOffset, long OriginalFileOffset,
    string Evidence);

public sealed record DosMzRebaseSummary(
    string SourceSha256, int OriginalFileBackedBytes, ushort AssumedLoadSegment,
    ushort AssumedEntryCs, ushort AssumedStackSs, ushort InitialIp, ushort InitialSp,
    int RelocationWriteCount, bool OverlappingRelocationWrites,
    string AnalysisStatus, IReadOnlyList<DosMzRelocationWrite> Writes);

/// <summary>
/// Independent, non-destructive simulation of DOS MZ fixup additions.
/// This is not a DOS process image: no PSP, BSS, DOS interrupts, overlay
/// manager or actual loader placement is supplied.
/// </summary>
public sealed class DosMzRebasedView
{
    private readonly byte[] _relocated;
    private readonly int _originalImageStart;
    private DosMzRebasedView(byte[] relocated, int originalImageStart,
        DosMzRebaseSummary summary)
    {
        _relocated = relocated;
        _originalImageStart = originalImageStart;
        Summary = summary;
    }

    public DosMzRebaseSummary Summary { get; }

    public static DosMzRebasedView Simulate(DosMzImage image, ushort assumedLoadSegment)
    {
        ArgumentNullException.ThrowIfNull(image);
        byte[] copied = image.ReadStoredImageBytes(0, image.FileBackedImageLength);
        byte[] original = (byte[])copied.Clone();
        var touched = new HashSet<int>();
        bool overlapping = false;
        var writes = new List<DosMzRelocationWrite>(image.Relocations.Count);
        int ordinal = 0;
        foreach (var record in image.Relocations)
        {
            int at = checked((int)(record.SourceFileOffset - image.LoadImageFileOffset));
            if (at < 0 || (long)at + 2 > copied.Length)
                throw new InvalidDataException("MZ relocation source exceeds original file-backed load image.");
            if (!touched.Add(at) || !touched.Add(at + 1))
                overlapping = true;
            ushort stored = BinaryPrimitives.ReadUInt16LittleEndian(original.AsSpan(at, 2));
            ushort before = BinaryPrimitives.ReadUInt16LittleEndian(copied.AsSpan(at, 2));
            ushort after = unchecked((ushort)(before + assumedLoadSegment));
            BinaryPrimitives.WriteUInt16LittleEndian(copied.AsSpan(at, 2), after);
            writes.Add(new DosMzRelocationWrite(ordinal++,
                record.Segment, record.Offset, at, record.SourceFileOffset,
                stored, before, after));
        }

        var summary = new DosMzRebaseSummary(image.SourceSha256, copied.Length,
            assumedLoadSegment,
            unchecked((ushort)(assumedLoadSegment + image.InitialCs)),
            unchecked((ushort)(assumedLoadSegment + image.InitialSs)),
            image.InitialIp, image.InitialSp,
            writes.Count, overlapping,
            overlapping
                ? "simulated-MZ-relocations;overlapping-fixups;runtime-base-assumed;not-verified"
                : "simulated-MZ-relocations;runtime-base-assumed;not-verified",
            writes);
        return new DosMzRebasedView(copied, image.LoadImageFileOffset, summary);
    }

    public ushort ReadRelocatedWord(int imageOffset)
    {
        if (imageOffset < 0 || (long)imageOffset + 2 > _relocated.Length)
            throw new ArgumentOutOfRangeException(nameof(imageOffset));
        return BinaryPrimitives.ReadUInt16LittleEndian(_relocated.AsSpan(imageOffset, 2));
    }

    /// <summary>
    /// Maps a caller-supplied runtime segment into the file-backed original,
    /// refusing selector wraparound or virtual-only regions. This does not
    /// assert that a DOS runtime used the assumed load segment.
    /// </summary>
    public DosMzRuntimeMapping? MapRuntimeFarPointer(ushort selector, ushort offset)
    {
        ushort load = Summary.AssumedLoadSegment;
        if (selector < load) return null; // wrapped segment aliases need A20/runtime evidence
        ushort relative = checked((ushort)(selector - load));
        long imageOffset = (long)relative * 16 + offset;
        if (imageOffset >= _relocated.Length) return null;
        return new DosMzRuntimeMapping(load, selector, offset, relative,
            checked((int)imageOffset), _originalImageStart + imageOffset,
            "MZ-relocation-simulation;nonwrapped-same-module;file-backed;runtime-unverified");
    }
}
