using System.Text;
using System.Text.Json;
using Iced.Intel;

namespace DosDis.Core;

/// <summary>
/// Predeclared x86-16 expectations obtained from an independently implemented
/// decoder. The supplied oracle is analyst-authored, not authenticated by
/// this reader; comparison never certifies its independence or runtime flow.
/// </summary>
public sealed record Dos16ExternalOracleManifest(
    int SchemaVersion, string GameId, string Module,
    string SourceSha256, int SourceByteLength,
    int RegionFileOffset, int RegionByteLength, ushort Segment, ushort Origin,
    string DecoderName, string DecoderVersion, string DecoderConfiguration,
    string OracleEvidenceId,
    IReadOnlyList<Dos16ExternalExpectedInstruction> ExpectedInstructions);

/// <summary>
/// Externally researched instruction identity and optional direct CFG edge.
/// FlowControl uses a canonical name from the Iced FlowControl vocabulary
/// for output normalization, not the external decoder's printed syntax.
/// </summary>
public sealed record Dos16ExternalExpectedInstruction(
    ushort Offset, long SourceFileOffset, string OriginalHex,
    string FlowControl, string EvidenceId,
    string? DirectEdgeKind = null, ushort? DirectTarget = null);

public sealed record Dos16ExternalOracleFinding(
    ushort Offset, string Status, string DetailCode, string EvidenceId);

public sealed record Dos16ExternalOracleReport(
    string Format, int SchemaVersion, string SourceSha256,
    string GameId, string Module, string DecoderName, string DecoderVersion,
    string OracleEvidenceId, string IndependenceStatus,
    string AnalysisStatus, bool Truncated, int Expected,
    int Matched, int Missing, int Conflicting, int NotObserved,
    int UnbenchmarkedCandidates,
    IReadOnlyList<Dos16ExternalOracleFinding> Findings);

/// <summary>
/// Read-only, exact-source x86-16 comparison against a separately prepared
/// external oracle. This is NOT a second decoder, independence attestation,
/// executable-code certification, or an automatic build/release gate.
/// </summary>
public static class Dos16ExternalOracleValidator
{
    public const int SchemaVersion = 1;
    private const int MaxManifestBytes = 512 * 1024;
    private const int MaxOutputBytes = 2 * 1024 * 1024;
    private const int MaxExpectations = 2048;
    private static readonly JsonSerializerOptions JsonOptions =
        new(JsonSerializerDefaults.Web) { WriteIndented = true };

