using System.Buffers.Binary;
using System.Text;
using System.Text.Json;

namespace DosDis.Core;

/// <summary>Container envelope only. Headers do not establish code regions.</summary>
public sealed record DosOverlayEnvelope(
    string Format, string Variant, int HeaderBytes, int PayloadBytes,
    string SourceSha256, int OriginalFileLength, string Confidence);

/// <summary>
/// Explicit analyst evidence for ONE source, not inferred overlay segments.
/// Offsets are physical file offsets; segment and IP are assumed load context.
/// </summary>
public sealed record DosOverlayRegion(
    string Id, int FileOffset, int ByteLength, ushort AssumedSegment,
    ushort AssumedOffset, string Role, string EvidenceId);
public sealed record DosOverlayRegionManifest(
    int SchemaVersion, string SourceSha256, int SourceByteLength,
    string Format, IReadOnlyList<DosOverlayRegion> Regions);

public sealed class DosOverlayImage
{
    private readonly BinaryArtifact _source;
    private DosOverlayImage(BinaryArtifact source, DosOverlayEnvelope envelope)
    {
        _source = source;
        Envelope = envelope;
    }

    public DosOverlayEnvelope Envelope { get; }

    public static DosOverlayImage LoadEnvelope(BinaryArtifact artifact)
    {
        ArgumentNullException.ThrowIfNull(artifact);
        ReadOnlySpan<byte> bytes = artifact.Data;
        if (bytes.Length < 4)
            throw new InvalidDataException("Overlay envelope is shorter than its signature.");
        string type = bytes.StartsWith("FBOV"u8) ? "fbov" :
            bytes.StartsWith("TPOV"u8) ? "tpov" :
            throw new NotSupportedException("Only signature-confirmed FBOV/TPOV overlay envelopes are recognized.");

        int headerBytes;
        string variant;
        if (type == "tpov")
        {
            headerBytes = 4;
            variant = "tpov-opaque-body";
        }
        else
        {
            if (bytes.Length < 8)
                throw new InvalidDataException("FBOV header has no bounded length field.");
            uint declared = BinaryPrimitives.ReadUInt32LittleEndian(bytes.Slice(4, 4));
            bool simple = (ulong)declared + 8 == (ulong)bytes.Length;
            bool extended = (ulong)declared + 16 == (ulong)bytes.Length;
            if (simple)
            {
                headerBytes = 8;
                variant = "fbov-eight-byte-envelope";
            }
            else if (extended && bytes.Length >= 16)
            {
                headerBytes = 16;
                variant = "fbov-sixteen-byte-envelope";
            }
            else
            {
                throw new InvalidDataException(
                    "FBOV declared payload length does not identify a complete supported envelope. Layout remains unknown.");
            }
        }
        return new DosOverlayImage(artifact,
            new DosOverlayEnvelope(type, variant, headerBytes,
                artifact.Length - headerBytes, artifact.Sha256, artifact.Length,
                "envelope-validated;internal-segments-unverified"));
    }

