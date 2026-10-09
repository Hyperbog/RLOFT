namespace DosDis.Core;

/// <summary>
/// Explicit analyst-selected, headerless, single-segment 16-bit load model.
/// A COM filename or a plausible opcode prefix is NOT evidence of this mode.
/// </summary>
public sealed class DosComImage
{
    private readonly BinaryArtifact _artifact;
    private DosComImage(BinaryArtifact artifact, ushort origin)
    {
        _artifact = artifact;
        Origin = origin;
    }

    public string SourceSha256 => _artifact.Sha256;
    public ushort Origin { get; }
    public int FileBackedImageLength => _artifact.Length;

    public static DosComImage LoadExplicit(BinaryArtifact artifact, ushort origin = 0x100)
    {
        ArgumentNullException.ThrowIfNull(artifact);
        if (BinaryFormatDetector.Inspect(artifact).FormatId != "unknown-headerless")
            throw new NotSupportedException(
                "Explicit COM mode cannot silently override a recognized container or executable header.");
        if (artifact.Length == 0 || artifact.Length > 0x10000 - (int)origin)
            throw new InvalidDataException(
                "Headerless image does not fit into one 16-bit segment at the declared origin.");
        return new DosComImage(artifact, origin);
    }

    public long? ToFileOffset(ushort offset)
    {
        if (offset < Origin) return null;
        int candidate = offset - Origin;
        return candidate < _artifact.Length ? candidate : null;
    }

    public IReadOnlyList<Dos16Instruction> Decode(ushort offset, int count = 32)
    {
        long? physical = ToFileOffset(offset);
        if (physical is null)
            throw new ArgumentOutOfRangeException(nameof(offset),
                "Requested offset is outside the explicitly mapped headerless source.");
        return X86RealModeDecoder.Decode(_artifact, (int)physical.Value,
            _artifact.Length - (int)physical.Value, 0, offset, count);
    }
}
