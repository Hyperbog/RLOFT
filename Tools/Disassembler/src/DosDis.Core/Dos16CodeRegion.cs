namespace DosDis.Core;

/// <summary>
/// An explicit, original-file-backed 16-bit code hypothesis. Segments are
/// caller-supplied load assumptions, never inferred process addresses.
/// </summary>
public sealed class Dos16CodeRegion
{
    private readonly BinaryArtifact _source;

    internal Dos16CodeRegion(BinaryArtifact source, int fileOffset, int byteLength,
        ushort segment, ushort origin, string mappingEvidence)
    {
        ArgumentNullException.ThrowIfNull(source);
        if (fileOffset < 0 || byteLength < 1 ||
            (long)fileOffset + byteLength > source.Length ||
            (long)origin + byteLength > 0x10000 ||
            string.IsNullOrWhiteSpace(mappingEvidence))
            throw new InvalidDataException("16-bit code hypothesis is unbacked or crosses the IP boundary.");
        _source = source;
        FileOffset = fileOffset;
        ByteLength = byteLength;
        Segment = segment;
        Origin = origin;
        MappingEvidence = mappingEvidence;
    }

    public string SourceSha256 => _source.Sha256;
    public int FileOffset { get; }
    public int ByteLength { get; }
    public ushort Segment { get; }
    public ushort Origin { get; }
    public int EndExclusive => Origin + ByteLength;
    public string MappingEvidence { get; }

    internal (int PhysicalOffset, int Remaining) Map(ushort offset)
    {
        if (offset < Origin || offset >= EndExclusive)
            throw new ArgumentOutOfRangeException(nameof(offset));
        int delta = offset - Origin;
        return (FileOffset + delta, ByteLength - delta);
    }

    internal BinaryArtifact Source => _source;
}