    public static DosOverlayRegionManifest ReadRegionManifest(string filename)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(filename);
        var file = new FileInfo(filename);
        if (file.Length > 1024 * 1024)
            throw new InvalidDataException("Overlay region manifest exceeds 1 MiB.");
        return ParseRegionManifest(File.ReadAllText(filename));
    }

    public static DosOverlayRegionManifest ParseRegionManifest(string json)
    {
        ArgumentNullException.ThrowIfNull(json);
        if (Encoding.UTF8.GetByteCount(json) > 1024 * 1024)
            throw new InvalidDataException("Overlay region manifest exceeds 1 MiB.");
        try
        {
            return JsonSerializer.Deserialize<DosOverlayRegionManifest>(
                json, new JsonSerializerOptions(JsonSerializerDefaults.Web))
                ?? throw new InvalidDataException("Overlay region manifest is empty.");
        }
        catch (JsonException e)
        {
            throw new InvalidDataException("Overlay region manifest JSON is invalid.", e);
        }
    }

    public IReadOnlyList<DosOverlayRegion> ValidateRegions(DosOverlayRegionManifest manifest)
    {
        ArgumentNullException.ThrowIfNull(manifest);
        if (manifest.SchemaVersion != 1 ||
            manifest.SourceSha256 is not { Length: 64 } ||
            !manifest.SourceSha256.All(c => c is >= '0' and <= '9' or >= 'a' and <= 'f' or >= 'A' and <= 'F') ||
            !string.Equals(manifest.SourceSha256, _source.Sha256, StringComparison.OrdinalIgnoreCase) ||
            manifest.SourceByteLength != _source.Length ||
            manifest.Format != Envelope.Format ||
            manifest.Regions is null ||
            manifest.Regions.Count is < 1 or > 1024)
            throw new InvalidDataException("Overlay region manifest does not match the exact original source or schema.");

        var byFileStart = new List<DosOverlayRegion>(manifest.Regions.Count);
        var ids = new HashSet<string>(StringComparer.Ordinal);
        foreach (var region in manifest.Regions)
        {
            if (region is null ||
                string.IsNullOrWhiteSpace(region.Id) || region.Id.Length > 128 ||
                !ids.Add(region.Id) ||
                region.FileOffset < Envelope.HeaderBytes || region.ByteLength <= 0 ||
                (long)region.FileOffset + region.ByteLength > _source.Length ||
                (long)region.AssumedOffset + region.ByteLength > 0x10000 ||
                region.Role is not ("code-candidate" or "data-candidate") ||
                string.IsNullOrWhiteSpace(region.EvidenceId) || region.EvidenceId.Length > 512)
                throw new InvalidDataException("Overlay mapping contains an unbacked, overlapping, or unsupported region.");
            byFileStart.Add(region);
        }
        byFileStart.Sort((a,b) => a.FileOffset.CompareTo(b.FileOffset));
        for (int i = 1; i < byFileStart.Count; i++)
            if ((long)byFileStart[i-1].FileOffset + byFileStart[i-1].ByteLength >
                byFileStart[i].FileOffset)
                throw new InvalidDataException("Overlay regions overlap in the original source file.");
        return byFileStart;
    }

    /// <summary>Read-only, analyst-provided code region for 16-bit CFG analysis.</summary>
    public Dos16CodeRegion CreateCodeRegion(
        DosOverlayRegionManifest manifest, string regionId)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(regionId);
        var region = ValidateRegions(manifest).SingleOrDefault(r => r.Id == regionId)
            ?? throw new ArgumentException("Unknown source-backed overlay region.", nameof(regionId));
        if (region.Role != "code-candidate")
            throw new NotSupportedException("Protected overlay data cannot become CFG code implicitly.");
        return new Dos16CodeRegion(_source, region.FileOffset, region.ByteLength,
            region.AssumedSegment, region.AssumedOffset,
            "analyst-source-hash-bound-overlay-region:" + region.EvidenceId);
    }

    public IReadOnlyList<Dos16Instruction> DecodeExplicit(
        DosOverlayRegionManifest manifest, string regionId,
        ushort regionRelativeOffset, int instructionCount)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(regionId);
        var regions = ValidateRegions(manifest);
        var region = regions.SingleOrDefault(r => r.Id == regionId)
            ?? throw new ArgumentException("Unknown overlay region ID.", nameof(regionId));
        if (region.Role != "code-candidate")
            throw new NotSupportedException("A data-marked overlay region cannot be decoded without an explicit code mapping.");
        if (regionRelativeOffset >= region.ByteLength)
            throw new ArgumentOutOfRangeException(nameof(regionRelativeOffset));
        return X86RealModeDecoder.Decode(_source,
            checked(region.FileOffset + regionRelativeOffset),
            region.ByteLength - regionRelativeOffset, region.AssumedSegment,
            checked((ushort)(region.AssumedOffset + regionRelativeOffset)), instructionCount);
    }
}