    public static Dos16ExternalOracleManifest ReadManifest(string path)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(path);
        var info = new FileInfo(path);
        if (info.Length > MaxManifestBytes)
            throw new InvalidDataException("External x86-16 oracle manifest exceeds 512 KiB.");
        try
        {
            var manifest = JsonSerializer.Deserialize<Dos16ExternalOracleManifest>(
                File.ReadAllText(path), JsonOptions)
                ?? throw new InvalidDataException("External oracle manifest was empty.");
            ValidateManifest(manifest);
            return manifest;
        }
        catch (JsonException e)
        {
            throw new InvalidDataException("Malformed external x86-16 oracle JSON.", e);
        }
    }

    private static void ValidateManifest(Dos16ExternalOracleManifest manifest)
    {
        ArgumentNullException.ThrowIfNull(manifest);
        if (manifest.SchemaVersion != SchemaVersion ||
            manifest.SourceSha256 is null || manifest.SourceSha256.Length != 64 ||
            !manifest.SourceSha256.All(c =>
                c is >= '0' and <= '9' or >= 'a' and <= 'f' or >= 'A' and <= 'F') ||
            manifest.SourceByteLength < 1 ||
            manifest.RegionFileOffset < 0 || manifest.RegionByteLength < 1 ||
            (long)manifest.RegionFileOffset + manifest.RegionByteLength >
                manifest.SourceByteLength ||
            (long)manifest.Origin + manifest.RegionByteLength > 0x10000 ||
            string.IsNullOrWhiteSpace(manifest.GameId) ||
            string.IsNullOrWhiteSpace(manifest.Module) ||
            string.IsNullOrWhiteSpace(manifest.DecoderName) ||
            string.IsNullOrWhiteSpace(manifest.DecoderVersion) ||
            string.IsNullOrWhiteSpace(manifest.DecoderConfiguration) ||
            string.IsNullOrWhiteSpace(manifest.OracleEvidenceId) ||
            manifest.ExpectedInstructions is null ||
            manifest.ExpectedInstructions.Count is < 1 or > MaxExpectations)
            throw new InvalidDataException(
                "External x86-16 oracle lacks bounded, source-specific provenance.");

        // Prevent a built-in decoder result from being mislabeled as a
        // *different* decoder. Other names are still untrusted claims:
        // external origin and reviewer must be assessed outside this code.
        string decoder = manifest.DecoderName.Trim().ToLowerInvariant();
        if (decoder.Contains("iced", StringComparison.Ordinal) ||
            decoder.Contains("dosdis", StringComparison.Ordinal))
            throw new InvalidDataException(
                "An external oracle cannot identify the embedded DOSDis/Iced decoder.");

        if (manifest.ExpectedInstructions.Any(e => e is null))
            throw new InvalidDataException(
                "External x86-16 oracle contains a null instruction expectation.");
        var previousEnd = -1;
        foreach (var e in manifest.ExpectedInstructions.OrderBy(x => x.Offset))
        {
            if (e is null || string.IsNullOrWhiteSpace(e.EvidenceId) ||
                e.OriginalHex is null || e.OriginalHex.Length is < 2 or > 30 ||
                (e.OriginalHex.Length & 1) != 0 ||
                !e.OriginalHex.All(c =>
                    c is >= '0' and <= '9' or >= 'a' and <= 'f' or >= 'A' and <= 'F') ||
                !Enum.TryParse<FlowControl>(e.FlowControl, false, out var flow) ||
                !string.Equals(Enum.GetName(flow), e.FlowControl, StringComparison.Ordinal) ||
                e.Offset < manifest.Origin ||
                (long)e.Offset + e.OriginalHex.Length / 2 >
                    (long)manifest.Origin + manifest.RegionByteLength ||
                e.SourceFileOffset !=
                    (long)manifest.RegionFileOffset + e.Offset - manifest.Origin ||
                ((e.DirectEdgeKind is null) != (e.DirectTarget is null)) ||
                (e.DirectEdgeKind is not null && e.DirectEdgeKind is not
                    ("branch" or "branch-taken" or "call")) ||
                (e.DirectEdgeKind is not null &&
                    e.DirectEdgeKind != (flow switch
                    {
                        FlowControl.Call => "call",
                        FlowControl.UnconditionalBranch => "branch",
                        FlowControl.ConditionalBranch => "branch-taken",
                        _ => null
                    })) ||
                e.Offset < previousEnd)
                throw new InvalidDataException(
                    "External x86-16 oracle has an invalid, duplicated, or overlapping instruction.");
            previousEnd = e.Offset + e.OriginalHex.Length / 2;
        }
    }

    public static Dos16ExternalOracleReport Compare(
        Dos16CodeRegion region, Dos16FlowResult graph,
        Dos16ExternalOracleManifest manifest)
    {
        ArgumentNullException.ThrowIfNull(region);
        ArgumentNullException.ThrowIfNull(graph);
        ValidateManifest(manifest);
        if (!string.Equals(manifest.SourceSha256, region.SourceSha256,
                StringComparison.OrdinalIgnoreCase) ||
            manifest.SourceByteLength != region.Source.Length ||
            manifest.RegionFileOffset != region.FileOffset ||
            manifest.RegionByteLength != region.ByteLength ||
            manifest.Origin != region.Origin ||
            manifest.Segment != region.Segment ||
            !string.Equals(graph.SourceSha256, region.SourceSha256,
                StringComparison.OrdinalIgnoreCase) ||
            graph.FileOffset != region.FileOffset ||
            graph.RegionByteLength != region.ByteLength ||
            graph.MappingEvidence != region.MappingEvidence ||
            graph.Entry.Segment != region.Segment)
            throw new InvalidDataException(
                "External x86-16 oracle/CFG/source region identity mismatch.");

        var decoded = new Dictionary<ushort, Dos16FlowInstruction>();
        foreach (var instruction in graph.Instructions)
            if (instruction is null ||
                instruction.Address.Segment != region.Segment ||
                !decoded.TryAdd(instruction.Address.Offset, instruction))
                throw new InvalidDataException(
                    "External oracle comparison requires unique same-segment CFG rows.");

        var findings = new List<Dos16ExternalOracleFinding>(
            manifest.ExpectedInstructions.Count);
        foreach (var expected in manifest.ExpectedInstructions)
        {
            var actualFileOffset = region.FileOffset + expected.Offset - region.Origin;
            string stored = Convert.ToHexString(region.Source.Data.Slice(
                actualFileOffset, expected.OriginalHex.Length / 2));
            string detail;
            string status;
            if (!string.Equals(stored, expected.OriginalHex,
                    StringComparison.OrdinalIgnoreCase))
            {
                status = "conflict";
                detail = "external-oracle-bytes-disagree-with-original-file";
            }
            else if (!decoded.TryGetValue(expected.Offset, out var row))
            {
                status = "not-observed";
                detail = "source-backed-instruction-not-discovered";
            }
            else if (row.SourceFileOffset != expected.SourceFileOffset ||
                     row.Length != expected.OriginalHex.Length / 2 ||
                     !string.Equals(row.OriginalBytes, stored,
                         StringComparison.OrdinalIgnoreCase))
            {
                status = "conflict";
                detail = "candidate-instruction-location-length-or-bytes-disagree";
            }
            else if (!string.Equals(row.FlowControl, expected.FlowControl,
                         StringComparison.Ordinal))
            {
                status = "conflict";
                detail = "candidate-flow-category-disagrees-with-external-oracle";
            }
            else if (expected.DirectEdgeKind is not null &&
                     !graph.Edges.Any(e => e.Source ==
                            new Dos16Address(region.Segment, expected.Offset) &&
                         e.Destination ==
                            new Dos16Address(region.Segment, expected.DirectTarget!.Value) &&
                         e.Kind == expected.DirectEdgeKind))
            {
                status = "missing";
                detail = "externally-expected-direct-edge-not-found";
            }
            else
            {
                status = "match";
                detail = "source-and-declared-oracle-agree";
            }
            findings.Add(new Dos16ExternalOracleFinding(
                expected.Offset, status, detail, expected.EvidenceId));
        }

        var expectedOffsets = manifest.ExpectedInstructions
            .Select(e => e.Offset).ToHashSet();
        return new Dos16ExternalOracleReport(
            "dosdis-x86-16-external-oracle-comparison", SchemaVersion,
            region.SourceSha256, manifest.GameId, manifest.Module,
            manifest.DecoderName, manifest.DecoderVersion,
            manifest.OracleEvidenceId, "claimed-external-not-reviewed",
            "candidate-comparison-not-certification",
            graph.Truncated, findings.Count,
            findings.Count(f => f.Status == "match"),
            findings.Count(f => f.Status == "missing"),
            findings.Count(f => f.Status == "conflict"),
            findings.Count(f => f.Status == "not-observed"),
            decoded.Keys.Count(offset => !expectedOffsets.Contains(offset)),
            findings);
    }

    public static string BuildJson(Dos16ExternalOracleReport report)
    {
        ArgumentNullException.ThrowIfNull(report);
        return JsonSerializer.Serialize(report, JsonOptions);
    }

    /// <summary>Nonoverwriting local JSON export, never a release attestation.</summary>
    public static void WriteNew(string path, Dos16ExternalOracleReport report)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(path);
        var destination = Path.GetFullPath(path);
        if (!string.Equals(Path.GetExtension(destination), ".json",
                StringComparison.OrdinalIgnoreCase))
            throw new ArgumentException("External oracle report must have .json extension.");
        var directory = Path.GetDirectoryName(destination)!;
        if (!Directory.Exists(directory))
            throw new DirectoryNotFoundException(
                "External oracle export directory does not exist.");
        string output = BuildJson(report);
        if (Encoding.UTF8.GetByteCount(output) > MaxOutputBytes)
            throw new InvalidDataException(
                "External oracle result exceeds 2 MiB safety limit.");
        string staging = Path.Combine(directory, "." + Path.GetFileName(destination) +
            "." + Guid.NewGuid().ToString("N") + ".part");
        try
        {
            File.WriteAllText(staging, output, new UTF8Encoding(false));
            File.Move(staging, destination);
        }
        finally
        {
            if (File.Exists(staging)) File.Delete(staging);
        }
    }
}
